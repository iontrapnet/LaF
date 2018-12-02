@echo off

taskkill /f /im mosquitto.exe
cd %~dp0
hidec mosquitto.exe -c mosquitto.conf