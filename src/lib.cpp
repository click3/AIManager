
#include <stdio.h>
#include <windows.h>
#include <atlbase.h>
#include "lib.h"

const char *id2char(int id){
	static char db[][11] = {
		{"—ì–²"},{"–‚—¹"},{"ç–é"},{"ƒAƒŠƒX"},
		{"ƒpƒ`ƒ…ƒŠ["},{"—d–²"},{"ƒŒƒ~ƒŠƒA"},{"—HXŽq"},
		{"Ž‡"},{"äÂ"},{"—D“Ü‰Ø"},{"•¶"},
		{"¬’¬"},{"ˆß‹è"},{"“VŽq"},{"‘•c"},
		{"ƒ`ƒ‹ƒm"},{"”ü—é"},{"‹ó"},{"z–KŽq"},
		{"•¡”"}};
	if(id>21){
		printf("Error:id2char ”ÍˆÍŠO‚ÌID\n");
		ATLASSERT(false);
		return db[0];
	}
	return db[id];
}
char *id2char2(int id){
	static char db[][11] = {
		{"reimu"},{"marisa"},{"sakuya"},{"alice"},
		{"patchouli"},{"youmu"},{"remilia"},{"yuyuko"},
		{"yukari"},{"suika"},{"udonge"},{"aya"},
		{"komachi"},{"iku"},{"tenshi"},{"sanae"},
		{"chirno"},{"meirin"},{"utsuho"},{"suwako"},
		{"other"}};
	if(id>21){
		printf("Error:id2char ”ÍˆÍŠO‚ÌID\n");
		ATLASSERT(false);
		return db[0];
	}
	return db[id];
}

bool DeleteDirectory(const char *fn){
	bool ret = false;
	char prev_dir[256];
	GetCurrentDirectory(256,prev_dir);
	if(SetCurrentDirectory(fn)==0){
		return false;
	}

	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile("*", &fd);
	if(hSearch == INVALID_HANDLE_VALUE){
		return false;
	}
	while(true){
		if(strcmp(fd.cFileName,"..") != 0 && strcmp(fd.cFileName,".") != 0){
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				DeleteDirectory(fd.cFileName);
			} else {
				printf(fd.cFileName);
				DeleteFile(fd.cFileName);
			}
		}
		if(!FindNextFile(hSearch, &fd)){
			if( GetLastError() == ERROR_NO_MORE_FILES ){
				break;
			}
		}
	}
	FindClose(hSearch);

	SetCurrentDirectory(prev_dir);
	return RemoveDirectory(fn);
}
