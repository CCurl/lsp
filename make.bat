@echo off

if "--%1%--" equ "--test--" (
	del _tc.exe _test.exe _tc.asm _test.asm 2>nul
	echo Debug\tc test.tc
	Debug\tc test.tc
	echo ren _tc.asm _test.asm
	ren _tc.asm _test.asm
	echo fasm _test.asm
	fasm _test.asm
	echo Debug\hex-dump _test.exe ^> _test.hex
	Debug\hex-dump _test.exe > _test.hex
)
if "--%1%--" equ "--bm--" (
	del _tc.exe _bm.exe _tc.asm _bm.asm 2>nul
	echo Debug\tc bm.tc
	Debug\tc bm.tc
	echo ren _tc.asm _bm.asm
	ren _tc.asm _bm.asm
	echo fasm _bm.asm
	fasm _bm.asm
	echo Debug\hex-dump _bm.exe ^> _bm.hex
	Debug\hex-dump _bm.exe > _bm.hex
)
