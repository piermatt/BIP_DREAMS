# Group 4 — Wiring Guide: SPS30 + AHT20 + BMP280

## I²C Bus (A4 = SDA, A5 = SCL)

| Sensor | VCC | GND | SDA | SCL | I²C Address |
|--------|-----|-----|-----|-----|-------------|
| AHT20 (T/RH) | 5V | GND | A4 | A5 | 0x38 |
| BMP280 (pressure) | 3.3V | GND | A4 | A5 | 0x76 |
| SPS30 (PM1/2.5/4/10) | 5V | GND | A4 | A5 | 0x69 |

## SPS30 — I²C Mode
- **SEL pin (connector pin 5) MUST be connected to GND** for I²C mode.
- Use the Sensirion ZH connector or cut+strip the 5-pin cable.
- Fan current: ~60 mA. Total system current: ~130 mA. Use USB 500 mA supply.

## Diagram (text)
```
Arduino Uno          SPS30 5-pin connector
5V   ─────────────── Pin 1 (VDD)
GND  ─────────────── Pin 2 (GND)
A4   ─────────────── Pin 3 (SDA)
A5   ─────────────── Pin 4 (SCL)
GND  ─────────────── Pin 5 (SEL) ← MUST be GND for I²C

Arduino Uno          AHT20
5V   ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL

Arduino Uno          BMP280
3.3V ─────────────── VCC
GND  ─────────────── GND
A4   ─────────────── SDA
A5   ─────────────── SCL
```
