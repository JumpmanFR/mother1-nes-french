copy ebb.nes test.nes
@xkas -o test.nes ebb.asm
@insert.exe 1
@echo.
@echo off
@for %%x in (%cmdcmdline%) do if /i "%%~x"=="/c" set DOUBLECLICKED=1
@if defined DOUBLECLICKED pause