@echo off
%~dp0py %~dp0py\Lib\visa.py shell
exit /b %ERRORLEVEL%