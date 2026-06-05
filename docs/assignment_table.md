# Lab Assignment Table — Low-Cost Air Quality Sensing

**Date:** ________________  
**Duration:** 2 hours  
**Each group:** 2–3 students

---

## Group 1 — ENS160 + AHT21 (MOX eCO₂ / TVOC, Indoor CO₂ comparison)

| Task | Done? | Notes |
|------|-------|-------|
| Wire AHT20+BMP280 module to Arduino (I2C: A4/A5, 5V, GND) | ☐ | |
| Wire ENS160+AHT21 module to Arduino (I2C: A4/A5, 5V, GND) | ☐ | ENS160 addr = 0x53 |
| Stack Logging Shield and insert SD card | ☐ | |
| Install libraries: RTClib, Adafruit AHTX0, Adafruit BMP280, ScioSense ENS160 | ☐ | |
| Set RTC time (upload sketch with compile-time date, then re-upload) | ☐ | |
| Upload group1.ino and verify Serial Monitor output | ☐ | |
| Confirm LED blink sequence (2×fast → 1×long) | ☐ | |
| Leave logger running — collect at least 4 rows | ☐ | |
| Download CSV from SD card | ☐ | |
| **Analysis:** Plot eCO₂ vs time. Note warm-up drift (first 60 min). | ☐ | |
| **Analysis:** Compare eCO₂ to Group 3 SCD41 values. What differences? | ☐ | |
| **Report questions** (see below) | ☐ | |

**Background questions:**
1. What is metal-oxide (MOX) sensing? How does resistance change with gas concentration?  
2. Why does the ENS160 measure "eCO₂" not true CO₂? What does it actually detect?  
3. What is AQI and how is it derived from TVOC?  
4. Why must T/RH compensation be applied to ENS160 readings?  
5. Compare the price of ENS160 vs SCD41. Justify the difference.

---

## Group 2 — JW01-CO2 / SEN0220 (NDIR UART, Indoor CO₂ comparison)

| Task | Done? | Notes |
|------|-------|-------|
| Wire AHT20+BMP280 to I2C (A4/A5, 5V, GND) | ☐ | |
| Wire JW01-CO2: VCC→5V, GND→GND, TX_sensor→D2, RX_sensor→D3 | ☐ | SoftwareSerial |
| Stack Logging Shield | ☐ | |
| Install libraries: RTClib, Adafruit AHTX0, Adafruit BMP280 (SoftwareSerial built-in) | ☐ | |
| Set RTC, upload group2.ino | ☐ | |
| Wait 3 min for sensor pre-heat, verify CO2 readings on Serial Monitor | ☐ | |
| Confirm LED blink and start logging | ☐ | |
| Download CSV from SD | ☐ | |
| **Analysis:** Plot CO₂ vs time. Identify pre-heat drift period. | ☐ | |
| **Analysis:** Compare response to sudden ventilation event (open window). | ☐ | |

**Background questions:**
1. Draw and explain the NDIR measurement principle (light source, gas cell, detector).  
2. Why does the JW01 have a 3-minute pre-heat time?  
3. What is temperature compensation in a CO₂ sensor? Why is it needed?  
4. Explain the 9-byte UART frame and checksum. What happens with a bad checksum?  
5. What is the T90 response time? Define it formally.

---

## Group 3 — SCD41 (Photoacoustic NDIR, I2C, Indoor CO₂ reference)

| Task | Done? | Notes |
|------|-------|-------|
| Wire AHT20+BMP280 and SCD41 to I2C (A4/A5, 5V, GND) | ☐ | SCD41 addr = 0x62 |
| Stack Logging Shield | ☐ | |
| Install: RTClib, Adafruit AHTX0, Adafruit BMP280, Sensirion I2C SCD4x | ☐ | |
| Set RTC, upload group3.ino | ☐ | |
| Wait for first measurement (≥5 s), verify on Serial Monitor | ☐ | |
| Confirm LED blink and start logging | ☐ | |
| Download CSV from SD | ☐ | |
| **Analysis:** Plot CO₂ from SCD41 and from AHT20 T/RH on same time axis. | ☐ | |
| **Analysis:** Compare SCD41 CO₂ vs Group 1 ENS160 eCO₂ and Group 2 JW01. | ☐ | |

