@echo off
%~dp0js %~dp0js\node_modules\mqtt\mqtt.js %*
exit /b %ERRORLEVEL%