/*
 * GROUP 2 — JW01-CO2-V2.2 (NDIR, auto-push) + AHT20 + BMP280
 * Sample every 15 s, write row to SD immediately, print to Serial.
 * Missing sensor → -1. File: YYMMDDHH.txt (8.3 FAT compliant)
 *
 * Wiring:
 *   AHT20  5V  GND  SDA→A4  SCL→A5  (I2C 0x38)
 *   BMP280 3.3V GND SDA→A4  SCL→A5  (I2C 0x76)
 *   JW01   5V  GND  TX (B)→D2   RX (A)→D3   (SoftwareSerial 9600)
 *              JW01 continuously pushes 6-byte packets, no command needed.
 *   Logging Shield: SD D10-D13 (SPI), RTC A4/A5 (I2C)
 *
 * JW01 packet format (6 bytes, auto ~2 s interval):
 *   [0] 0x2C  start byte
 *   [1] CO2_H  high byte
 *   [2] CO2_L  low byte
 *   [3] 0x00
 *   [4] 0x00
 *   [5] checksum = (b0+b1+b2+b3+b4) & 0xFF
 *   CO2 ppm = b[1]*256 + b[2]
 *
 * Libraries: RTClib, Adafruit AHTX0, Adafruit BMP280, SD (built-in)
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#define SD_CS      10
#define LED        13
#define SAMPLE_MS  15000UL
#define MARK_MS   180000UL
#define ROW_LEN    52

RTC_DS1307      rtc;
Adafruit_AHTX0  aht;
Adafruit_BMP280 bmp;
SoftwareSerial  jw01(2, 3);   // D2=RX (←JW01 TX), D3=TX (→JW01 RX, unused)

bool rtcOK=false, sdOK=false, ahtOK=false, bmpOK=false;
unsigned long lastSample=0, lastMark=0;

// JW01 packet state (updated continuously in background)
byte   jw01buf[6];
uint8_t jw01cnt = 0;
int    jw01co2  = -1;   // last valid CO2 ppm, -1 until first good packet

// ── JW01: call every loop(), drains SoftwareSerial buffer ────────
void pollJW01() {
  while (jw01.available() > 0) {
    byte b = jw01.read();
    if (jw01cnt == 0 && b != 0x2C) continue;  // wait for start byte
    jw01buf[jw01cnt++] = b;
    if (jw01cnt == 6) {
      byte cs = jw01buf[0]+jw01buf[1]+jw01buf[2]+jw01buf[3]+jw01buf[4];
      if (cs == jw01buf[5])
        jw01co2 = (int)jw01buf[1]*256 + jw01buf[2];
      // else: checksum fail — keep previous value, discard packet
      jw01cnt = 0;
    }
  }
}

// ── Helpers ──────────────────────────────────────────────────────
uint8_t appendFP1(char *buf, uint8_t pos, long v) {
  if(v==-1){buf[pos++]='-';buf[pos++]='1';return pos;}
  if(v<0){buf[pos++]='-';v=-v;}
  long w=v/10,d=v%10;
  if(w>=1000){buf[pos++]=(char)('0'+w/1000);}
  if(w>=100) {buf[pos++]=(char)('0'+w/100%10);}
  if(w>=10)  {buf[pos++]=(char)('0'+w/10%10);}
  buf[pos++]=(char)('0'+w%10);buf[pos++]='.';buf[pos++]=(char)('0'+d);return pos;
}
uint8_t appendLong(char *buf, uint8_t pos, long v) {
  if(v==-1){buf[pos++]='-';buf[pos++]='1';return pos;}
  if(v==0){buf[pos++]='0';return pos;}
  char tmp[8];int8_t i=0;long u=v;
  while(u>0){tmp[i++]=(char)('0'+(u%10));u/=10;}
  while(i>0) buf[pos++]=tmp[--i];return pos;
}
void buildFilename(char *buf, const DateTime &dt) {
  // 8.3 filename: YYMMDDHH.txt  (SD library requires max 8 chars before dot)
  buf[0]=(char)('0'+dt.year()/10%10); buf[1]=(char)('0'+dt.year()%10);
  buf[2]=(char)('0'+dt.month()/10);   buf[3]=(char)('0'+dt.month()%10);
  buf[4]=(char)('0'+dt.day()/10);     buf[5]=(char)('0'+dt.day()%10);
  buf[6]=(char)('0'+dt.hour()/10);    buf[7]=(char)('0'+dt.hour()%10);
  buf[8]='.'; buf[9]='t'; buf[10]='x'; buf[11]='t'; buf[12]='\0';
}
uint8_t appendTS(char *buf, uint8_t pos, const DateTime &dt) {
  buf[pos++]=(char)('0'+dt.year()/1000);buf[pos++]=(char)('0'+dt.year()/100%10);
  buf[pos++]=(char)('0'+dt.year()/10%10);buf[pos++]=(char)('0'+dt.year()%10);
  buf[pos++]='-';buf[pos++]=(char)('0'+dt.month()/10);buf[pos++]=(char)('0'+dt.month()%10);
  buf[pos++]='-';buf[pos++]=(char)('0'+dt.day()/10);buf[pos++]=(char)('0'+dt.day()%10);
  buf[pos++]=' ';buf[pos++]=(char)('0'+dt.hour()/10);buf[pos++]=(char)('0'+dt.hour()%10);
  buf[pos++]=':';buf[pos++]=(char)('0'+dt.minute()/10);buf[pos++]=(char)('0'+dt.minute()%10);
  buf[pos++]=':';buf[pos++]=(char)('0'+dt.second()/10);buf[pos++]=(char)('0'+dt.second()%10);
  return pos;
}
void blinkFast(uint8_t cnt){
  for(uint8_t i=0;i<cnt;i++){digitalWrite(LED,HIGH);delay(100);digitalWrite(LED,LOW);delay(100);}
}

// ── Setup ────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  jw01.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(SD_CS, OUTPUT); digitalWrite(SD_CS, HIGH); delay(100);
  Wire.begin(); delay(100);

  if(rtc.begin()){ rtcOK=true;
    if(!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
    Serial.println(F("RTC OK")); } else Serial.println(F("RTC missing"));

  { Sd2Card c; c.init(SPI_HALF_SPEED, SD_CS); }
  if(SD.begin(SD_CS)){ sdOK=true; Serial.println(F("SD OK")); }
  else Serial.println(F("SD not found"));

  if(aht.begin()){ ahtOK=true; Serial.println(F("AHT20 OK")); }
  else Serial.println(F("AHT20 missing"));
  if(bmp.begin(0x76)){ bmpOK=true; Serial.println(F("BMP280 OK")); }
  else Serial.println(F("BMP280 missing"));

  Serial.println(F("JW01: waiting for first packet (warm-up ~1-3 min)..."));
  Serial.println(F("timestamp,temp_C,rh_pct,pres_hPa,CO2_ppm"));
  lastSample=lastMark=millis();
}

// ── Loop ─────────────────────────────────────────────────────────
void loop() {
  pollJW01();   // drain serial buffer every iteration — never blocks

  unsigned long now=millis();
  if(now-lastSample >= SAMPLE_MS){
    lastSample=now;

    long t=-1, rh=-1, p=-1;
    if(ahtOK){ sensors_event_t eh,et; aht.getEvent(&eh,&et);
      t =(long)(et.temperature*10.0f+0.5f);
      rh=(long)(eh.relative_humidity*10.0f+0.5f); }
    if(bmpOK) p=(long)(bmp.readPressure()/10.0f+0.5f);

    char row[ROW_LEN]; uint8_t pos=0;
    DateTime dt=rtcOK?rtc.now():DateTime(2000,1,1,0,0,0);
    pos=appendTS(row,pos,dt);
    row[pos++]=','; pos=appendFP1(row,pos,t);
    row[pos++]=','; pos=appendFP1(row,pos,rh);
    row[pos++]=','; pos=appendFP1(row,pos,p);
    row[pos++]=','; pos=appendLong(row,pos,(long)jw01co2);
    row[pos]='\0';

    Serial.println(row);

    if(sdOK){
      char fname[13]; buildFilename(fname,dt);
      bool isNew=!SD.exists(fname);
      File f=SD.open(fname,FILE_WRITE);
      if(f){ if(isNew) f.println(F("timestamp,temp_C,rh_pct,pres_hPa,CO2_ppm"));
        f.println(row); f.close();
        digitalWrite(LED,HIGH); delay(80); digitalWrite(LED,LOW); }
      else Serial.println(F("SD open fail"));
    }
    if(now-lastMark>=MARK_MS){ lastMark=now; blinkFast(2); }
  }
}
