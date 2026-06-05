# Group 2 — Wiring Guide: JW01-CO2 + AHT20 + BMP280

## I²C Sensors

| Sensor | VCC | GND | SDA | SCL | I²C Address |
|--------|-----|-----|-----|-----|-------------|
| AHT20 (T/RH) | 5V | GND | A4 | A5 | 0x38 |
| BMP280 (pressure) | 3.3V | GND | A4 | A5 | 0x76 |

## UART Sensor — JW01-CO2 (SoftwareSerial)

| JW01 Pin | Wire | Arduino Pin |
|----------|------|-------------|
| VCC (+5V) | Red | 5V |
| GND | Black | GND |
| TX (out) | Yellow | D2 (SW-RX) |
| RX (in) | Green | D3 (SW-TX) |

> ⚠️ JW01 TX → Arduino D2, JW01 RX → Arduino D3.
> Do NOT connect to D0/D1 (hardware UART = USB Serial — will conflict with USB upload).

## JW01 Warm-Up
The NDIR lamp needs ~3 minutes after power-on to stabilize.
First few readings may show 0 or incorrect values. This is normal.

## Diagram (text)
```
Arduino Uno          JW01-CO2
5V   ─────────────── VCC
GND  ─────────────── GND
D2   ←──── yellow ── TX   (sensor sends data)
D3   ──── green  ──→ RX   (optional commands)

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
