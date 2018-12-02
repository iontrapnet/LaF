@echo off
%~dp0lua -lilua %*
exit /b %ERRORLEVEL%