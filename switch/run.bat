@echo off
SET "CDIR=%~dp0"
:: for loop requires removing trailing backslash from %~dp0 output
SET "CDIR=%CDIR:~0,-1%"
FOR %%i IN ("%CDIR%") DO SET "PARENTFOLDERNAME=%%~nxi"
@echo on
F:\Emu\ryujinx-1.1.58-win_x64\publish\Ryujinx.exe %PARENTFOLDERNAME%.nro