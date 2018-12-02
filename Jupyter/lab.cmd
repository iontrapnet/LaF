@echo off

setlocal
set argv=%*
if "%1"=="" set argv=lab
%~dp0..\py -m jupyter %argv%
exit /b %ERRORLEVEL%