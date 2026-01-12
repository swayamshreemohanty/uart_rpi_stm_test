# UART Bidirectional Communication Tester
## Raspberry Pi 4 ↔ STM32 Nucleo H7A3ZI-Q

This project tests bidirectional UART communication between a Raspberry Pi 4 and STM32 Nucleo H7A3ZI-Q board.

## Quick Start

### Hardware Setup

Connect the devices:
```
RPi 4 (GPIO)         STM32 Nucleo (USART3)
───────────          ─────────────────────
GPIO 14 (TX) ──────→ PD9 (RX)
GPIO 15 (RX) ←────── PD8 (TX)
GND          ──────── GND
```

**Important:** Do NOT connect 5V or 3.3V power between boards if both are powered separately.

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

**On Raspberry Pi:**
```
UART opened on /dev/serial0 at 115200 baud
Press Ctrl+C to exit
--------------------------------------------------
Sent: RPi->STM32: 0
Received: STM32->RPi: ACK [RPi->STM32: 0]
Received: STM32 Heartbeat: 0
Sent: RPi->STM32: 1
Received: STM32->RPi: ACK [RPi->STM32: 1]
Received: STM32 Heartbeat: 1
```

**Communication flow:**
- RPi sends numbered messages every second
- STM32 echoes received messages with acknowledgment
- STM32 sends heartbeat every 2 seconds
- Both directions tested simultaneously

### Troubleshooting

**No data received on RPi:**
```bash
# Check UART availability
ls -l /dev/serial*

# Verify Bluetooth not using UART (RPi 3/4)
# Add to /boot/config.txt:
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
