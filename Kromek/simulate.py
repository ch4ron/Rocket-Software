#!/usr/bin/python2
from jumper.vlab import Vlab
import os
import sys
import pty
import serial
import time

class SerialEmulator():
    def __init__(self):
        self.master, self.slave = pty.openpty()
        self.fd = os.fdopen(self.master, "rwb")
        print os.ttyname(self.slave)

    def write(self, data):
            self.fd.write(data)
            self.fd.flush()

    def read(self):
        return self.fd.read(1)

#  set up the device simulation
v = Vlab(working_directory=".", print_uart=True, uarts_to_print=['UART5'], platform="stm32f446", gdb_mode=False)
v.load("cmake-build-simulate/Kromek.bin")
v.start()

ser = SerialEmulator()

if __name__ == '__main__':
    while True:
        read = ser.read()
        v.USART2.write(read)
