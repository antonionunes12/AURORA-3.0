#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(3,4); /*TXD, RXD*/
/*---------------------------------------------------------------------------------*/
/* Function to get GPS values ---> Para teste final*/ 
void gpsReadVals (double *latitude_val, double *longitude_val, double *altitude_val)
{
  unsigned long start_time;
  gpsSerial.listen();
  start_time = millis();
  
  while(millis()-start_time <1000) /*delay of 1ms*/
    if(gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());

      if(gps.location.isUpdated())
      {
        start_time = millis();
        *latitude_val = gps.location.lat();
        *longitute_val = gps.location.lng();
        *altitude_val = gps.altitude.meters();
        return;
      }
    }
  return;
}
/*-----------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------*/
void setup() 
{
  Serial.begin(9600); /*Baud Rate*/
  gpsSerial.begin(9600); 
}

/* Now, for GPS test only*/
void loop() 
{
  double latitude_val, longitude_val, altitude_val;
  while(gpsSerial.available() > 0)
  {
    gps.encode(gpsSerial.read())
    {
      if(gps.location.isUpdated())
      {
        latitude_val = gps.location.lat();
        Serial.print("Latitude=");
        Serial.println(latitude_val,6);
        longitude_val = gps.location.lng();
        Serial.print("Longitude=");
        Serial.println(longitude_val, 6);
        altitude_val = gps.altitude.meters();
        Serial.print("Altitude=");
        Serial.println(altitude_val,6);
      }
    }
  }
}
/*----------------------------------------------------------------------------------*/
