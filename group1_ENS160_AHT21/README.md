# Group 1 — ENS160 + AHT21 + BMP280

**Principle:** Metal Oxide Semiconductor (MOX)  
**Placement:** Indoors  
**Comparison task:** CO₂ comparison with Groups 2 & 3

## Required Libraries

Install all of these via **Sketch → Include Library → Manage Libraries**:

| Library name (search exactly) | Author | Notes |
|-------------------------------|--------|-------|
| `RTClib` | Adafruit | RTC DS1307 support |
| `Adafruit AHTX0` | Adafruit | AHT21 temperature & humidity |
| `Adafruit BMP280` | Adafruit | Pressure sensor |
| `ScioSense ENS160` | ScioSense | MOX CO₂/TVOC sensor |

`SD` and `Wire` / `SPI` are **built-in** — do not install separately.

## I²C Addresses

| Device | Address |
|--------|---------|
| RTC DS1307 (shield) | 0x68 |
| AHT21 | 0x38 |
| BMP280 | 0x76 |
| ENS160 | 0x53 |

## Wiring Summary

All sensors share the I²C bus (A4 = SDA, A5 = SCL). Power AHT21, BMP280, ENS160 from **3.3 V**.  
See `WIRING.md` for full details.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, temp_C, rh_pct, pres_hPa, eCO2_ppm, TVOC_ppb, AQI`

## Notes

- ENS160 requires T/RH compensation — the sketch passes AHT21 readings automatically.
- AHT20 and AHT21 share address 0x38: use **only one** on this bus (AHT21 here).
- Allow 5–10 min warm-up before trusting eCO₂ readings.
