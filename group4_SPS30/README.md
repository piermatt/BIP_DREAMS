# Group 4 — SPS30 + AHT20 + BMP280  [INDOOR]

**Principle:** Laser particle counting (Mie scattering OPC)  
**Placement:** Indoors  
**Comparison task:** PM baseline for Groups 5 & 6

## Required Libraries

Install via **Sketch → Include Library → Manage Libraries**:

| Library name | Author | Notes |
|--------------|--------|-------|
| `RTClib` | Adafruit | RTC support |
| `Adafruit AHTX0` | Adafruit | Temperature & humidity |
| `Adafruit BMP280` | Adafruit | Pressure sensor |
| `sensirion-sps` | Sensirion | SPS30 PM sensor (search "SPS30 Sensirion") |

`SD`, `Wire`, `SPI` are **built-in**.

## I²C Addresses

| Device | Address |
|--------|---------|
| RTC DS1307 (shield) | 0x68 |
| AHT20 | 0x38 |
| BMP280 | 0x76 |
| SPS30 | 0x69 |

## Wiring Summary

**Critical:** SPS30 pin 5 (SEL) **must be connected to GND** to enable I²C mode.  
SPS30 requires **5 V** and draws ~60 mA (fan running). See `WIRING.md`.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, temp_C, rh_pct, pres_hPa, PM1, PM25, PM4, PM10, NC05, NC1, NC25`

## Notes

- SPS30 fan needs ~8 s spin-up. The sketch waits automatically.
- Audible hum from fan is normal.
- NC = number concentration (#/cm³), PMx = mass concentration (μg/m³).
