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

# TODO: Add support for multiple (11) magnetometers
def main():
    with open("magneto_data.txt", "w") as file:
        # Write header into the file
        file.write("X1\t  Y1\tZ1\n")

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
