@echo off

rem set TCC=d:\Develop\tcc\tcc 
set TCC=d:\Develop\tcc\64\tcc 

%TCC% -shared -I d:\Lua\include -I. -DLUA_BUILD_AS_DLL -DLUA_LIB lvisa.c ..\lua51.def
%TCC% -shared -I d:\Lua\include -I. -DLUA_BUILD_AS_DLL -DLUA_LIB -D__WIN32__ _ul_serial.c ..\lua51.def