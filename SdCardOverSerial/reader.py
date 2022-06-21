#!/usr/bin/env python3

import serial

with serial.Serial('/dev/cu.usbmodem1301', 115200) as ser:
  while True:
    filename = ser.readline()[:-1]
    filesize = sum((ser.read(1)[0] << (i * 8)) for i in range(4))
    print('Downloading', filename, '[' + str(filesize), 'bytes]... ',
      end='', flush=True)
    with open(filename, 'wb') as file:
      file.write(ser.read(filesize))
    print('Done!')
