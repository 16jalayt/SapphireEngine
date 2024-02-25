@echo off
SET "CDIR=%~dp0"
:: for loop requires removing trailing backslash from %~dp0 output
SET "CDIR=%CDIR:~0,-1%"
FOR %%i IN ("%CDIR%") DO SET "PARENTFOLDERNAME=%%~nxi"
@echo on
nxlink.exe %PARENTFOLDERNAME%.nro -a 192.168.1.57 -s
PAUSE