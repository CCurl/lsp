@echo on
rem this is a stupid little batch file to help test the code on windows machines
set f=%1%
if [%f%] equ [] (
	set %f=Debug
)
del _tc.exe test.exe 2>nul
%f%\tc test.tc
fasm _tc.asm
ren _tc.exe test.exe
%f%\hex-dump tc.out > vm.lst
