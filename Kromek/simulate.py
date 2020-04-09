#!/usr/bin/python2
from jumper.vlab import Vlab
import os
import pty


class SerialEmulator:
    def __init__(self):
        self.master, self.slave = pty.openpty()
        print os.ttyname(self.slave)

    def write(self, data):
        os.write(self.master, chr(data))

    def read(self):
        return os.read(self.master, 1)


abs_kromek_dir = os.path.dirname(os.path.abspath(__file__))
#  set up the device simulation
v = Vlab(working_directory=".", print_uart=True, uarts_to_print=['UART5'], platform="stm32f446", gdb_mode=False)
v.load(abs_kromek_dir + "/cmake-build-simulate/Kromek.bin")
v.start()


def receive_uart(data):
    ser.write(data)


ser = SerialEmulator()
v.USART2.on_uart_data(receive_uart)


if __name__ == '__main__':
    while True:
        read = ser.read()
        v.USART2.write(read)
