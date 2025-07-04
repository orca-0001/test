
@echo off
setlocal enabledelayedexpansion
set path=%~dp0..\twen32\tools\gcc-arm\bin;%~dp0..\asrpro\asr_pro_sdk\tools\build-tools\bin;%~dp0..\twen-asr\RISCV-GCC\bin;%path%;

make
Twen003ISP





