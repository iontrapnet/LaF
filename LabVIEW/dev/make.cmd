@echo off

lib /machine:x64 /def:libjnlua-5.3-windows-amd64.def
cl /nologo /MD /O2 /Iinclude\jnlua /DLUA_BUILD_AS_DLL=1 /DLUA_LIB /c lua5-compat.c
link /dll /NOENTRY /machine:x64 lua5-compat.obj libjnlua-5.3-windows-amd64.lib /def:lua53.def /out:lua53.dll

rem lib /machine:x64 /def:libluajit.def
rem link /dll /NOENTRY /NODEFAULTLIB /machine:x64 libluajit.lib /def:lua51.def /out:lua51.dll

del *.exp *.lib *.obj

rem ..\..\tcc\tcc -Iinclude\jnlua lua.c libjnlua-5.3-windows-amd64.def -o jnlua.exe
rem move jnlua.exe ..

rem ..\..\tcc\tcc -Iinclude\luajit lua.c libluajit.def -o luajit.exe
rem move luajit.exe ..

rem ..\..tcc\tcc -shared -DLUA_BUILD_AS_DLL=1 -DLUA_LIB -IC:\JDK7\include -IC:\JDK7\include\win32 -I..\lua53\include javavm.c libjnlua-5.3-windows-amd64.def jvm.def

rem \JDK8\bin\java -Dretrolambda.inputDir=JNLua -Dretrolambda.classpath=JNLua -Dretrolambda.defaultMethods=false -jar retrolambda.jar