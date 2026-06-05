# Group 2 — JW01-CO2 (NDIR) + AHT20 + BMP280

**Principle:** Non-Dispersive Infrared (NDIR)  
**Placement:** Indoors  
**Comparison task:** CO₂ comparison with Groups 1 & 3

## Required Libraries

Install via **Sketch → Include Library → Manage Libraries**:

| Library name | Author | Notes |
|--------------|--------|-------|
| `RTClib` | Adafruit | RTC support |
| `Adafruit AHTX0` | Adafruit | Temperature & humidity |
| `Adafruit BMP280` | Adafruit | Pressure sensor |

`SD`, `Wire`, `SPI`, `SoftwareSerial` are **built-in**.

## I²C Addresses

| Device | Address |
|--------|---------|
| RTC DS1307 (shield) | 0x68 |
| AHT20 | 0x38 |
| BMP280 | 0x76 |

## Wiring Summary

JW01 uses **SoftwareSerial** on D2 (RX) / D3 (TX) — **not** D0/D1.  
AHT20 and BMP280 on I²C (A4/A5). See `WIRING.md`.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, temp_C, rh_pct, pres_hPa, CO2_ppm`

## Notes

- JW01 NDIR lamp needs **~3 minutes** warm-up after power-on.
- First readings may be 0 or erratic — this is normal.
- Do **not** connect JW01 to D0/D1 (conflicts with USB Serial during upload).
