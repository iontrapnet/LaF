@echo off

setlocal
set PYTHONPATH=%~dp0py\Lib.zip;%~dp0py\Lib\IPython.zip;%~dp0cairo\Lib;%~dp0gnuplot\Lib;%~dp0Jupyter\Lib;%~dp0Jupyter\Lib\Jupyter.zip;%~dp0Qt\Lib;%~dp0LabVIEW\Lib;%~dp0IDE\Lib;%~dp0IDE\Lib\Spyder.zip;%~dp0num\Lib
set PYTHONSTARTUP=%~dp0py\Lib\startup.py
set PATH=%~dp0lua;%~dp0py\Scripts;%PATH%
%~dp0py\python %*
exit /b %ERRORLEVEL%