# STM32 UART Test Project

This project contains the complete setup for bidirectional UART communication between Raspberry Pi 4 and STM32 Nucleo H7A3ZI-Q.

## Project Structure

```
stm32_uart_test/
├── main.cpp                    # Main application (C++)
├── Makefile                    # Build configuration
├── STM32H7A3ZITXQ_FLASH.ld    # Linker script
├── Core/
│   ├── Inc/                    # Header files
│   │   ├── main.h
│   │   ├── stm32h7xx_hal_conf.h
│   │   └── stm32h7xx_it.h
│   └── Src/                    # Source files
│       ├── stm32h7xx_hal_msp.c
│       ├── stm32h7xx_it.c
│       └── system_stm32h7xx.c
└── Drivers/                    # STM32 HAL and CMSIS libraries
    ├── STM32H7xx_HAL_Driver/
    └── CMSIS/
```

## Prerequisites

### 1. Install ARM Toolchain

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi
```

Verify installation:

```bash
arm-none-eabi-gcc --version
```

### 2. Install st-flash (STLink Tools)

```bash
sudo apt install stlink-tools
```

Verify:

```bash
st-info --version
```

### 3. Setup udev rules for ST-Link (optional but recommended)

```bash
sudo tee /etc/udev/rules.d/99-stlink.rules << EOF
# ST-Link V2
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE="0666"
# ST-Link V2-1
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE="0666"
# ST-Link V3
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Building the Project

### 1. Clean previous builds

```bash
make clean
```

### 2. Build

```bash
make -j4
```

This will create:

- `build/stm32_uart_test.elf` - ELF executable
- `build/stm32_uart_test.hex` - Intel HEX format
- `build/stm32_uart_test.bin` - Raw binary for flashing

### 3. Check ST-Link connection

```bash
make probe
# or
st-info --probe
```

Expected output:

```
Found 1 stlink programmers
version:    V3J9M3
serial:     066DFF535150897867145023
flash:      2097152 (pagesize: 131072)
sram:       1048576
chipid:     0x0450
dev-type:   STM32H7Ax/7Bx
```

### 4. Flash to board

```bash
make flash
# or manually
st-flash write build/stm32_uart_test.bin 0x08000000
```

### 5. Erase flash (if needed)

```bash
make erase
```

## Hardware Connections

Connect the STM32 to Raspberry Pi:

```
RPi 4 GPIO          STM32 Nucleo (USART3)
────────────        ─────────────────────
GPIO 14 (TX)  ────→ PD9 (RX)
GPIO 15 (RX)  ←──── PD8 (TX)
GND           ────── GND
```

**Important:**

- Do NOT connect 5V or 3.3V power between boards if both are powered separately
- STM32 GPIO pins are 3.3V tolerant
- RPi GPIO pins are also 3.3V

## Troubleshooting

### "arm-none-eabi-gcc: not found"

Install the ARM toolchain:

```bash
sudo apt install gcc-arm-none-eabi
```

### "st-flash: command not found"

Install stlink tools:

```bash
sudo apt install stlink-tools
```

### ST-Link not detected

1. Check USB connection
2. Try different USB port
3. Check permissions:

```bash
sudo st-flash --version
```

4. Add udev rules (see Prerequisites section)

### Build errors

1. Clean and rebuild:

```bash
make clean
make -j4
```

2. Check that all HAL drivers are present in `Drivers/` folder

### Flash write failed

1. Erase the chip first:

```bash
make erase
```

2. Try flashing again:

```bash
make flash
```

3. Reset the board and try again

## Makefile Targets

- `make` or `make all` - Build the project
- `make clean` - Remove build artifacts
- `make flash` - Flash binary to STM32
- `make erase` - Erase STM32 flash memory
- `make probe` - Check ST-Link connection

## Configuration

The UART is configured for:

- **Baud Rate:** 115200
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Flow Control:** None

The application sends:

- Acknowledgments for received data
- Periodic heartbeat messages every 2 seconds

## Next Steps

Once flashed, run the Python script on Raspberry Pi to test communication:

```bash
cd ..
python3 rpi_uart_test.py
```

You should see bidirectional communication between the devices.
