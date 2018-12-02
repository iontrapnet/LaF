@echo off

setlocal
set argv=%*
IF "%1"=="" set argv=console
%~dp0..\py -m jupyter %argv% --kernel=lua64
exit /b %ERRORLEVEL%