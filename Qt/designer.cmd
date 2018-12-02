@echo off

setlocal
set PATH=%~dp0\Lib\PyQt4;%PATH%
cd %~dp0Designer4
start designer
exit /b %ERRORLEVEL%