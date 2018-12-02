@echo off
setlocal
set LUA_PATH=%~dp0?.lua;%~dp0?\init.lua;%~dp0cairo\lua\?.lua;%~dp0cairo\lua\?\init.lua;%~dp0gnuplot\lua\?.lua;%~dp0gnuplot\lua\?\init.lua;%~dp0Jupyter\lua\?.lua;%~dp0Jupyter\lua\?\init.lua;%~dp0Qt\lua\?.lua;%~dp0Qt\lua\?\init.lua;%~dp0LabVIEW\lua\?.lua;%~dp0LabVIEW\lua\?\init.lua;%~dp0IDE\lua\?.lua;%~dp0IDE\lua\?\init.lua;;
set LUA_CPATH=%~dp0cairo\lua\?.dll;%~dp0Qt\lua\?.dll;%~dp0IDE\lua\?.dll;;
set PATH=%~dp0cairo;%~dp0Qt\lua;%~dp0IDE\lua;%PATH%
%~dp0lua\luajit %*
exit /b %ERRORLEVEL%