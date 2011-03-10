#include <stdio.h>
#include "consts.h"
#include "network.h"
#include "zip.h"
#include "window.h"

#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "ComDlg32.lib")
#pragma comment(lib, "winhttp.lib")

//int main(void){
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int nCmdShow){
	HANDLE h = CreateMutex(NULL,FALSE,"AIManager");
	if(WAIT_OBJECT_0 != WaitForSingleObject(h,0)){
		CloseHandle(h);
		return 0;
	}

	SetAppDir();
	CAIListDialog dlg;
	dlg.DoModal();
	ReleaseMutex(h);
	CloseHandle(h);
	return 0;
}


