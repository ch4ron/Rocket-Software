#!/usr/bin/python2
from jumper.vlab import Vlab
# set up the device simulation
v = Vlab(working_directory=".", print_uart=True, uarts_to_print=['UART5'], platform="stm32f446")
v.load("build/Kromek_v2.bin")

v.start()


# print 'dupa'
# v.uart.write(b'log\r\n')
# while True:
#    v.USART2.write(b'\x05&\t\x12\xe8\x05\x00\x00\x00\x009d\xae\n')
#    time.sleep(1)
#
# v.uart.write(b'python\r\n')
# v.uart.wait_until_uart_receives(b'Nice joke ;)', 500)
#
# v.stop()
#
# print('Got here, so that means the test result should be OK')
