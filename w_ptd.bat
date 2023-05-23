python.exe d:\MicroPython\esptool\esptool.py --chip esp32 --baud 921600 --flash_mode dio --flash_freq 80m write_flash 0x10000 D:\LoRa\MESHTASTIC\firmware\.pio\build\ptd/firmware.bin

pause
