@echo off
rem this is a stupid little batch file to help test the code on windows machines

set f=%1%

if [%f%] equ [] (
	set %f=debug
)

echo f=%f%

%f%\tc test.tc
%f%\vm
%f%\hex-dump tc.out >> vm.lst
