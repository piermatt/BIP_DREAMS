# Group 6 — SCD41 + SPS30 + AHT20 + BMP280  [OUTDOOR]

**Principles:** Photoacoustic NDIR (CO₂) + Laser OPC (PM)  
**Placement:** OUTDOORS — paired with Group 5 (indoor)  
**Task:** Provide outdoor PM reference for infiltration factor calculation

## Required Libraries

Install via **Sketch → Include Library → Manage Libraries**:

| Library name | Author | Notes |
|--------------|--------|-------|
| `RTClib` | Adafruit | RTC support |
| `Adafruit AHTX0` | Adafruit | Temperature & humidity |
| `Adafruit BMP280` | Adafruit | Pressure sensor |
| `Sensirion I2C SCD4x` | Sensirion | SCD41 CO₂ sensor |
| `sensirion-sps` | Sensirion | SPS30 PM sensor |

`SD`, `Wire`, `SPI` are **built-in**.

## I²C Addresses

| Device | Address |
|--------|---------|
| RTC DS1307 (shield) | 0x68 |
| AHT20 | 0x38 |
| BMP280 | 0x76 |
| SCD41 | 0x62 |
| SPS30 | 0x69 |

## Wiring Summary

Identical to Group 5. SPS30 pin 5 (SEL) → GND. SCD41 and SPS30 on 5 V.  
See `WIRING.md`. Place the node sheltered from direct rain; avoid direct sun on BMP280.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, placement, CO2_ppm, scd_T, scd_RH, aht_T, aht_RH, pres_hPa, PM1, PM25, PM4, PM10, NC25`

## Notes for Outdoor Deployment

- Shelter the node from rain (open box facing down, or place under overhang).
- Avoid placing SPS30 intake directly into wind — causes artificially high readings.
- Outdoor CO₂ should read ~420 ppm; use this to verify SCD41 calibration.
- Synchronise RTC with Group 5 before deployment (see Group 5 README).
