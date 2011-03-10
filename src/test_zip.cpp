#include <stdio.h>
#include "zip.h"

int main(int argc,char **argv){
	SetAppDir();
	printf("%d\n",Dir2Zip(argv[1],"test_zip.zip"));
	return 0;
}
