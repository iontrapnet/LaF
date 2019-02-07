@echo off
%~dp0luajit -lilua %*
exit /b %ERRORLEVEL%