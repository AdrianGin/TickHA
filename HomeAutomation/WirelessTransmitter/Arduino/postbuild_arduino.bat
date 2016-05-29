set filename=%1
set comport=%2

avr-objcopy -j .text -j .data %filename%.elf -Obinary %filename%.bin
avr-objcopy -j .text -j .data %filename%.elf -Oihex %filename%.hex
avr-size %filename%.elf
avr-objdump -h -S %filename%.elf > %filename%.lst
avrdude -c arduino -P%comport% -b57600 -pm328p -e -U flash:w:%filename%.hex:i