@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
powershell.exe -NoLogo -ExecutionPolicy Bypass %*