@echo off
%~dp0js %~dp0js\node_modules\nesh\bin\nesh.js -p "> " %*
exit /b %ERRORLEVEL%