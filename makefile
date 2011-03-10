cc  = cl
opt = /errorReport:none /W1 /MT /Ox /EHsc
obj = lib.obj main.obj network.obj zip.obj resource.res
exe = AIManager

.c.obj:
        $(cc) $(opt) -c /Tp $<
.cpp.obj:
        $(cc) $(opt) -c /Tp $<
$(exe).exe: $(obj)
	version.exe
        $(cc) $(opt) /Fe$* $** $(lib)

resource.res: src/resource.rc
	rc -i"E:\WTL81\Include" -i"E:\Program Files\Microsoft Visual Studio 8\VC\include" -fo"resource.res" src/resource.rc

lib.obj: src/lib.cpp src/lib.h
main.obj: src/main.cpp src/consts.h src/window.h src/network.h src/zip.h src/lib.h
network.obj: src/network.cpp src/network.h
zip.obj: src/zip.cpp

