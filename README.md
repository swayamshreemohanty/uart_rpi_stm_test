# UART Bidirectional Communication Tester
## Raspberry Pi 4 ↔ STM32 Nucleo H7A3ZI-Q

This project tests bidirectional UART communication between a Raspberry Pi 4 and STM32 Nucleo H7A3ZI-Q board.

## Quick Start

### Hardware Setup

Connect the devices:
```
RPi 4 (GPIO)         STM32 Nucleo (USART2)
───────────          ─────────────────────
GPIO 14 (TX) ──────→ PA3 (RX)
GPIO 15 (RX) ←────── PA2 (TX)
GND          ──────── GND
```

**Important:** 
- Do NOT connect 5V or 3.3V power between boards if both are powered separately
- Ensure TX → RX and RX → TX cross-connection
- STM32 uses USART2 on pins PA2 (TX) and PA3 (RX)

### STM32 Setup

**1. Install prerequisites:**
```bash
sudo apt install gcc-arm-none-eabi stlink-tools
```

**2. Build and flash:**
```bash
cd stm32_uart_test
make clean
make -j4
make flash
```

**3. Verify flashing:**
```bash
make probe
```

### Raspberry Pi Setup

**1. Enable UART:**
```bash
sudo raspi-config
# → Interface Options → Serial Port
#   - Login shell over serial: NO
#   - Serial port hardware enabled: YES
sudo reboot
```

**2. Install Python dependencies:**
```bash
sudo apt install python3-serial
```

**3. Run the test:**
```bash
python3 rpi_uart_test.py
```

## Testing the Communication

### Expected Behavior

**On Raspberry Pi (using Python script):**
```
UART opened on /dev/serial0 at 115200 baud
Listening for incoming UART data...
Press Ctrl+C to exit
--------------------------------------------------
Received: Counter: 0
Received: Counter: 1
Received: Counter: 2
```

**On Raspberry Pi (using command line):**
```bash
$ stty -F /dev/serial0 115200 raw -echo && cat /dev/serial0
Counter: 0
Counter: 1
Counter: 2
...
```

**STM32 LEDs:**
- **Green LED:** Toggles every second when sending counter data
- **Yellow LED:** Toggles when data is received from RPi (echo back)

**Communication flow:**
- STM32 sends counter messages every second
- STM32 listens for incoming data and echoes it back
- Yellow LED confirms STM32 is receiving data
- Green LED confirms STM32 is transmitting data

### Command Line Testing (Without Python Script)

**Direct UART communication test:**

**1. Listen for incoming data from STM32:**
```bash
# Configure UART and listen
stty -F /dev/serial0 115200 raw -echo && cat /dev/serial0

# You should see:
# Counter: 0
# Counter: 1
# Counter: 2
# ...
```

**2. Send data to STM32:**
```bash
# Send test data (Yellow LED on STM32 will blink)
echo "test" > /dev/serial0

# Send multiple bytes
echo "Hello STM32" > /dev/serial0
```

**3. Test with hexdump (see raw bytes):**
```bash
# View incoming data in hex format
stty -F /dev/serial0 115200 raw && hexdump -C /dev/serial0
```

**4. Check current UART settings:**
```bash
stty -F /dev/serial0
```

### Troubleshooting

**No data received on RPi:**
```bash
# Check UART availability
ls -l /dev/serial*

# Verify UART is enabled in config
cat /boot/firmware/config.txt | grep uart
# Should show: enable_uart=1

# Check user permissions
groups
# Should include 'dialout'

# Verify Bluetooth not using UART (RPi 3/4)
# Add to /boot/config.txt or /boot/firmware/config.txt:
dtoverlay=disable-bt

# Disable Bluetooth service
sudo systemctl disable hciuart
sudo reboot
```

**STM32 not detected:**
```bash
# Check ST-Link connection
st-info --probe

# Try re-flashing
cd stm32_uart_test
make erase
make flash
```

**Loopback test (RPi only):**
```bash
# Connect TX to RX on RPi
stty -F /dev/serial0 115200
echo "test" > /dev/serial0 &
cat /dev/serial0
```

## Project Structure

```
uart_rpi_stm_test/
├── .gitignore                   # Git ignore file
├── README.md                    # This file
├── rpi_uart_test.py             # Raspberry Pi test script
├── stm32_uart_test.c            # STM32 C version (reference)
├── stm32_uart_test.cpp          # STM32 C++ version (reference)
├── Makefile                     # Makefile (reference)
└── stm32_uart_test/             # Complete STM32 project (ready to build)
    ├── Makefile                 # Build configuration
    ├── STM32H7A3ZITXQ_FLASH.ld  # Linker script
    ├── main.cpp                 # Standalone main (copy)
    ├── Core/
    │   ├── Inc/                 # Header files
    │   └── Src/
    │       ├── main.cpp         # Main application (C++ with UARTComm class)
    │       ├── system_stm32h7xx.c
    │       ├── stm32h7xx_it.c   # Interrupt handlers
    │       ├── stm32h7xx_hal_msp.c
    │       └── syscalls.c       # System calls
    └── Drivers/                 # STM32 HAL and CMSIS
        ├── STM32H7xx_HAL_Driver/
        └── CMSIS/
```

## Build Commands (STM32)

```bash
cd stm32_uart_test

# Build project
make clean       # Clean build artifacts
make -j4         # Compile (parallel build)

# Flash to board
make probe       # Check ST-Link connection
make flash       # Flash binary to STM32
make erase       # Erase flash memory

# Output files in build/
# - stm32_uart_test.bin  (flash this)
# - stm32_uart_test.elf  (debug symbols)
# - stm32_uart_test.hex  (Intel HEX)
```

## Technical Details

### UART Configuration
- **Baud Rate:** 115200
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Flow Control:** None

### STM32 Features
- C++ implementation with `UARTComm` class
- Interrupt-driven receive (HAL_UART_Receive_IT)
- Automatic acknowledgment of received messages
- Periodic heartbeat transmission (every 2 seconds)
- Variadic message formatting support

### RPi Features
- Python 3 with pyserial
- Sends numbered messages every second
- Displays all received data
- Clean error handling and exit

## Files Description

- **rpi_uart_test.py**: Python script for RPi UART communication
- **stm32_uart_test/Core/Src/main.cpp**: C++ application with UARTComm class
- **stm32_uart_test/Makefile**: Build configuration for arm-none-eabi-gcc
- **stm32_uart_test/BUILD_README.md**: Detailed build instructions

## License

This is a test/example project for educational purposes.

## License

This is a test/example project for educational purposes.
