# Group 1 — Wiring Guide: ENS160 + AHT21 + AHT20 + BMP280

## I²C Bus (A4 = SDA, A5 = SCL)

All sensors share the same 2-wire I²C bus.

| Sensor | VCC | GND | SDA | SCL | I²C Address |
|--------|-----|-----|-----|-----|-------------|
| AHT21 (T/RH main) | 3.3V | GND | A4 | A5 | 0x38 |
| BMP280 (pressure) | 3.3V | GND | A4 | A5 | 0x76 |
| ENS160 (eCO₂/TVOC) | 3.3V | GND | A4 | A5 | 0x53 |
| Logging Shield RTC | — | — | A4 | A5 | 0x68 (internal) |

> ⚠️ AHT20 and AHT21 both use address 0x38 — use **only one** on this bus.
> This group uses AHT21 as the primary T/RH sensor.

## Logging Shield
- Stacks directly on Arduino Uno headers
- Insert a FAT32-formatted microSD card (≤32 GB)
- Install CR1220 coin cell in the RTC holder

## Breadboard Connections
All sensors connect to the same 4 rails on the breadboard:
- `+3.3V rail` from Arduino 3.3V pin
- `GND rail` from Arduino GND
- `SDA rail` → Arduino A4
- `SCL rail` → Arduino A5

## Diagram (text)
```
Arduino Uno          ENS160 Breakout
3.3V ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL

Arduino Uno          AHT21 Breakout
3.3V ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL

Arduino Uno          BMP280 Breakout
3.3V ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL
D10  ─────(floating) CS (tie to 3.3V or leave NC — I²C mode)
```
