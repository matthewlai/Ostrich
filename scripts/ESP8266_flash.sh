#!/bin/bash

esptool.py -p /dev/ttyACM0 --flash_size 2MB-c1 write_flash 0x0 boot_v1.7.bin 0x01000 at/1024+1024/user1.2048.new.5.bin 0x1fc000 esp_init_data_default_v08.bin 0xfe000 blank.bin  0x1fe000 blank.bin
