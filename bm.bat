rem a stupid little batch file for windows
@echo off
del _tc.exe _bm.exe _tc.asm _bm.asm 2>nul
echo Debug\tc bm.tc
Debug\tc bm.tc
echo ren _tc.asm _bm.asm
ren _tc.asm _bm.asm
echo fasm _bm.asm
fasm _bm.asm
echo Debug\hex-dump _bm.exe ^> _bm.hex
Debug\hex-dump _bm.exe > _bm.hex
