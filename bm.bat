@echo off
rem this is a stupid little batch file to help test the code on windows machines

set f=%1%

if [%f%] equ [] (
	set %f=debug
)

echo %f%\tc bm.tc
%f%\tc bm.tc

echo %f%\vm
%f%\vm

%f%\hex-dump tc.out >> vm.lst
