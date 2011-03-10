#pragma once
#include <windows.h>

void SetAppDir(void);
bool Dir2Zip(const char *dir_name, const char *zip_name=NULL);
bool Zip2Dir(const char *dir_name,const char *fn);
