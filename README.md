# Low-Cost Air Quality Sensing — DREAMS BIP

How building low cost sensors for IAQ and water quality – Blended Intensive Programme (BIP) DREAMS - 15-19 June 2026

---

## Overview

Six groups build independent sensor nodes based on **Arduino Uno + Logging Recorder Shield V1.0**.  
Groups 1–4 compare CO2 sensors (indoor).  
Groups 5 & 6 measure PM2.5 (indoor vs outdoor).

All nodes log timestamped CSV data to a microSD card at **5-minute intervals** for multi-day comparison.

---

## Repository Structure

```
lab-airquality/
├── README.md               ← You are here
├── presentation.slides.html ← Introductory lecture slides
├── docs/
│   └── ASSIGNMENT_TABLES.md ← Per-group tasks, discussion questions, report requirements
├── group1_ENS160_AHT21/
│   ├── group1_ENS160_AHT21.ino
│   └── WIRING.md
├── group2_JW01_CO2/
│   ├── group2_JW01_CO2.ino
│   └── WIRING.md
├── group3_SCD41_indoor/
│   ├── group3_SCD41_indoor.ino
│   └── WIRING.md
├── group4_SPS30/
│   ├── group4_SPS30.ino
│   └── WIRING.md
├── group5_SCD41_outdoor/     ← INDOOR node
│   ├── group5_SCD41_SPS30_indoor.ino
│   └── WIRING.md
└── group6_SCD41_outdoor/     ← OUTDOOR node
    ├── group6_SCD41_SPS30_outdoor.ino
    └── WIRING.md
```

---

## Quick Start — Arduino IDE Setup

### 1. Install Arduino IDE 2
Download from: https://www.arduino.cc/en/software  
Run the installer with default options. No account needed.

### 2. Install CH340 USB Driver (older Windows)
The Arduino Uno clone boards used in this lab use the CH340G USB-Serial chip.

| OS | Download |
|----|----------|
| Windows 10/11 | https://www.wch-ic.com/downloads/CH341SER_EXE.html |
| macOS | https://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html |
| Linux ≥ kernel 3.4 | Built-in — no driver needed |

After installing: **Reboot**, then plug in Arduino. The board appears as `COM3` to `COM9` on Windows or `/dev/ttyUSB0` on Linux.

### 3. Select Board & Port
In Arduino IDE:
- `Tools → Board → Arduino AVR Boards → Arduino Uno`
- `Tools → Port → COMx` (Windows) or `/dev/ttyUSBx` (Linux/Mac)

### 4. Upload Blink (test)
`File → Examples → 01.Basics → Blink`  
Click ▶ (Upload) or press `Ctrl+U`.  
The LED on pin 13 should blink every second. ✓

### 5. Open Serial Monitor
`Tools → Serial Monitor` or `Ctrl+Shift+M`  
Set baud rate to **9600** (matches all lab sketches).

---

## Install Libraries

Open the Library Manager: `Sketch → Include Library → Manage Libraries`

Install the following:

| Library | Author | Used by |
|---------|--------|---------|
| RTClib | Adafruit | All groups |
| Adafruit AHTX0 | Adafruit | All groups |
| Adafruit BMP280 | Adafruit | All groups |
| Sensirion I2C SCD4x | Sensirion | Groups 3, 5, 6 |
| sensirion-sps | Sensirion | Groups 4, 5, 6 |
| ScioSense ENS160 | ScioSense | Group 1 |

> SD library and SoftwareSerial are built into the Arduino IDE — no installation needed.

---

## Setting the RTC Clock

The Logging Shield uses a DS1307 RTC. Set the time once:

1. In the sketch `setup()`, uncomment: `rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));`
2. Upload the sketch (this sets RTC to your computer's compile time)
3. Comment out that line again
4. Upload again (RTC now runs independently from the CR1220 battery)

---

## LED Indicator Behavior

| LED state | Meaning |
|-----------|---------|
| 2× fast blink (100 ms) | **Warning:** SD write in 5 seconds — do not remove card |
| Solid ON for 1 second | **Writing** to SD card |
| Off | Normal operation — sampling sensors |

### Safe SD Removal
1. Wait for LED to go off
2. Unplug USB power
3. Remove SD card
4. Copy CSV files to your computer

---

## Data Format

Each file is named `YYYYMMDD_HH.csv` (one per hour).

Example: `20260605_14.csv` = June 5 2026, 14:00 hour.

Column headers are written automatically when a new file is created.

---

## Quick Python Analysis

```python
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv('20260605_14.csv', parse_dates=['timestamp'])

# Time-series plot
df.plot(x='timestamp', y='CO2_ppm', title='CO2 vs Time')
plt.ylabel('CO2 (ppm)'); plt.tight_layout(); plt.show()

# Noise estimate (stable period)
stable = df[df['timestamp'].dt.hour == 3]  # night
sigma = stable['CO2_ppm'].std()
swing = df['CO2_ppm'].max() - df['CO2_ppm'].min()
print(f"σ noise = {sigma:.1f} ppm")
print(f"SNR = {20 * (swing/sigma).__class__.__name__}")
import numpy as np
print(f"SNR = {20*np.log10(swing/sigma):.1f} dB")

# Infiltration factor (Groups 5 & 6)
df_in  = pd.read_csv('indoor.csv', parse_dates=['timestamp'])
df_out = pd.read_csv('outdoor.csv', parse_dates=['timestamp'])
merged = pd.merge_asof(df_in.sort_values('timestamp'),
                       df_out.sort_values('timestamp'),
                       on='timestamp', suffixes=('_in', '_out'),
                       tolerance=pd.Timedelta('5min'))
merged['F'] = merged['PM25_in'] / merged['PM25_out']
print(f"Mean F (PM2.5) = {merged['F'].mean():.3f} ± {merged['F'].std():.3f}")
```

---

## Sensor Datasheets

| Sensor | Datasheet |
|--------|-----------|
| ENS160 | https://www.sciosense.com/wp-content/uploads/2023/12/ENS160-Datasheet.pdf |
| AHT20 / AHT21 | https://cdn.sparkfun.com/assets/d/2/b/e/d/AHT20.pdf |
| BMP280 | https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf |
| JW01-CO2 | Provided by instructor (manufacturer PDF) |
| SCD41 | https://sensirion.com/media/documents/48C4B7FB/64C134E7/Sensirion_SCD4x_Datasheet.pdf |
| SPS30 | https://sensirion.com/media/documents/8600FF3E/616542B5/Sensirion_PM_Sensors_Datasheet_SPS30.pdf |

---

*Contact: [matpierp@pg.edu.pl]*  
*GitHub: [https://github.com/piermatt/]*
