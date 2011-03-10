#include <windows.h>
#include <time.h>
#include "consts.h"
#include "network.h"
#include "zip.h"
#include "resource.h"
#include "version.h"
#include "lib.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <atldlgs.h>
#include <atlctrlx.h>
#include <vector>

class AIData{
public:
	time_t time_i;
	char time_s[256];
	char date_s[256];
	int char_id;
	char sig[256];
	char name[256];
	char des[4096];
	char fn[256];

	bool SetData(const char *string){
		char data[4096],*s;
		strncpy(data,string,sizeof(data));
		int i = 0;
		while((s = strstr(data,"<>"))!=NULL){
			*s = '\0';
			if(i==0){
				strncpy(sig,data,sizeof(sig));
			} else if(i==1){
				char_id = atoi(data);
			} else if(i==2){
				strncpy(name,data,sizeof(name));
			} else if(i==3){
				strncpy(des,data,sizeof(des));
			} else if(i==4){
				strncpy(fn,data,sizeof(fn));
			} else if(i==5){
				time_i = atoi(data);
				struct tm *t_st;
				t_st = localtime(&time_i);
				sprintf(time_s,"%4d/%02d/%02d",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday);
				sprintf(date_s,"%4d/%02d/%02d %02d:%02d:%02d",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
			}
			memmove(data,&s[2],strlen(&s[2])+1);
			i++;
		}
		if(i<6){
			return false;
		}
		while((s=strstr(des,"\\n"))!=NULL){
			s[0] = '\r';
			s[1] = '\n';
		}
		while((s=strstr(des,"\\\\"))!=NULL){
			memmove(s,&s[1],strlen(&s[1])+1);
		}
		return true;
	}

	bool GetLocalDirPath(char *path){
		char dir[256];
		GetCurrentDirectory(256,dir);
		sprintf(path,"%s\\script\\%s\\",dir,sig);
		return true;
	}

	bool GetLocalPath(char *path){
		char dir[256];
		GetLocalDirPath(dir);
		sprintf(path,"%s%s",dir,fn);
		return true;
	}

	bool CheckLocal(){
		char path[256];
		GetLocalPath(path);

		HANDLE h;
		h = CreateFile(path,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(h == INVALID_HANDLE_VALUE){
			return false;
		}
		CloseHandle(h);
		return true;
	}

	bool CheckLatest(){
		char path[256];
		GetLocalPath(path);

		HANDLE h;
		FILETIME ft;
		time_t t;
		h = CreateFile(path,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(h == INVALID_HANDLE_VALUE){
			return false;
		}
		GetFileTime(h,NULL,NULL,&ft);
		CloseHandle(h);
		t = static_cast<time_t>((*(ULONGLONG *)&ft-0x19DB1DED53E8000)/10000000);
		if(t < time_i){
			return false;
		}
		return true;
	}

	bool DeleteLocal(){
		char path[256];
		GetLocalDirPath(path);
		return DeleteDirectory(path);
	}

	const char *GetLocalStatus(){
		static const char *ret[] = {
			"○",	"△",	"×"
		};
		int i;
		if(!CheckLocal()){
			i = 2;
		} else if(!CheckLatest()){
			i = 1;
		} else {
			i = 0;
		}
		return ret[i];
	}

	void GetDownloadUrl(char *buf){
		sprintf(buf,"%s%s/%s.zip",URL_AISAVEDIR,id2char2(char_id),sig);
	}

	bool DownloadZip(const char *filename){
		char url[512];
		int size;

		GetDownloadUrl(url);
		char *zip = get(url,&size);
		if(zip==NULL || zip[0]=='\0'){
			return false;
		}
		FILE *fp = fopen(filename,"wb");
		if(fp==NULL){
			free(zip);
			return false;
		}
		fwrite(zip,1,size,fp);
		fclose(fp);
		free(zip);
		return true;
	}

	bool DecompressionZip(const char *filename){
		CreateDirectory("script",NULL);
		char dir[256];
		sprintf(dir,"script/%s",sig);
		CreateDirectory(dir,NULL);
		Zip2Dir(dir,filename);
		return true;
	}

	bool CallTH123AI(const char *filename){
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		char cmd[512];
		sprintf(cmd,"th123_ai.exe \"%s\" -signature %s -char %d",filename,sig,char_id);

		memset(&si,0,sizeof(si));
		memset(&pi,0,sizeof(pi));
		si.cb = sizeof(si);
		if(CreateProcess(NULL,cmd,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi)){
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return true;
		}
		return false;
	}

	bool Run(bool down_flag=true){
		char *error[10],**e;
		e = error;
		if(!CheckLatest()){
			if(!down_flag || DownloadZip("aiscript_tmp1234.zip")){
				DeleteLocal();
				if(!DecompressionZip("aiscript_tmp1234.zip")){
					*e = "ZIP形式AIの展開に失敗しました。\n";
					e++;
				}
				DeleteFile("aiscript_tmp1234.zip");
			} else {
				*e = "AIのダウンロードに失敗しました。\n";
				e++;
			}
		}
		char filename[256];
		GetLocalPath(filename);

		HANDLE h;
		h = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(h == INVALID_HANDLE_VALUE){
			*e = "該当するAIがローカルに存在しません。\nAIの起動に失敗しました。\n";
			e++;
			int i = 0;
			char text[2048]="";
			while(&error[i] != e){
				strcat(text,error[i]);
				i++;
			}
			MessageBox(NULL, text, "エラー", MB_ICONERROR | MB_OK);
			return false;
		}
		CloseHandle(h);

		CallTH123AI(filename);
		return true;
	}
};

class AIList{
private:
	std::vector<AIData> list;
public:
	bool SetData(const char *string){
		char *data,*s,*p;
		data = static_cast<char *>(malloc(strlen(string)+1));
		strcpy(data,string);
		p = data;
		AIData col;
		list.clear();
		while((s=strstr(p,"\n"))!=NULL){
			*s = '\0';
			col.SetData(p);
			list.push_back(col);
			p = s+1;
		}
		free(data);
		return true;
	}

	int GetSize(void){
		return list.size();
	}

	std::vector<AIData>::iterator Begin(void){
		return list.begin();
	}

	std::vector<AIData>::iterator End(void){
		return list.end();
	}
};

class CAIZipConfigDialog : public CDialogImpl<CAIZipConfigDialog>
{
public:
	enum { IDD = IDD_POSTAIDLG };

	CComboBox char_list;
	CEdit signature;
	CEdit display_name;
	CEdit description;
	CEdit script_path;
	CEdit script_root;

	BEGIN_MSG_MAP(CAIZipConfigDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_PUT_FILE, OnPutFile)
		COMMAND_ID_HANDLER_EX(IDC_SEND, OnSend)
		COMMAND_ID_HANDLER_EX(IDC_TEST_RUN, OnTestRun)
		COMMAND_ID_HANDLER_EX(IDC_REFER, OnRefer)
		COMMAND_ID_HANDLER_EX(IDC_DELETE, OnDelete)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam){
		char_list = GetDlgItem(IDC_CHAR_ID);
		int i = 0;
		while(i < 21){
			char_list.AddString(id2char(i));
			char_list.SetItemData(i,i);
			i++;
		}
		char_list.SetCurSel(0);

		signature = GetDlgItem(IDC_SIGNATURE);
		display_name = GetDlgItem(IDC_DISPLAY_NAME);
		description = GetDlgItem(IDC_DESCRIPTION);
		script_path = GetDlgItem(IDC_PATH);
		script_root = GetDlgItem(IDC_ROOT);

		FILE *fp = fopen("AIPostSetting.glvn","rb");
		if(fp!=NULL){
			int size;
			char *data,*s;
			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fseek(fp,0,SEEK_SET);
			data = static_cast<char*>(malloc(size+1));
			fread(data,1,size,fp);
			fclose(fp);
			data[size] = '\0';

			int char_id;
			char sig[256];
			char name[256];
			char des[4096];
			char path[256];
			char fn[256];
			i = 0;
			while((s = strstr(data,"<>"))!=NULL){
				*s = '\0';
				if(i==0){
					char_id = atoi(data);
				} else if(i==1){
					strncpy(sig,data,sizeof(sig));
				} else if(i==2){
					strncpy(name,data,sizeof(name));
				} else if(i==3){
					strncpy(des,data,sizeof(des));
				} else if(i==4){
					strncpy(path,data,sizeof(path));
				} else if(i==5){
					strncpy(fn,data,sizeof(fn));
				}
				memmove(data,&s[2],strlen(&s[2])+1);
				i++;
			}
			free(data);

			i = 0;
			while(i < 21){
				if(char_list.GetItemData(i)==char_id){
					char_list.SetCurSel(i);
					break;
				}
				i++;
			}
			signature.SetWindowText(sig);
			display_name.SetWindowText(name);
			description.SetWindowText(des);
			script_path.SetWindowText(path);
			script_root.SetWindowText(fn);
		}
		return TRUE;
	}

	void OnRefer(UINT uNotifyCode, int nID, HWND hWndCtl){
		CFileDialog dlg(TRUE, "ai", NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT,
			_T("AIスクリプト(*.ai)\0*.ai\0すべてのファイル (*.*)\0*.*\0\0"));
		if(IDOK == dlg.DoModal()){
			char path[256];
			char file[256];
			strcpy(path,dlg.m_szFileName);
			int i = strlen(path);
			while(i>0 && path[i]!='\\'){
				i--;
			}
			strcpy(file,&path[i+1]);
			path[i] = '\0';
			script_path.SetWindowText(path);
			script_root.SetWindowText(file);
		}
	}

	bool GetData(int &char_id,char *sig,char *name,char *des,char *path,char *fn){
		char *list[6],**p;
		list[0] = NULL;
		p = list;
		char_id = char_list.GetItemData(char_list.GetCurSel());
		if(char_id<0 || char_id>21){
			*p = "対象キャラクターが不正です\n";
			p++;
		}
		signature.GetWindowText(sig,256);
		if(strlen(sig)<4 || strlen(sig)>8 || strstr(sig,"<>")!=NULL){
			*p = "識別名は4～8文字のみです。\n";
			p++;
		}
		display_name.GetWindowText(name,256);
		if(strlen(name)<1 || strstr(name,"<>")!=NULL){
			*p = "表示名が未入力であるか、禁止文字が含まれています。\n";
			p++;
		}
		description.GetWindowText(des,4096);
		if(strstr(des,"<")!=NULL || strstr(des,">")!=NULL){
			*p = "説明文中に<>は含められません\n";
			p++;
		}
		script_path.GetWindowText(path,256);
		if(strlen(path)<1){
			*p = "フォルダ位置が未設定です。\n";
			p++;
		} else {
			char dir_path[257];
			strcpy(dir_path,path);
			strcat(dir_path,"\\");
			if(PathFileExists(path)!=TRUE || PathIsDirectoryA(path)==FALSE){
				*p = "存在しないパスがフォルダ位置に指定されています。\n";
				p++;
			}
		}
		script_root.GetWindowText(fn,256);
		if(strlen(fn)<1 || strstr(fn,"<>")!=NULL){
			*p = "ルートスクリプト名が未設定であるか、禁止文字列が含まれています。\n";
			p++;
		} else {
			char full_path[512];
			sprintf(full_path,"%s\\%s",path,fn);
			if(PathFileExists(full_path)!=TRUE || PathIsDirectory(full_path)!=FALSE){
				*p = "存在しないファイルパスがルートスクリプト名に指定されています。\n";
				p++;
			}
		}
		if(list != p){
			char str[1024] = "";
			int i = 0;
			while(&list[i] < p){
				strcat(str,list[i]);
				i++;
			}
			MessageBox(str, "エラー", MB_ICONERROR | MB_OK);
			return false;
		}
		return true;
	}
	void OnPutFile(UINT uNotifyCode, int nID, HWND hWndCtl){
		int char_id;
		char sig[256];
		char name[256];
		char des[4096];
		char path[256];
		char fn[256];

		SetAppDir();
		if(!GetData(char_id,sig,name,des,path,fn)){
			MessageBox("保存処理に失敗しました", "エラー", MB_ICONERROR | MB_OK);
			return;
		}
		FILE *fp = fopen("AIPostSetting.glvn","wb");
		if(fp==NULL){
			MessageBox("ファイルのオープンに失敗しました", "エラー", MB_ICONERROR | MB_OK);
			return;
		}
		fprintf(fp,"%d<>%s<>%s<>%s<>%s<>%s<>",char_id,sig,name,des,path,fn);
		fclose(fp);
		MessageBox("保存処理に成功しました", "情報", MB_ICONINFORMATION);
		return;
	}
	void OnSend(UINT uNotifyCode, int nID, HWND hWndCtl){
		int char_id;
		char sig[256];
		char name[256];
		char des[4096];
		char path[256];
		char fn[256];

		SetAppDir();
		if(!GetData(char_id,sig,name,des,path,fn)){
			MessageBox("送信処理に失敗しました", "エラー", MB_ICONERROR | MB_OK);
			return;
		}

		Dir2Zip(path);
		if(AIZipPost(URL_AIPOSTCGI,"aiscript_tmp1234.zip",char_id,sig,name,des,fn)){
			MessageBox("送信処理に成功しました", "情報", MB_ICONINFORMATION);
		} else {
			MessageBox("サーバーがエラーを返しました", "エラー", MB_ICONERROR | MB_OK);
		}
		DeleteFile("aiscript_tmp1234.zip");
	}
	void OnTestRun(UINT uNotifyCode, int nID, HWND hWndCtl){
		int char_id;
		char sig[256];
		char name[256];
		char des[4096];
		char path[256];
		char fn[256];

		SetAppDir();
		if(!GetData(char_id,sig,name,des,path,fn)){
			MessageBox("テスト実行に失敗しました", "エラー", MB_ICONERROR | MB_OK);
			return;
		}

		AIData data;
		{
			char str[6000];
			sprintf(str,"test_run<>%d<>%s<>%s<>%s<>%d<>",char_id,name,des,fn,time(NULL));
			printf("SetData(%s)\n",str);
			data.SetData(str);
		}
		Dir2Zip(path);
		data.Run(false);
	}
	void OnDelete(UINT uNotifyCode, int nID, HWND hWndCtl){
		if(MessageBox("このAIを削除します。\n本当によろしいですか？","確認",MB_ICONQUESTION | MB_OKCANCEL) != IDOK){
			return;
		}
		int char_id;
		char sig[256];
		char name[256];
		char des[4096];
		char path[256];
		char fn[256];

		SetAppDir();
		if(!GetData(char_id,sig,name,des,path,fn)){
			MessageBox("送信処理に失敗しました", "エラー", MB_ICONERROR | MB_OK);
			return;
		}

		if(PostDeleteMethod(URL_AIPOSTCGI,char_id,sig,name,des,fn)){
			MessageBox("削除処理に成功しました", "情報", MB_ICONINFORMATION);
		} else {
			MessageBox("サーバーがエラーを返しました", "エラー", MB_ICONERROR | MB_OK);
		}
	}

	void OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl){
		EndDialog(nID);
	}
};

class CDescriptionTab : public CPropertyPageImpl<CDescriptionTab>
{
public:
	enum { IDD = IDD_DESCRIPTIONDLG };

