@echo off
rem this is a stupid little batch file to help test the code on windows machines

debug\tc test.tc
debug\vm
debug\hex-dump tc.out >> vm.lst
