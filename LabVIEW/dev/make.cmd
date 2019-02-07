@echo off

lib /machine:x64 /def:libjnlua-5.3-windows-amd64.def
cl /nologo /MD /O2 /Iinclude /DLUA_BUILD_AS_DLL=1 /DLUA_LIB /c lua53.c
link /dll /NOENTRY /machine:x64 lua53.obj libjnlua-5.3-windows-amd64.lib /def:lua53.def /out:lua53.dll

rem lib /machine:x64 /def:lua51.def
rem link /dll /NOENTRY /NODEFAULTLIB /machine:x64 lua51.lib /def:lua5.def /out:lua5.dll

rem lib /machine:x64 /def:lua53.def
rem link /dll /NOENTRY /NODEFAULTLIB /machine:x64 lua53.lib /def:lua5.def /out:lua5.dll
rem move lua5.dll ..

del *.exp *.lib *.obj

..\..\tcc\tcc -Iinclude lua.c lua53.def
move lua.exe ..

rem ..\..tcc\tcc -shared -DLUA_BUILD_AS_DLL=1 -DLUA_LIB -IC:\JDK7\include -IC:\JDK7\include\win32 -I..\lua53\include javavm.c libjnlua-5.3-windows-amd64.def jvm.def

rem \JDK8\bin\java -Dretrolambda.inputDir=JNLua -Dretrolambda.classpath=JNLua -Dretrolambda.defaultMethods=false -jar retrolambda.jar