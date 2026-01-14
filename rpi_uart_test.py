#!/usr/bin/env python3
"""
Raspberry Pi 4 UART Communication Test
For bidirectional communication with STM32 Nucleo H7A3ZI-Q
"""

import sys
import time

import serial

# UART Configuration
SERIAL_PORT = '/dev/serial0'  # RPi 4 default UART (GPIO 14/15)
BAUD_RATE = 115200
TIMEOUT = 1

def main():
    try:
        # Initialize UART
        ser = serial.Serial(
            port=SERIAL_PORT,
            baudrate=BAUD_RATE,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=TIMEOUT
        )
        
        print(f"UART opened on {SERIAL_PORT} at {BAUD_RATE} baud")
        print("Listening for incoming UART data from STM32...")
        print("Press Ctrl+C to exit")
        print("-" * 50)
        
     
        
        while True:
            # Listen for data from STM32
            if ser.in_waiting > 0:
                received = ser.readline().decode('utf-8', errors='ignore').strip()
                if received:
                    print(f"[Incoming DATA] {received}")
            else:
                # Small delay to prevent CPU spinning when no data
                time.sleep(0.01)
            
    except serial.SerialException as e:
        print(f"Serial port error: {e}")
        print("\nTroubleshooting tips:")
        print("1. Enable UART: sudo raspi-config -> Interface Options -> Serial Port")
        print("   - Disable serial console, enable serial port hardware")
        print("2. Check connections: TX->RX, RX->TX, GND->GND")
        print("3. Verify port: ls -l /dev/serial*")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n\nExiting...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    main()
