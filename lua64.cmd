@echo off
setlocal
set LUA_PATH=%~dp0?.lua;%~dp0?\init.lua;%~dp0num\lua\?.lua;%~dp0num\lua\?\init.lua;%~dp0lua\lua\?.lua;%~dp0lua\lua\?\init.lua;%~dp0Jupyter\lua\?.lua;%~dp0Jupyter\lua\?\init.lua;%~dp0LabVIEW\lua\?.lua;%~dp0LabVIEW\lua\?\init.lua;%~dp0IDE\lua\?.lua;%~dp0IDE\lua\?\init.lua;;
set LUA_CPATH=%~dp0num\lua\64\?.dll;%~dp0IDE\lua\64\?.dll;;
set PATH=%~dp0num\lua\64;%~dp0IDE\lua\64;%PATH%
%~dp0lua\64\luajit %*
exit /b %ERRORLEVEL%