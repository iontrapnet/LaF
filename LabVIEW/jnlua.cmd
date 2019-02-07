@echo off
set PATH=C:\JRE8\bin;C:\JRE8\bin\server;%PATH%
%~dp0lua jnlua.lua
exit /b %ERRORLEVEL%