cmd /k "%~dp0/AirISP/AirISP.exe -c air001 -p COM37  -b 115200  write_flash  -e  0x08000000 "%~dp0/AirISP/bootloader.bin"
