@echo off
%~dp0..\py -m PyQt4.uic.pyuic %*
exit /b %ERRORLEVEL%