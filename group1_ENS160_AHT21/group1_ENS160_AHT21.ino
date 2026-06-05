/*
 * GROUP 1 — ENS160 + AHT21 + BMP280  [MOX + NDIR comparison]
 * Sample every 15 s, write row to SD immediately, print to Serial.
 * Missing sensor → -1. File: YYYYMMDD_HHMM.txt
 *
 * Wiring (I2C: A4=SDA, A5=SCL):
 *   AHT21  3.3V  GND  SDA→A4  SCL→A5  (0x38)
 *   BMP280 3.3V  GND  SDA→A4  SCL→A5  (0x76)
 *   ENS160 3.3V  GND  SDA→A4  SCL→A5  ADDR→GND → 0x52
 *   Logging Shield: SD D10-D13, RTC A4/A5
 *
 * Libraries: RTClib, Adafruit AHTX0, Adafruit BMP280,
 *            ScioSense ENS160 (search: ScioSense_ENS160), SD (built-in)
 *
 * ENS160 ADDR pin: LOW → 0x52 | HIGH → 0x53
 * Pass T/RH from AHT21 to ENS160 for compensation (improves accuracy).
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <ScioSense_ENS160.h>

#define SD_CS      10
#define LED        13
#define SAMPLE_MS  15000UL
#define MARK_MS   180000UL
#define ROW_LEN    60

RTC_DS1307        rtc;
Adafruit_AHTX0    aht;
Adafruit_BMP280   bmp;
ScioSense_ENS160  ens(ENS160_I2CADDR_1);  // ADDR→GND = 0x52

bool rtcOK=false,sdOK=false,ahtOK=false,bmpOK=false,ensOK=false;
unsigned long lastSample=0,lastMark=0;

uint8_t appendFP1(char *buf,uint8_t pos,long v){
  if(v==-1){buf[pos++]='-';buf[pos++]='1';return pos;}
  if(v<0){buf[pos++]='-';v=-v;}
  long w=v/10,d=v%10;
  if(w>=1000){buf[pos++]=(char)('0'+w/1000);}
  if(w>=100){buf[pos++]=(char)('0'+w/100%10);}
  if(w>=10){buf[pos++]=(char)('0'+w/10%10);}
  buf[pos++]=(char)('0'+w%10);buf[pos++]='.';buf[pos++]=(char)('0'+d);return pos;
}
uint8_t appendLong(char *buf,uint8_t pos,long v){
  if(v==-1){buf[pos++]='-';buf[pos++]='1';return pos;}
  if(v==0){buf[pos++]='0';return pos;}
  char tmp[8];int8_t i=0;long u=v;
  while(u>0){tmp[i++]=(char)('0'+(u%10));u/=10;}
  while(i>0) buf[pos++]=tmp[--i];return pos;
}
void buildFilename(char *buf,const DateTime &dt){
  // 8.3 filename: YYMMDDHH.txt  (SD library requires max 8 chars before dot)
  buf[0]=(char)('0'+dt.year()/10%10); buf[1]=(char)('0'+dt.year()%10);
  buf[2]=(char)('0'+dt.month()/10);   buf[3]=(char)('0'+dt.month()%10);
  buf[4]=(char)('0'+dt.day()/10);     buf[5]=(char)('0'+dt.day()%10);
  buf[6]=(char)('0'+dt.hour()/10);    buf[7]=(char)('0'+dt.hour()%10);
  buf[8]='.'; buf[9]='t'; buf[10]='x'; buf[11]='t'; buf[12]='\0';
}
uint8_t appendTS(char *buf,uint8_t pos,const DateTime &dt){
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

void setup(){
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(SD_CS,OUTPUT);digitalWrite(SD_CS,HIGH);delay(100);
  Wire.begin();delay(100);
  if(rtc.begin()){rtcOK=true;
    if(!rtc.isrunning())rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
    Serial.println(F("RTC OK"));}else Serial.println(F("RTC missing"));
  {Sd2Card c;c.init(SPI_HALF_SPEED,SD_CS);}
  if(SD.begin(SD_CS)){sdOK=true;Serial.println(F("SD OK"));}
  else Serial.println(F("SD not found"));
  if(aht.begin()){ahtOK=true;Serial.println(F("AHT21 OK"));}
  else Serial.println(F("AHT21 missing"));
  if(bmp.begin(0x77)){bmpOK=true;Serial.println(F("BMP280 OK"));}
  else Serial.println(F("BMP280 missing"));
  if(ens.begin()&&ens.available()){ens.setMode(ENS160_OPMODE_STD);ensOK=true;Serial.println(F("ENS160 OK"));}
  else Serial.println(F("ENS160 missing"));
  Serial.println(F("timestamp,temp_C,rh_pct,pres_hPa,eCO2,TVOC,AQI"));
  lastSample=lastMark=millis();
}

void loop(){
  unsigned long now=millis();
  if(now-lastSample>=SAMPLE_MS){
    lastSample=now;
    long t=-1,rh=-1,p=-1,eco2=-1,tvoc=-1;int8_t aqi=-1;
    if(ahtOK){sensors_event_t eh,et;aht.getEvent(&eh,&et);
      t=(long)(et.temperature*10.0f+0.5f);rh=(long)(eh.relative_humidity*10.0f+0.5f);}
    if(bmpOK) p=(long)(bmp.readPressure()/10.0f+0.5f);
    if(ensOK){
      if(ahtOK) ens.set_envdata(t/10.0f,rh/10.0f);
      ens.measure(true);
      eco2=(long)ens.geteCO2();tvoc=(long)ens.getTVOC();aqi=(int8_t)ens.getAQI();}
    char row[ROW_LEN];uint8_t pos=0;
    DateTime dt=rtcOK?rtc.now():DateTime(2000,1,1,0,0,0);
    pos=appendTS(row,pos,dt);
    row[pos++]=',';pos=appendFP1(row,pos,t);
    row[pos++]=',';pos=appendFP1(row,pos,rh);
    row[pos++]=',';pos=appendFP1(row,pos,p);
    row[pos++]=',';pos=appendLong(row,pos,eco2);
    row[pos++]=',';pos=appendLong(row,pos,tvoc);
    row[pos++]=',';row[pos++]=(aqi>=0?(char)('0'+aqi):'-');
    row[pos]='\0';
    Serial.println(row);
    if(sdOK){
      char fname[13];buildFilename(fname,dt);
      bool isNew=!SD.exists(fname);
      File f=SD.open(fname,FILE_WRITE);
      if(f){if(isNew)f.println(F("timestamp,temp_C,rh_pct,pres_hPa,eCO2,TVOC,AQI"));
        f.println(row);f.close();
        digitalWrite(LED,HIGH);delay(80);digitalWrite(LED,LOW);}
      else Serial.println(F("SD open fail"));}
    if(now-lastMark>=MARK_MS){lastMark=now;blinkFast(2);}
  }
}