	AIData *data;
	CHyperLink download_url;

	CDescriptionTab(AIData *p){
		data = p;
	}

	BEGIN_MSG_MAP(CDescriptionTab)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CDescriptionTab>)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam){
		CEdit name = GetDlgItem(IDC_NAME);
		CEdit time = GetDlgItem(IDC_TIME);
		CEdit c_name = GetDlgItem(IDC_CHAR);
		CEdit des = GetDlgItem(IDC_DESCRIPTION);

		name.SetWindowText(data->name);
		time.SetWindowText(data->date_s);
		c_name.SetWindowText(id2char(data->char_id));
		des.SetWindowText(data->des);

		{
			char url[512];
			data->GetDownloadUrl(url);
			download_url.SubclassWindow(GetDlgItem(IDC_URL));
			download_url.SetLabel(url);
			download_url.SetHyperLink(url);
		}
		return TRUE;
	}

};

class CErrorReportTab : public CPropertyPageImpl<CErrorReportTab>
{
public:
	enum { IDD = IDD_ERRORDISPLAYDIALOG };

	AIData *data;
	CErrorReportTab(AIData *p){
		data = p;
	}

	BEGIN_MSG_MAP(CErrorReportTab)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CErrorReportTab>)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam){
		char url[256];
		sprintf(url,"%s%s",URL_ERRORDIR,data->sig);
		char *str = get(url);
		CString body(str);
		free(str);
		body.Replace("\r\n","\n");
		body.Replace("\r","\n");
		body.Replace("\n","\r\n");
		CEdit des = GetDlgItem(IDC_DESCRIPTION);
		des.SetWindowText(body);
		return TRUE;
	}
};
/**
 * ダイアログ本体
 */
