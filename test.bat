@echo off
rem this is a stupid little batch file to help test the code on windows machines

set f=%1%

if [%f%] equ [] (
	set %f=Debug
)

echo %f%\tc test.tc
%f%\tc test.tc

echo %f%\vm
%f%\vm

echo %f%\hex-dump tc.out
%f%\hex-dump tc.out >> vm.lst
