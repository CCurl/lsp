rem a stupid little batch file for windows
@echo off
del _tc.exe _test.exe _tc.asm _test.asm 2>nul
echo Debug\tc test.tc
Debug\tc test.tc
echo ren _tc.asm _test.asm
ren _tc.asm _test.asm
echo fasm _test.asm
fasm _test.asm
echo Debug\hex-dump _test.exe ^> _test.hex
Debug\hex-dump _test.exe > _test.hex
