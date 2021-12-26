import serial
from serial.serialutil import SerialException

# Serial port configuration parameters
COM_PORT = 'COM4'
BAUDRATE = 115200
TIMEOUT = 10

# Try to create an serial port instance
try:
    magneto = serial.Serial(COM_PORT, baudrate=BAUDRATE, timeout=TIMEOUT)
    print("Serial port opened successfully\n")
except SerialException:
    print("Serial port cannot be opened, wrong COM port?")
    exit()

def magneto_read_lines():
    """Read one full line (terminated with \n) from Kromek"""

    data = magneto.readline()
    return data

def parse_magneto_data(data):
    """
    Parse raw data received from Kromek
    Example: 
    input - b'\x31\x32\x33' (bytes)
    output - 1 2 3 (string)
    """

    parsed_data = str(data).split("b'")
    parsed_data = parsed_data[1].split("\\n'")
    return parsed_data[0]

def main():
    with open("magneto_data.txt", "w") as file:
        # Write header into the file
        # Variant 1 - retrieving measurement one magnetometer at a time
        # file.write("time1 X1")

        # Variant 2 - retrieving measurements for all magnetometers at one iteration
        file.write("time1 X1 Y1 Z1 time2 X2 Y2 Z2 time3 X3 Y3 Z3 time4 X4 Y4 Z4 time5 X5 Y5 Z5 time6 X6 Y6 Z6")
        file.write("time7 X7 Y7 Z7 time8 X8 Y8 Z8 time9 X9 Y9 Z9 time10 X10 Y10 Z10 time11 X11 Y11 Z11\n")

        # Get the data and save it to the file
        while(1):
            try:
                data = magneto_read_lines()
                file.write(parse_magneto_data(data))
                file.write("\n")
            except KeyboardInterrupt:
                break

        magneto.close()
        print("Serial port successfully closed\n")

if __name__ == '__main__':
    main()
