#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "zlib.h"
#include "zip.h"
//#include "network.h"

unsigned int cd_size;//CentralDirectoryのサイズ
unsigned int cd_addr;//CentrakDurectoryの位置

// crc32_startでは、前のバッファブロックのCRC32値をここに指定する。
// バッファが単一であるのなら、個々に引数は指定しなくてよい(0xFFFFFFFFが初期値)。
unsigned int GetCRC32(const unsigned char *buffer, unsigned int bufferlen, unsigned int crc32_start){
	static unsigned int table[256] = {
		0x00000000,	0x77073096,	0xee0e612c,	0x990951ba,	0x076dc419,	0x706af48f,
		0xe963a535,	0x9e6495a3,	0x0edb8832,	0x79dcb8a4,	0xe0d5e91e,	0x97d2d988,
		0x09b64c2b,	0x7eb17cbd,	0xe7b82d07,	0x90bf1d91,	0x1db71064,	0x6ab020f2,
		0xf3b97148,	0x84be41de,	0x1adad47d,	0x6ddde4eb,	0xf4d4b551,	0x83d385c7,
		0x136c9856,	0x646ba8c0,	0xfd62f97a,	0x8a65c9ec,	0x14015c4f,	0x63066cd9,
		0xfa0f3d63,	0x8d080df5,	0x3b6e20c8,	0x4c69105e,	0xd56041e4,	0xa2677172,
		0x3c03e4d1,	0x4b04d447,	0xd20d85fd,	0xa50ab56b,	0x35b5a8fa,	0x42b2986c,
		0xdbbbc9d6,	0xacbcf940,	0x32d86ce3,	0x45df5c75,	0xdcd60dcf,	0xabd13d59,
		0x26d930ac,	0x51de003a,	0xc8d75180,	0xbfd06116,	0x21b4f4b5,	0x56b3c423,
		0xcfba9599,	0xb8bda50f,	0x2802b89e,	0x5f058808,	0xc60cd9b2,	0xb10be924,
		0x2f6f7c87,	0x58684c11,	0xc1611dab,	0xb6662d3d,	0x76dc4190,	0x01db7106,
		0x98d220bc,	0xefd5102a,	0x71b18589,	0x06b6b51f,	0x9fbfe4a5,	0xe8b8d433,
		0x7807c9a2,	0x0f00f934,	0x9609a88e,	0xe10e9818,	0x7f6a0dbb,	0x086d3d2d,
		0x91646c97,	0xe6635c01,	0x6b6b51f4,	0x1c6c6162,	0x856530d8,	0xf262004e,
		0x6c0695ed,	0x1b01a57b,	0x8208f4c1,	0xf50fc457,	0x65b0d9c6,	0x12b7e950,
		0x8bbeb8ea,	0xfcb9887c,	0x62dd1ddf,	0x15da2d49,	0x8cd37cf3,	0xfbd44c65,
		0x4db26158,	0x3ab551ce,	0xa3bc0074,	0xd4bb30e2,	0x4adfa541,	0x3dd895d7,
		0xa4d1c46d,	0xd3d6f4fb,	0x4369e96a,	0x346ed9fc,	0xad678846,	0xda60b8d0,
		0x44042d73,	0x33031de5,	0xaa0a4c5f,	0xdd0d7cc9,	0x5005713c,	0x270241aa,
		0xbe0b1010,	0xc90c2086,	0x5768b525,	0x206f85b3,	0xb966d409,	0xce61e49f,
		0x5edef90e,	0x29d9c998,	0xb0d09822,	0xc7d7a8b4,	0x59b33d17,	0x2eb40d81,
		0xb7bd5c3b,	0xc0ba6cad,	0xedb88320,	0x9abfb3b6,	0x03b6e20c,	0x74b1d29a,
		0xead54739,	0x9dd277af,	0x04db2615,	0x73dc1683,	0xe3630b12,	0x94643b84,
		0x0d6d6a3e,	0x7a6a5aa8,	0xe40ecf0b,	0x9309ff9d,	0x0a00ae27,	0x7d079eb1,
		0xf00f9344,	0x8708a3d2,	0x1e01f268,	0x6906c2fe,	0xf762575d,	0x806567cb,
		0x196c3671,	0x6e6b06e7,	0xfed41b76,	0x89d32be0,	0x10da7a5a,	0x67dd4acc,
		0xf9b9df6f,	0x8ebeeff9,	0x17b7be43,	0x60b08ed5,	0xd6d6a3e8,	0xa1d1937e,
		0x38d8c2c4,	0x4fdff252,	0xd1bb67f1,	0xa6bc5767,	0x3fb506dd,	0x48b2364b,
		0xd80d2bda,	0xaf0a1b4c,	0x36034af6,	0x41047a60,	0xdf60efc3,	0xa867df55,
		0x316e8eef,	0x4669be79,	0xcb61b38c,	0xbc66831a,	0x256fd2a0,	0x5268e236,
		0xcc0c7795,	0xbb0b4703,	0x220216b9,	0x5505262f,	0xc5ba3bbe,	0xb2bd0b28,
		0x2bb45a92,	0x5cb36a04,	0xc2d7ffa7,	0xb5d0cf31,	0x2cd99e8b,	0x5bdeae1d,
		0x9b64c2b0,	0xec63f226,	0x756aa39c,	0x026d930a,	0x9c0906a9,	0xeb0e363f,
		0x72076785,	0x05005713,	0x95bf4a82,	0xe2b87a14,	0x7bb12bae,	0x0cb61b38,
		0x92d28e9b,	0xe5d5be0d,	0x7cdcefb7,	0x0bdbdf21,	0x86d3d2d4,	0xf1d4e242,
		0x68ddb3f8,	0x1fda836e,	0x81be16cd,	0xf6b9265b,	0x6fb077e1,	0x18b74777,
		0x88085ae6,	0xff0f6a70,	0x66063bca,	0x11010b5c,	0x8f659eff,	0xf862ae69,
		0x616bffd3,	0x166ccf45,	0xa00ae278,	0xd70dd2ee,	0x4e048354,	0x3903b3c2,
		0xa7672661,	0xd06016f7,	0x4969474d,	0x3e6e77db,	0xaed16a4a,	0xd9d65adc,
		0x40df0b66,	0x37d83bf0,	0xa9bcae53,	0xdebb9ec5,	0x47b2cf7f,	0x30b5ffe9,
		0xbdbdf21c,	0xcabac28a,	0x53b39330,	0x24b4a3a6,	0xbad03605,	0xcdd70693,
		0x54de5729,	0x23d967bf,	0xb3667a2e,	0xc4614ab8,	0x5d681b02,	0x2a6f2b94,
		0xb40bbe37,	0xc30c8ea1,	0x5a05df1b,	0x2d02ef8d
	};
	unsigned int result = crc32_start;
	for(unsigned int i = 0; i < bufferlen; i++){
		result = (result >> 8) ^ table[buffer[i] ^ (result & 0xFF)];
	}
	return ~result;
}
void SetAppDir(void){
	int i;
	char AppDir[256];

	GetModuleFileName(NULL,AppDir,256);
	i = strlen(AppDir)-1;
	while(AppDir[i] != '\\' && AppDir[i] != '/' && i > 0)i--;
	AppDir[i] = '\0';
	SetCurrentDirectory(AppDir);
}
bool GetFileDosTime(const char *name,WORD *fdate,WORD *ftime){
	HANDLE h;
	FILETIME ft,ft2;

	h = CreateFile(name,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	GetFileTime(h,NULL,NULL,&ft);
	CloseHandle(h);
	FileTimeToLocalFileTime(&ft,&ft2);
	FileTimeToDosDateTime(&ft2,fdate,ftime);
	return true;
}
WORD GetFileTime(const char *name){
	WORD fdate,ftime;
	GetFileDosTime(name,&fdate,&ftime);
	return ftime;
}
WORD GetFileDate(const char *name){
	WORD fdate,ftime;
	GetFileDosTime(name,&fdate,&ftime);
	return fdate;
}

bool Compress(unsigned char *in,DWORD &size ,WORD &type){
	z_stream z;
	char *out = static_cast<char*>(malloc(sizeof(char)*size));
	int count, flush, status;

	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	if(deflateInit2(&z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 6, Z_DEFAULT_STRATEGY) != Z_OK){
		return false;
	}

	z.next_out = reinterpret_cast<Bytef*>(out);
	z.avail_out = size;

	z.next_in = reinterpret_cast<Bytef*>(in);
	z.avail_in = size;
	status = deflate(&z, Z_FINISH);
	if(status!=Z_STREAM_END && status!=Z_OK){
		return false;
	}

	if(deflateEnd(&z) != Z_OK){
		return false;
	}
	if(z.avail_out>0){
		size -= z.avail_out;
		memcpy(in,out,size);
		type = 0x08;//deflate
	} else {
		type=0x00;//未圧縮
	}
	free(out);
	return true;
}

bool DeCompress(unsigned char *in,DWORD &size,DWORD out_size){
	z_stream z;
	char *out = static_cast<char*>(malloc(sizeof(char)*out_size));
	int count, flush, status;

	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	if(inflateInit2(&z, -15) != Z_OK){
		return false;
	}
	z.avail_in = 0;
	z.next_out = reinterpret_cast<Bytef*>(out);
	z.avail_out = out_size;

	z.next_in = reinterpret_cast<Bytef*>(in);
	z.avail_in = size;
	status = inflate(&z, Z_FINISH);
	if(status!=Z_STREAM_END && status!=Z_OK){
		return false;
	}
	if(inflateEnd(&z) != Z_OK){
		return false;
	}

	size -= z.avail_out;
	memcpy(in,out,out_size);
	free(out);
	return true;
}

bool GetFileNameList(const char *dir_name, char **buf, int *num){
	HANDLE hSearch;
	WIN32_FIND_DATA fd;
	int count,t;
	char temp[256];
	bool ret;

	strncpy(temp,dir_name,sizeof(temp));
	strncat(temp,"/*",sizeof(temp));
	hSearch = FindFirstFile(temp, &fd );
	if(hSearch == INVALID_HANDLE_VALUE){
		return false;
	}
	count = 0;
	while(1){
		if(fd.cFileName[0] != '.'){
			sprintf(temp,"%s/%s",dir_name,fd.cFileName);
			t = *num-count;
			if(t < 0){
				t = 0;
			}
			ret = GetFileNameList(temp, buf, &t);
			if(ret && t > 0){
				count += t;
				if(buf!=NULL){
					buf += t;
				}
			} else {
				if(*num - count > 0){
					*buf = static_cast<char*>(malloc(sizeof(char)*strlen(temp)+1));
					strcpy(*buf,temp);
					buf++;
				}
				count++;
			}
		}
		if(!FindNextFile( hSearch, &fd )){
			if( GetLastError() == ERROR_NO_MORE_FILES ){
				break;
			}
		}
	}
	FindClose( hSearch );

	*num = count;
	return true;
}

int GetFileNum(const char *dir_name){
	int n=0;
	if(!GetFileNameList(dir_name,NULL,&n)){
		if(n == 0){
			return -1;
		}
	}
	return n;
}

struct local_file_header{
	DWORD sig;
	WORD ver;
	WORD flag;
	WORD comp;
	WORD ftime;
	WORD fdate;
	DWORD crc32;
	DWORD comp_size;
	DWORD orig_size;
	WORD name_len;
	WORD ex_len;
};

bool CreateLocalFileHeader(const char *name,FILE *fp){
	struct local_file_header lfh;
	unsigned char *data;

	if(fp==NULL || name==NULL || name[0]=='\0'){
		return false;
	}
	lfh.sig = 0x04034b50;
	lfh.ver = 20;
	lfh.flag = 0;
	lfh.comp = 0x08;//意味は無い
	lfh.ftime = GetFileTime(name);
	lfh.fdate = GetFileDate(name);
	FILE *fsize = fopen(name,"rb");
	fseek(fsize,0,SEEK_END);
	lfh.comp_size = lfh.orig_size = ftell(fsize);
	fseek(fsize,0,SEEK_SET);
	data = static_cast<unsigned char*>(malloc(sizeof(char)*lfh.orig_size));
	fread(data,1,lfh.orig_size,fsize);
	fclose(fsize);
	lfh.crc32 = GetCRC32(data, lfh.orig_size, 0xffffffff);
	Compress(data,lfh.comp_size,lfh.comp);
	free(data);
	lfh.name_len = strlen(name);
	lfh.ex_len = 0;
	fwrite(&lfh.sig,1,14,fp);
	fwrite(&lfh.crc32,1,16,fp);
	fwrite(name,1,lfh.name_len,fp);
	return true;
}
bool CreateDataDescriptor(const char *name,FILE *fp){
	//flag3がONの時だけでいいのでつけない
	return true;
}

bool AddLocalFile(const char *name, FILE *f){
	FILE *fp;
	unsigned char *data;
	DWORD size;

	if(name==NULL || name[0]=='\0' || f==NULL){
		return false;
	}

	if(!CreateLocalFileHeader(name,f)){
		return false;
	}
	fp = fopen(name,"rb");
	if(fp==NULL){
		return false;
	}
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	data = static_cast<unsigned char *>(malloc(size));
	fseek(fp,0,SEEK_SET);
	fread(data,1,size,fp);
	if(size!=0){
		WORD type;
		Compress(data,size,type);
		fwrite(data,1,size,f);
	}
	free(data);
	fclose(fp);
	if(!CreateDataDescriptor(name,f)){
		return false;
	}
	return true;
}

bool CreateArchiveDescryptionHeader(char **file_list, int num, FILE *fp){
	//暗号化をかけないので必要ない
	//使うならVer6.2以上
	return true;
}
bool CreateArchiveExtraDataRecord(char **file_list, int num, FILE *fp){
	//シグネチャは0x08064b50
	//暗号化をかけないので必要ない
	return true;
}

struct file_header{
	DWORD sig;
	WORD made;
	WORD need;
	WORD flag;
	WORD comp;
	WORD ftime;
	WORD fdate;
	DWORD crc32;
	DWORD comp_size;
	DWORD orig_size;
	WORD name_len;
	WORD ex_len;
	WORD comment_len;
	WORD disk;
	WORD inter_attr;
	DWORD ex_attr;
	DWORD offset;
};
bool CreateFileHeader(const char *name, unsigned int addr, FILE *fp){
	struct file_header fh;
	unsigned char *data;

	if(fp==NULL || name==NULL || name[0]=='\0'){
		return false;
	}
	fh.sig = 0x02014b50;
	fh.made = 20;
	fh.need = 20;
	fh.flag = 0;
	fh.comp = 0x08;//意味は無い
	fh.ftime = GetFileTime(name);
	fh.fdate = GetFileDate(name);
	FILE *fsize = fopen(name,"rb");
	fseek(fsize,0,SEEK_END);
	fh.orig_size = fh.comp_size = ftell(fsize);
	fseek(fsize,0,SEEK_SET);
	data = static_cast<unsigned char*>(malloc(sizeof(char)*fh.orig_size));
	fread(data,1,fh.orig_size,fsize);
	fclose(fsize);
	fh.crc32 = GetCRC32(data, fh.orig_size, 0xffffffff);
	Compress(data,fh.comp_size,fh.comp);
	free(data);
	fh.name_len = strlen(name);
	fh.ex_len = 0;
	fh.comment_len = 0;
	fh.disk = 0;
	fh.inter_attr = 1;
	fh.ex_attr = GetFileAttributes(name);
	fh.offset = addr;

	fwrite(&fh.sig,1,38,fp);
	fwrite(&fh.ex_attr,1,8,fp);
	fwrite(name,1,fh.name_len,fp);
	return true;
}
bool CreateDigitalSignature(char **file_list, int num, FILE *fp){
	//オプションヘッダなので必要ない
	return true;
}
bool CreateCentralDirectory(char **file_list, unsigned int *addr_list, int num, FILE *fp){
	cd_addr = ftell(fp);
	int i = 0;
	while(i < num){
		if(!CreateFileHeader(file_list[i], addr_list[i], fp)){
			return false;
		}
		i++;
	}
	if(!CreateDigitalSignature(file_list,num,fp)){
		return false;
	}
	cd_size = ftell(fp)-cd_addr;
	return true;
}
bool CreateZip64EndOfCentralDirectoryRecord(char **file_list, int num, FILE *fp){
	//必要なし
	return true;
}
bool CreateZip64EndOfCentralDirectoryLocator(char **file_list, int num, FILE *fp){
	//必要なし
	return true;
}

struct end_of_central_directory_record{
	DWORD sig;
	WORD disk;
	WORD start_disk;
	WORD disk_file_count;
	WORD total_file_count;
	DWORD cd_size;
	DWORD offset;
	WORD comment_len;
};
bool CreateEndOfCentralDirectoryRecord(char **file_list, int num, FILE *fp){
	struct end_of_central_directory_record ecdr;

	ecdr.sig = 0x06054b50;
	ecdr.disk = 0;
	ecdr.start_disk = 0;
	ecdr.disk_file_count = num;
	ecdr.total_file_count = num;
	ecdr.cd_size = cd_size;
	ecdr.offset = cd_addr;
	ecdr.comment_len = 0;
	fwrite(&ecdr,1,22,fp);
	return true;
}

bool CreateZipEndData(char **file_list, unsigned int *addr_list, int num, FILE *fp){
	if(!CreateArchiveDescryptionHeader(file_list,num,fp)){
		return false;
	}
	if(!CreateArchiveExtraDataRecord(file_list,num,fp)){
		return false;
	}
	if(!CreateCentralDirectory(file_list,addr_list,num,fp)){
		return false;
	}
	if(!CreateZip64EndOfCentralDirectoryRecord(file_list,num,fp)){
		return false;
	}
	if(!CreateZip64EndOfCentralDirectoryLocator(file_list,num,fp)){
		return false;
	}
	if(!CreateEndOfCentralDirectoryRecord(file_list,num,fp)){
		return false;
	}
	return true;
}

bool Dir2Zip(const char *dir_name, const char *zip_name){
	int num,i;
	char **file_list;
	unsigned int *addr_list;
	bool ret;
	char name[256];
	FILE *fp;
	char prev_dir[256],dir[256];

	GetCurrentDirectory(256, prev_dir);
	strcpy(dir,dir_name);
	i = strlen(dir);
	while(i>0 && dir[i]!='\\'){
		i--;
	}
	if(i>0){
		dir[i] = '\0';
		SetCurrentDirectory(dir);
		strcpy(dir,&dir_name[i+1]);
	}

	num = GetFileNum(dir);
	if(num < 0){
		printf("Error:GetFileNum\n");
		return false;
	}

	file_list = static_cast<char**>(malloc(sizeof(char*)*num));
	addr_list = static_cast<unsigned int*>(malloc(sizeof(int)*num));
	if(file_list == NULL){
		printf("Error:malloc\n");
		return false;
	}
	ret = GetFileNameList(dir, file_list, &num);
	if(!ret){
		printf("Error:GetFileNameList\n");
		goto free_return;
	}

	if(zip_name==NULL){
		zip_name = "aiscript_tmp1234.zip";
	}
	sprintf(name,"%s\\%s",prev_dir,zip_name);
	fp = fopen(name,"wb");
	if(fp==NULL){
		printf("Error:fopen\n");
		goto free_return;
	}

	i = 0;
	while(i < num){
		addr_list[i] = ftell(fp);
		ret = AddLocalFile(file_list[i], fp);
		if(!ret){
			printf("Error:AddLocalFile\n");
			goto fclose_return;
		}
		i++;
	}
	ret = CreateZipEndData(file_list, addr_list, num, fp);
	if(!ret){
		printf("Error:CreateZipEndData\n");
		goto fclose_return;
	}
	fclose(fp);
	free(file_list);
	free(addr_list);
	SetCurrentDirectory(prev_dir);
	return true;
fclose_return:
	fclose(fp);
free_return:
	free(addr_list);
	free(file_list);
	SetCurrentDirectory(prev_dir);
	return false;
}

bool Extract(FILE *fp){
	struct local_file_header lfh;
	char name[256];
	unsigned char *data;

	if(fread(&lfh.sig,1,14,fp)!=14 || lfh.sig!=0x04034b50){
		return false;
	}
	fread(&lfh.crc32,1,16,fp);
	fread(name,1,lfh.name_len,fp);
	name[lfh.name_len] = '\0';
	{
		int size = strlen(name);
		int i = 0;
		while(i<size && name[i]!='/'){
			i++;
		}
		if(i<size){
			memmove(name,&name[i+1],strlen(&name[i+1])+1);
		}
	}

	data = static_cast<unsigned char*>(malloc(lfh.orig_size));
	fread(data,1,lfh.comp_size,fp);
	DeCompress(data,lfh.comp_size,lfh.orig_size);
	{
		char *s=name;
		while((s=strstr(s,"/"))!=NULL){
			*s = '\0';
			CreateDirectory(name,NULL);
			*s = '/';
			s++;
		}
		FILE *wfp = fopen(name,"wb");
		fwrite(data,1,lfh.orig_size,wfp);
		fclose(wfp);
	}
	free(data);
	return true;
}

bool Zip2Dir(const char *dir_name,const char *fn){
	FILE *fp;
	char prev_dir[256];

	GetCurrentDirectory(256, prev_dir);
	fp = fopen(fn,"rb");
	if(fp==NULL){
		return false;
	}
	SetCurrentDirectory(dir_name);
	while(Extract(fp)){
		//何もしない
	}
	fclose(fp);
	SetCurrentDirectory(prev_dir);
	return true;
}
