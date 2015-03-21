#!/bin/bash

export PROGRAMMER_PORT=/dev/tty.usbmodemfd131

export AVR_ORIGIN=/Users/Projects/Arduino/build/macosx/work/Arduino.app/Contents/Java/hardware/tools/avr
export AVRDUDE=${AVR_ORIGIN}/bin/avrdude
export AVRDUDE_CONF=${AVR_ORIGIN}/etc/avrdude.conf

export BOOTLOADER_DIR=~/Sketchbook/hardware/ChainDuino/avr/bootloaders/optiboot
export SN_FILE=/tmp/sn.hex

# Create a serial number file
awk 'BEGIN{"date +'%s'"|getline d0; d1=int(d0/256); d0=d0%256; d2=int(d1/256); d1=d1%256; d3=int(d2/256)%256; d2=d2%256; cksum=(d0+d1+d2+d3)%256; printf ":047FFC00%02x%02x%02x%02x%02x\n:00000001FF\n", d0, d1, d2, d3, (256+129-cksum)%256}' >$SN_FILE

# Erase the chip
# Unlock the chip
# Burn the bootloader
# Burn the Serial number
# Lock the Bootloader
$AVRDUDE -C$AVRDUDE_CONF -p atmega328p -c stk500v1 -P $PROGRAMMER_PORT -b19200 \
  -e \
  -Ulock:w:0x3f:m \
  -Uflash:w:${BOOTLOADER_DIR}/optiboot_chainduino.hex:i \
  -Uflash:w:${SN_FILE}:i \
  -Ulock:w:0x2f:m

rm $SN_FILE