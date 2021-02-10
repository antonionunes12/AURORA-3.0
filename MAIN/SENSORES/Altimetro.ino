
#include <Wire.h>
#include "SparkFunMPL3115A2.h"

MPL3115A2 altimetro;

void setup()
{
  Wire.begin();        
  Serial.begin(9600);  

  altimetro.begin(); //Liga o Altimetro

  altimetro.setOversampleRate(7);   
  altimetro.enableEventFlags();
}

void loop()
{
  float altitude = altimetro.readAltitude(); // Lê o altimetro
  Serial.print("Altitude(m):");
  Serial.print(altitude, 2);

}