**Background questions:**
1. Explain photoacoustic NDIR. How does the PASens® principle differ from classic NDIR?  
2. What is the role of the built-in SHT4x sensor in the SCD41?  
3. What is Automatic Self-Calibration (ASC)? When should it be disabled?  
4. Outdoor CO₂ baseline is ~420 ppm (2024). What causes elevated indoor levels?  
5. At what CO₂ concentration does cognitive performance begin to decrease? (cite study)

---

## Group 4 — SPS30 (Laser scattering PM, I2C, Indoor PM reference)

| Task | Done? | Notes |
|------|-------|-------|
| Wire AHT20+BMP280 to I2C | ☐ | |
| Wire SPS30: VDD→5V, GND→GND, SEL→GND (I2C mode), SDA→A4, SCL→A5 | ☐ | JST-ZHR-5 cable |
| Stack Logging Shield | ☐ | |
| Install: RTClib, Adafruit AHTX0, Adafruit BMP280, sensirion-sps | ☐ | |
| Set RTC, upload group4.ino | ☐ | |
| Wait ~8 s for fan start, verify PM readings on Serial Monitor | ☐ | |
| Start logging, note any PM spikes (cooking? walking past?) | ☐ | |
| Download CSV from SD | ☐ | |
| **Analysis:** Plot PM1, PM2.5, PM10 vs time. Note ratio PM2.5/PM10. | ☐ | |
| **Analysis:** Identify any indoor emission events (candle, aerosol spray). | ☐ | |

**Background questions:**
1. Explain the laser scattering (Mie scattering) principle for particle counting.  
2. What is the difference between PM1, PM2.5, PM4, and PM10?  
3. How is number concentration different from mass concentration?  
4. Why are sub-2.5 μm particles more dangerous to human health?  
5. What is Sensirion's "contamination resistance" technology in SPS30?

---

## Groups 5 & 6 — SCD41 + SPS30 (Indoor vs Outdoor PM sensing)

| Task | Done? | Notes |
|------|-------|-------|
| Wire AHT20+BMP280, SCD41, and SPS30 to I2C (all share A4/A5) | ☐ | |
| Stack Logging Shield | ☐ | |
| Install all required libraries (RTClib, AHTX0, BMP280, SCD4x, sensirion-sps) | ☐ | |
| **Group 5:** confirm `#define LOCATION "INDOOR"` in sketch | ☐ | |
| **Group 6:** confirm `#define LOCATION "OUTDOOR"` in sketch | ☐ | |
| Set RTC, upload group5.ino (or group6.ino) | ☐ | |
| **Group 5:** Place sensor INSIDE classroom | ☐ | Away from vents |
| **Group 6:** Place sensor OUTSIDE (windowsill or corridor) | ☐ | Protected from rain |
| Start logging simultaneously (synchronise time!) | ☐ | |
| Download CSV from both SD cards after session | ☐ | |
| **Analysis:** Plot PM2.5 indoor vs outdoor vs time. Calculate I/O ratio. | ☐ | |
| **Analysis:** Calculate infiltration factor F = PM_indoor / PM_outdoor | ☐ | |

**Background questions:**
1. Define "infiltration factor" and "penetration coefficient" for PM.  
2. What building factors affect indoor PM? (windows, HVAC filters, sealing)  
3. Study: median US residential infiltration factor for PM2.5 is ~0.52. How does yours compare?  
4. How does outdoor CO₂ level compare between both groups? Should it differ?  
5. What meteorological parameters affect outdoor PM levels?

---

## Cross-Group Analysis (after data download)

All groups pool their data and answer:

| Question | Groups |
|----------|--------|
| Plot all CO₂ sensors on one graph (normalised 0–1). Calculate Pearson r between each pair. | 1, 2, 3 |
| Calculate SNR (signal-to-noise ratio) for each CO₂ sensor during a stable period (night). | 1, 2, 3 |
| Estimate T50 and T90 response time for each CO₂ sensor (use window-opening event). | 1, 2, 3 |
| Compare PM2.5 indoor vs outdoor. Plot I/O ratio as a function of time. | 4, 5, 6 |
| Apply Nyquist-Shannon: what is the minimum required sampling rate for CO₂ monitoring? | All |
| Identify which sensor has the best price-to-performance ratio. Justify. | All |

