# Group 5 — SCD41 + SPS30 + AHT20 + BMP280  [INDOOR]

**Principles:** Photoacoustic NDIR (CO₂) + Laser OPC (PM)  
**Placement:** INDOORS — paired with Group 6 (outdoor)  
**Task:** Measure PM infiltration factor F = PM_indoor / PM_outdoor

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

All I²C (A4/A5). SPS30 pin 5 (SEL) → GND. SCD41 and SPS30 both on 5 V.  
See `WIRING.md`.

## Data Logged (5-min average of 20 × 15 s samples)

`timestamp, placement, CO2_ppm, scd_T, scd_RH, aht_T, aht_RH, pres_hPa, PM1, PM25, PM4, PM10, NC25`

## Synchronisation with Group 6

Both Group 5 and 6 must have their RTC set at the same time before deployment.  
Set RTC with `rtc.adjust(DateTime(F(__DATE__), F(__TIME__)))`, upload, remove that line, re-upload.  
Verify timestamps agree on Serial Monitor before separating the nodes.

## Post-Lab: Infiltration Factor

```python
import pandas as pd
df_in  = pd.read_csv('group5_YYYYMMDD_HH.csv', parse_dates=['timestamp'])
df_out = pd.read_csv('group6_YYYYMMDD_HH.csv', parse_dates=['timestamp'])
merged = pd.merge_asof(df_in.sort_values('timestamp'),
                       df_out.sort_values('timestamp'),
                       on='timestamp', suffixes=('_in','_out'),
                       tolerance=pd.Timedelta('6min'))
merged['F_PM25'] = merged['PM25_in'] / merged['PM25_out']
print(merged[['timestamp','PM25_in','PM25_out','F_PM25']].to_string())
```
