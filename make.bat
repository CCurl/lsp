@echo off

SET fld=%2
if "--%2%--" equ "----" (
	SET fld=Debug
)

if "--%1%--" equ "--test--" (
	del _tc.exe _test.exe _tc.asm _test.asm 2>nul
	echo %fld%\tc test.tc
	%fld%\tc test.tc > _test.asm
	echo fasm _test.asm
	fasm _test.asm
	echo %fld%\hex-dump _test.exe ^> _test.hex
	%fld%\hex-dump _test.exe > _test.hex
)
if "--%1%--" equ "--bm--" (
	del _tc.exe _bm.exe _tc.asm _bm.asm 2>nul
	echo %fld%\tc bm.tc
	%fld%\tc bm.tc > _bm.asm
	echo fasm _bm.asm
	fasm _bm.asm
	echo %fld%\hex-dump _bm.exe ^> _bm.hex
	%fld%\hex-dump _bm.exe > _bm.hex
)
