@echo off

setlocal
set argv=%*
if "%1"=="" set argv=notebook
%~dp0..\py -m jupyter %argv%
exit /b %ERRORLEVEL%