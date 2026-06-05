/*
 * GROUP 3 — SCD41 + AHT20 + BMP280  [INDOOR]
 * Sample every 15 s, write row to SD immediately, print to Serial.
 * Missing sensor → -1. File: YYYYMMDD_HHMM.txt
 *
 * Wiring (I2C: A4=SDA, A5=SCL):
 *   AHT20  5V  GND  SDA→A4  SCL→A5  (0x38)
 *   BMP280 3.3V GND SDA→A4  SCL→A5  (0x76)
 *   SCD41  5V  GND  SDA→A4  SCL→A5  (0x62)
 *   Logging Shield: SD D10-D13, RTC A4/A5
 *
 * Libraries: RTClib, Adafruit AHTX0, Adafruit BMP280,
 *            Sensirion I2C SCD4x, SD (built-in)
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <SensirionI2cScd4x.h>

#define SD_CS      10
#define LED        13
#define SAMPLE_MS  15000UL
#define MARK_MS   180000UL
#define ROW_LEN    64

RTC_DS1307        rtc;
Adafruit_AHTX0    aht;
Adafruit_BMP280   bmp;
SensirionI2cScd4x scd4x;

bool rtcOK=false,sdOK=false,ahtOK=false,bmpOK=false,scdOK=false;
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
  if(aht.begin()){ahtOK=true;Serial.println(F("AHT20 OK"));}
  else Serial.println(F("AHT20 missing"));
  if(bmp.begin(0x76)){bmpOK=true;Serial.println(F("BMP280 OK"));}
  else Serial.println(F("BMP280 missing"));
  scd4x.begin(Wire, 0x62);scd4x.stopPeriodicMeasurement();delay(500);
  if(scd4x.startPeriodicMeasurement()==0){scdOK=true;Serial.println(F("SCD41 OK"));}
  else Serial.println(F("SCD41 missing"));
  Serial.println(F("timestamp,scd_CO2,scd_T,scd_RH,aht_T,aht_RH,pres_hPa"));
  lastSample=lastMark=millis();
}

void loop(){
  unsigned long now=millis();
  if(now-lastSample>=SAMPLE_MS){
    lastSample=now;
    long ahtT=-1,ahtRh=-1,p=-1;
    if(ahtOK){sensors_event_t eh,et;aht.getEvent(&eh,&et);
      ahtT=(long)(et.temperature*10.0f+0.5f);ahtRh=(long)(eh.relative_humidity*10.0f+0.5f);}
    if(bmpOK) p=(long)(bmp.readPressure()/10.0f+0.5f);
    long sCO2=-1,sST=-1,sSRh=-1;
    if(scdOK){bool rdy=false;uint16_t co2=0;float st=0,srh=0;
      scd4x.getDataReadyStatus(rdy);
      if(rdy&&scd4x.readMeasurement(co2,st,srh)==0){
        sCO2=(long)co2;sST=(long)(st*10.0f+0.5f);sSRh=(long)(srh*10.0f+0.5f);}}
    char row[ROW_LEN];uint8_t pos=0;
    DateTime dt=rtcOK?rtc.now():DateTime(2000,1,1,0,0,0);
    pos=appendTS(row,pos,dt);
    row[pos++]=',';pos=appendLong(row,pos,sCO2);
    row[pos++]=',';pos=appendFP1(row,pos,sST);
    row[pos++]=',';pos=appendFP1(row,pos,sSRh);
    row[pos++]=',';pos=appendFP1(row,pos,ahtT);
    row[pos++]=',';pos=appendFP1(row,pos,ahtRh);
    row[pos++]=',';pos=appendFP1(row,pos,p);
    row[pos]='\0';
    Serial.println(row);
    if(sdOK){
      char fname[13];buildFilename(fname,dt);
      bool isNew=!SD.exists(fname);
      File f=SD.open(fname,FILE_WRITE);
      if(f){if(isNew)f.println(F("timestamp,scd_CO2,scd_T,scd_RH,aht_T,aht_RH,pres_hPa"));
        f.println(row);f.close();
        digitalWrite(LED,HIGH);delay(80);digitalWrite(LED,LOW);}
      else Serial.println(F("SD open fail"));}
    if(now-lastMark>=MARK_MS){lastMark=now;blinkFast(2);}
  }
}
