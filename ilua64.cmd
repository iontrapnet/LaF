@echo off
%~dp0lua64 -lilua %*
exit /b %ERRORLEVEL%