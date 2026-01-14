# UART Bidirectional Communication Test
## Raspberry Pi 4 ↔ STM32 Nucleo H7A3ZI-Q

Simple test for bidirectional UART communication between Raspberry Pi 4 and STM32 Nucleo H7A3ZI-Q board.

## Hardware Connections

Connect these 3 wires:
```
Raspberry Pi 4       STM32 Nucleo
──────────────       ────────────
GPIO 14 (TX)    ──►  PA3 (RX)
GPIO 15 (RX)    ◄──  PA2 (TX)
GND             ───  GND
```

**Important:** Don't connect power (3.3V/5V) if both boards are powered separately.

## How to Test

### Step 1: Setup STM32

**Using STM32CubeIDE:**
1. Open STM32CubeIDE
2. Import project: File → Open Projects from File System → Select `stm32_uart_test` folder
3. Build: Right-click project → Build Project
4. Flash: Right-click project → Run As → STM32 C/C++ Application

### Step 2: Setup Raspberry Pi

Enable UART hardware:
```bash
sudo raspi-config
```
- Go to: Interface Options → Serial Port
- "Login shell over serial?" → **NO**
- "Serial port hardware enabled?" → **YES**
- Reboot

Install Python library:
```bash
sudo apt install python3-serial
```

### Step 3: Run the Test

On Raspberry Pi:
```bash
python3 rpi_uart_test.py
```

## What You Should See

### On Raspberry Pi Terminal:
```
UART opened on /dev/serial0 at 115200 baud
Bidirectional UART communication with STM32...
Press Ctrl+C to exit
--------------------------------------------------
[Outgoing DATA] RPi->STM32: Message 0
[Incoming DATA] Counter: 0
[Incoming DATA] STM32 ACK: RPi->STM32: Message 0
[Outgoing DATA] RPi->STM32: Message 1
[Incoming DATA] Counter: 1
[Incoming DATA] STM32 ACK: RPi->STM32: Message 1
```

### On STM32 Board:
- **Green LED:** Blinks every second (sending counter)
- **Yellow LED:** Blinks when receiving messages from RPi

## What's Happening

**STM32 → RPi:**
- STM32 sends "Counter: X" every second
- Green LED blinks when sending

**RPi → STM32:**
- RPi sends "RPi->STM32: Message X" every 2 seconds
- STM32 receives it and replies with "STM32 ACK: ..."
- Yellow LED blinks when receiving

## Quick Troubleshooting

**Direct UART communication test:**

**No data from STM32:**
- Check wiring (TX→RX, RX→TX)
- Check UART is enabled: `ls -l /dev/serial*`
- Add to /boot/firmware/config.txt: `enable_uart=1` and reboot
- Check wiring (TX→RX, RX→TX, GND→GND)
- Verify STM32 is flashed and running (Green LED should blink every second)
- Check UART is enabled on RPi: `ls -l /dev/serial*`
- Add to /boot/firmware/config.txt: `enable_uart=1` and reboot

**RPi can't send to STM32:**
- Verify Yellow LED blinks on STM32 when sending from RPi
- Try: `echo "test" > /dev/serial0`

**Permission error on RPi:**
```bash
sudo usermod -a -G dialout $USER
# Then logout and login again
```

**STM32 build/flash issues:**
- Use STM32CubeIDE to build and flash (project was generated with STM32CubeMX)
- Check ST-Link connection is detected in STM32CubeIDE
Listen to STM32:
```bash
stty -F /dev/serial0 115200 raw -echo && cat /dev/serial0
```

Send to STM32:
```bash
echo "Hello STM32" > /dev/serial0
```

## Technical Details

- **STM32 Project:** Generated with STM32CubeMX, build with STM32CubeIDE
- **Baud Rate:** 115200
- **STM32 UART:** USART2 (PA2=TX, PA3=RX)
- **RPi UART:** GPIO 14=TX, GPIO 15=RX (/dev/serial0)
- **Data:** 8 bits, no parity, 1 stop bit
