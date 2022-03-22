copy ebb.nes test.nes
@python converttext.py script.txt eng_table.txt script_converted.txt
@xkas test.nes ebb.asm
@insert.exe 1
@echo.
@del script_converted.txt
@echo off
@for %%x in (%cmdcmdline%) do if /i "%%~x"=="/c" set DOUBLECLICKED=1
@if defined DOUBLECLICKED pause