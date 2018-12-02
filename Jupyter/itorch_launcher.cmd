@echo off

rem taskkill /f /im luajit.exe

FOR /F "tokens=1 delims=" %%A in ('%~dp0..\lua mktemp.lua') do SET IO_STDO=%%A
FOR /F "tokens=1 delims=" %%A in ('%~dp0..\lua mktemp.lua') do SET IO_PORTNUM=%%A

rem Process 1 writes the port number to communicate to this file
rem Process 2 reades from this file and binds to that port
start /B %~dp0..\lua -e "arg={[[%1]],[[%IO_STDO%]],[[%IO_PORTNUM%]]};require 'itorch.IOHandler'"
%~dp0..\lua -e "arg={[[%1]],[[%IO_PORTNUM%]]};require 'itorch.main'" >%IO_STDO%
