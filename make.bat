@echo off

SET src=%1
SET fld=%2
if "--%2%--" equ "----" (
	SET fld=Debug
)

del %src%.exe %src%.exe _tc.asm %src%.asm 2>nul
echo %fld%\tc %src%.tc
%fld%\tc %src%.tc > %src%.asm
echo fasm %src%.asm
fasm %src%.asm
echo %fld%\hex-dump %src%.exe ^> %src%.hex
%fld%\hex-dump %src%.exe > %src%.hex
