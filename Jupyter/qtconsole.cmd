@echo off

setlocal
set argv=%*
IF "%1"=="" set argv=py
%~dp0..\py -m jupyter qtconsole --kernel=%argv%
exit /b %ERRORLEVEL%