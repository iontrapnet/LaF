@echo off
%~dp0py -m IPython %*
exit /b %ERRORLEVEL%