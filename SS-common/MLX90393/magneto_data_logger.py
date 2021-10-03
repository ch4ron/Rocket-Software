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
        file.write("X1   Y1   Z1   X2   Y2   Z2   X3   Y3   Z3   X4   Y4   Z4   X5   Y5   Z5")
        file.write("X6   Y6   Z6   X7   Y7   Z7   X8   Y8   Z8   X9   Y9   Z9   X10  Y10  Z10  X11  Y11  Z11\n")

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
