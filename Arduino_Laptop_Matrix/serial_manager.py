import serial
import serial.tools.list_ports
import time
import config

class SerialManager:
    """
    Manages serial communication with the Arduino.
    Handles auto-detection and reconnects.
    """
    def __init__(self):
        self.ser = None
        self.last_sent_value = None
        self.port_name = config.SERIAL_PORT
        
        self.connect()

    def find_arduino_port(self):
        """
        Attempts to find the Arduino COM port automatically.
        """
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # Look for common Arduino descriptors
            if "Arduino" in port.description or "CH340" in port.description or "ttyACM" in port.device:
                return port.device
        
        # If not found by description, return the first available one as fallback
        # or return None to indicate failure.
        return None

    def connect(self):
        """
        Attempts to establish a connection to the serial port.
        """
        if self.port_name is None:
            print("Auto-detecting Arduino...")
            self.port_name = self.find_arduino_port()

        if self.port_name:
            try:
                self.ser = serial.Serial(self.port_name, config.SERIAL_BAUD_RATE, timeout=1)
                time.sleep(2) # Give Arduino time to reset upon connection
                print(f"Successfully connected to Arduino on {self.port_name}")
                return True
            except Exception as e:
                print(f"Failed to connect to {self.port_name}: {e}")
                self.ser = None
                # Reset port_name so it can try auto-detecting again if it was auto-detected
                if config.SERIAL_PORT is None:
                    self.port_name = None
                return False
        else:
            print("No Arduino found.")
            return False

    def is_connected(self):
        return self.ser is not None and self.ser.is_open

    def send_value(self, value):
        """
        Sends the value to the Arduino if it has changed.
        Value should be an integer 0-5, or None for no hand.
        """
        if not self.is_connected():
            # Try to reconnect
            if not self.connect():
                return False

        # If value is None, we send a special character, e.g., 'X'
        if value is None:
            char_to_send = 'X'
        elif value == 10:
            char_to_send = 'A'
        else:
            char_to_send = str(value)

        # Only send if the value changed
        if char_to_send != self.last_sent_value:
            try:
                self.ser.write(char_to_send.encode())
                self.last_sent_value = char_to_send
                return True
            except serial.SerialException:
                print("Serial connection lost.")
                self.ser.close()
                self.ser = None
                self.last_sent_value = None
                return False
        return True

    def close(self):
        """
        Closes the serial connection.
        """
        if self.ser and self.ser.is_open:
            self.ser.close()
