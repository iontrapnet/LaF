@echo off

%~dp0..\..\..\lua64 %~dp0..\lua-complete.cmd %*
exit /b %ERRORLEVEL%