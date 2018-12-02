@echo off

setlocal
set NODE_PATH=%~dp0js\node_modules;%~dp0cairo\node_modules;%~dp0gnuplot\node_modules;%~dp0Jupyter\node_modules;%~dp0Qt\node_modules;%~dp0LabVIEW\node_modules
:: optional arguments: --log_all --trace_gc --trace_gc_verbose
%~dp0js\node --harmony %*
exit /b %ERRORLEVEL%