# Wiring Guide: SCD41 (+ SPS30 for Groups 5/6) + AHT20 + BMP280

## I²C Bus (A4 = SDA, A5 = SCL)

| Sensor | VCC | GND | SDA | SCL | I²C Address |
|--------|-----|-----|-----|-----|-------------|
| AHT20 (T/RH) | 5V | GND | A4 | A5 | 0x38 |
| BMP280 (pressure) | 3.3V | GND | A4 | A5 | 0x76 |
| SCD41 (CO₂/T/RH) | 5V | GND | A4 | A5 | 0x62 |
| SPS30 (PM, Groups 5&6) | 5V | GND | A4 | A5 | 0x69 |

## SPS30 — I²C Mode Selection
- Connect SPS30 **SEL pin (pin 5) → GND** to activate I²C mode.
- Default (SEL floating) = UART mode → will NOT work with this sketch.

## SCD41 — Notes
- Requires 5V supply (do not use 3.3V).
- Internal fan-less PA-NDIR: silent operation.
- First measurement available 5 s after startPeriodicMeasurement().

## SPS30 — Notes
- Fan starts on sps30_start_measurement(). Audible hum is normal.
- 8 second spin-up before first valid measurement.
- Typical current draw: 55–60 mA.

## Diagram (text)
```
Arduino Uno          SCD41 Breakout
5V   ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL

Arduino Uno          SPS30 Connector (Groups 5 & 6)
5V   ─────────────── Pin 1 (VDD)
GND  ─────────────── Pin 2 (GND)
A4   ─────────────── Pin 3 (SDA / MOSI)
A5   ─────────────── Pin 4 (SCL / SCLK)
GND  ─────────────── Pin 5 (SEL)  ← critical for I²C mode!
```
