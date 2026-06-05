# Group 3 — SCD41 + AHT20 + BMP280  [INDOOR]

**Principle:** Photoacoustic NDIR (Sensirion PASens®)  
**Placement:** Indoors  
**Comparison task:** CO₂ reference sensor for Groups 1 & 2

## Required Libraries

Install via **Sketch → Include Library → Manage Libraries**:

| Library name | Author | Notes |
|--------------|--------|-------|
| `RTClib` | Adafruit | RTC support |
| `Adafruit AHTX0` | Adafruit | Temperature & humidity |
| `Adafruit BMP280` | Adafruit | Pressure sensor |
| `Sensirion I2C SCD4x` | Sensirion | SCD41 CO₂ sensor |

`SD`, `Wire`, `SPI` are **built-in**.

## I²C Addresses

| Device | Address |
|--------|---------|
| RTC DS1307 (shield) | 0x68 |
| AHT20 | 0x38 |
| BMP280 | 0x76 |
| SCD41 | 0x62 |

## Wiring Summary

All I²C (A4/A5). SCD41 requires **5 V** supply. See `WIRING.md`.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, scd_CO2_ppm, scd_temp_C, scd_rh_pct, aht_temp_C, aht_rh_pct, pres_hPa`

## Notes

- Both SCD41 T/RH and AHT20 T/RH are logged — compare to study SCD41 self-heating offset.
- SCD41 built-in Automatic Self-Calibration (ASC) activates after ~1 week continuous operation.
- First valid measurement available ~5 s after `startPeriodicMeasurement()`.