class CAIDescriptionDialog : public CPropertySheetImpl<CAIDescriptionDialog>
{
public:
	CDescriptionTab desc;
	CErrorReportTab er;

	// コンストラクタ
	CAIDescriptionDialog(AIData *p,
		ATL::_U_STRINGorID title = _T("詳細表示"),
		UINT uStartPage = 0, HWND hWndParent = NULL)
			: CPropertySheetImpl<CAIDescriptionDialog>(title, uStartPage, hWndParent),
			desc(p),er(p)
	{
		AddPage(desc);
		AddPage(er);
		m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
	}

	BEGIN_MSG_MAP_EX(CAIDescriptionDialog)
		CHAIN_MSG_MAP(CPropertySheetImpl<CAIDescriptionDialog>)
	END_MSG_MAP()
};

class CAIListDialog : public CDialogImpl<CAIListDialog>
{
public:
	enum { IDD = IDD_MAINDLG };

	AIList data_list;

	CSortListViewCtrl list_view;
	CButton bt_list[21];
	CButton exec_bt;
	CButton all_on_bt;
	CButton all_off_bt;

	bool no_player_mode;
	CButton mode_bt;
	CStatic player_str[2];
	CEdit player_ai[2];
	AIData *no_player[2];

	BEGIN_MSG_MAP(CAIListDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_AI_REGIST, OnAIRegist)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDOK, OnExec)
		COMMAND_ID_HANDLER_EX(IDC_ALL_ON, OnAllOn)
		COMMAND_ID_HANDLER_EX(IDC_ALL_OFF, OnAllOff)
        	NOTIFY_HANDLER_EX(IDC_AILIST, NM_DBLCLK, OnListDblClick)
		NOTIFY_HANDLER_EX(IDC_AILIST, LVN_ITEMCHANGED, OnSelect)
		COMMAND_ID_HANDLER_EX(IDC_REIMU,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_MARISA,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_SAKUYA,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_ALICE,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_PATCHOULI,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_YOUMU,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_REMILIA,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_YUYUKO,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_YUKARI,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_SUIKA,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_UDONGE,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_AYA,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_KOMACHI,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_IKU,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_TENSHI,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_SANAE,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_CHIRNO,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_MEIRIN,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_UTUHO,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_SUWAKO,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_OTHER,OnChange)
		COMMAND_ID_HANDLER_EX(IDC_AIBATTLE,OnAIBattle)
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam){
		no_player_mode = false;
		player_str[0] = GetDlgItem(IDC_STATIC1P);
		player_str[1] = GetDlgItem(IDC_STATIC2P);
		player_ai[0] = GetDlgItem(IDC_EDIT1P);
		player_ai[1] = GetDlgItem(IDC_EDIT2P);
		mode_bt = GetDlgItem(IDC_AIBATTLE);

		exec_bt = GetDlgItem(IDOK);
		all_on_bt = GetDlgItem(IDC_ALL_ON);
		all_off_bt = GetDlgItem(IDC_ALL_OFF);
        	list_view.SubclassWindow(GetDlgItem(IDC_AILIST));
		list_view.SetExtendedListViewStyle(list_view.GetExtendedListViewStyle() | LVS_EX_FULLROWSELECT);
		list_view.SendMessage(TTM_SETMAXTIPWIDTH,0,120);

		{
			char title[256];
			GetWindowText(title,256);
			sprintf(&title[strlen(title)]," %s",OPEN_VERSION);
			SetWindowText(title);
		}

		{
			int idc_list[] = {
				IDC_REIMU,	IDC_MARISA,	IDC_SAKUYA,	IDC_ALICE,
				IDC_PATCHOULI,	IDC_YOUMU,	IDC_REMILIA,	IDC_YUYUKO,
				IDC_YUKARI,	IDC_SUIKA,	IDC_UDONGE,	IDC_AYA,
				IDC_KOMACHI,	IDC_IKU,	IDC_TENSHI,	IDC_SANAE,
				IDC_CHIRNO,	IDC_MEIRIN,	IDC_UTUHO,	IDC_SUWAKO,
				IDC_OTHER};
			int idc_max = 21;
			int i = 0;
			while(i < idc_max){
				bt_list[i] = GetDlgItem(idc_list[i]);
				i++;
			}
		}

		{
			CRect rcList;
			list_view.GetWindowRect(rcList);
			int nScrollWidth = GetSystemMetrics(SM_CXVSCROLL);
			int n3DEdge = GetSystemMetrics(SM_CXEDGE);
			int width[5] = {20,80,100,70,150};
			CString name[5] = {"","登録日","名称","キャラクター","解説"};
			width[4] = rcList.Width() - (width[0]+width[1]+width[2]+width[3]) - nScrollWidth - n3DEdge * 2;
			int i=0;
			while(i < 5){
				list_view.InsertColumn(i, name[i], LVCFMT_LEFT, width[i], -1);
				list_view.SetColumnSortType(i, LVCOLSORT_TEXT);
				i++;
			}
		}
		ReloadAIListData();
		OnAllOn(0,0,NULL);
		ChangeMode(no_player_mode);
		return TRUE;
	}

	bool GetCheckList(bool *check_list){
		int i = 0;
		BOOL on=FALSE,off=FALSE;
		while(i < 21){
			check_list[i] = (bt_list[i].GetCheck() == BST_CHECKED);
			if(check_list[i]){
				off = TRUE;
			} else {
				on = TRUE;
			}
			i++;
		}
		all_on_bt.EnableWindow(on);
		all_off_bt.EnableWindow(off);
		return true;
	}

	int set_listview(void){
		int pos = list_view.GetSelectedIndex();
		list_view.DeleteAllItems();
		if(data_list.GetSize()<1){
			return 0;
		}
		bool check_list[21];
		GetCheckList(check_list);
		std::vector<AIData>::iterator p = data_list.Begin();
		while(p != data_list.End()){
			int nIndex = list_view.GetItemCount();
			if(check_list[p->char_id]){
				list_view.AddItem(nIndex, 0, p->GetLocalStatus());
				list_view.AddItem(nIndex, 1, p->time_s);
				list_view.AddItem(nIndex, 2, p->name);
				list_view.AddItem(nIndex, 3, id2char(p->char_id));
				list_view.AddItem(nIndex, 4, p->des);
				list_view.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(&*p));
			}
			p++;
		}
		list_view.SelectItem(pos);
		OnSelect(NULL);
		return list_view.GetItemCount();
	}
	void ReloadAIListData(void){
		char *temp = get(URL_AILISTGLVN);
		if(temp!=NULL && temp[0]!='\0'){
			data_list.SetData(temp);
			FILE *fp = fopen("list.glvn","wb");
			if(fp!=NULL){
				fwrite(temp,1,strlen(temp),fp);
				fclose(fp);
			}
			free(temp);
		} else {
			FILE *fp = fopen("list.glvn","rb");
			if(fp!=NULL){
				fseek(fp,0,SEEK_END);
				int size = ftell(fp);
				fseek(fp,0,SEEK_SET);
				char *data = static_cast<char*>(malloc(size+1));
				fread(data,1,size,fp);
				fclose(fp);
				data[size] = '\0';
				data_list.SetData(data);
				free(data);
			}
		}
		set_listview();
	}
	LRESULT OnListDblClick(LPNMHDR pnmh){
		int pos = list_view.GetSelectedIndex();
		if(pos==-1){
			return 0;
		}
		AIData *data = reinterpret_cast<AIData*>(list_view.GetItemData(list_view.GetSelectedIndex()));
		CAIDescriptionDialog dlg(data);
		if(dlg.DoModal() == IDOK){
			OnExec(0,0,0);
		}
		return 0;
	}
	void change_execbt(){
		int pos = list_view.GetSelectedIndex();
		if(no_player_mode){
			if(no_player[0]==NULL){
				exec_bt.SetWindowText("1Pに登録");
			} else if(no_player[1]==NULL){
				exec_bt.SetWindowText("2Pに登録");
			} else {
				exec_bt.SetWindowText("起動する");
			}
			if(pos==-1 && no_player[1]==NULL){
				exec_bt.EnableWindow(FALSE);
			} else {
				exec_bt.EnableWindow(TRUE);
			}
		} else {
			exec_bt.SetWindowText("起動する");
			if(pos==-1){
				exec_bt.EnableWindow(FALSE);
			} else {
				exec_bt.EnableWindow(TRUE);
			}
		}
	}
	LRESULT OnSelect(LPNMHDR pnmh){
		change_execbt();
		return 0;
	}
	void OnChange(UINT uNotifyCode, int nID, HWND hWndCtl){
		set_listview();
	}
	void OnAllOn(UINT uNotifyCode, int nID, HWND hWndCtl){
		int i = 0;
		while(i < 21){
			bt_list[i].SetCheck(BST_CHECKED);
			i++;
		}
		OnChange(uNotifyCode,nID,hWndCtl);
	}
	void OnAllOff(UINT uNotifyCode, int nID, HWND hWndCtl){
		int i = 0;
		while(i < 21){
			bt_list[i].SetCheck(BST_UNCHECKED);
			i++;
		}
		OnChange(uNotifyCode,nID,hWndCtl);
	}
	void OnAIRegist(UINT uNotifyCode, int nID, HWND hWndCtl){
		CAIZipConfigDialog dlg;
		dlg.DoModal();
		ReloadAIListData();
		list_view.SetFocus();
	}
	void ChangeMode(bool flag){
		RECT rc;
		rc.left = 11;
		rc.top = 107;
		rc.right = rc.left + 546;
		rc.bottom = rc.top + 228;
		no_player_mode = flag;
		if(no_player_mode){
			rc.top += 20;
			player_str[0].ShowWindow(SW_SHOW);
			player_str[1].ShowWindow(SW_SHOW);
			player_ai[0].ShowWindow(SW_SHOW);
			player_ai[1].ShowWindow(SW_SHOW);
			mode_bt.SetWindowText("ノーマルモードに変える");
		} else {
			no_player[0] = no_player[1] = NULL;
			player_str[0].ShowWindow(SW_HIDE);
			player_str[1].ShowWindow(SW_HIDE);
			player_ai[0].ShowWindow(SW_HIDE);
			player_ai[1].ShowWindow(SW_HIDE);
			player_ai[0].SetWindowText("");
			player_ai[1].SetWindowText("");
			mode_bt.SetWindowText("AIvsAIモードに変える");
		}
		list_view.MoveWindow(&rc);
		change_execbt();
	}
	void OnAIBattle(UINT uNotifyCode, int nID, HWND hWndCtl){
		ChangeMode(!no_player_mode);
		list_view.SetFocus();
	}
	void OnExec(UINT uNotifyCode, int nID, HWND hWndCtl){
		int pos = list_view.GetSelectedIndex();
		if(no_player_mode){
			if(no_player[1]==NULL && pos==-1){
				return;
			}
			AIData *data = reinterpret_cast<AIData*>(list_view.GetItemData(list_view.GetSelectedIndex()));
			if(no_player[0]==NULL){
				no_player[0] = data;
				player_ai[0].SetWindowText(data->name);
			} else if(no_player[1]==NULL){
				no_player[1] = data;
				player_ai[1].SetWindowText(data->name);
			} else {
				no_player[0]->Run();
				no_player[1]->Run();
				ReloadAIListData();
				ChangeMode(false);
			}
			change_execbt();
		} else {
			if(pos==-1){
				return;
			}
			AIData *data = reinterpret_cast<AIData*>(list_view.GetItemData(list_view.GetSelectedIndex()));
			data->Run();
			ReloadAIListData();
		}
		list_view.SetFocus();
	}
	void OnCancel(UINT uNotifyCode, int nID, HWND hWndCtl){
		EndDialog(IDCANCEL);
	}
};


