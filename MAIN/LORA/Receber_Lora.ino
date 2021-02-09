#include <NeoSWSerial.h>

NeoSWSerial loraS(5,4);

void setup() 
{
loraS.setTimeout(50);
loraS.begin(9600);
Serial.begin(9600);


}
void loop() {

 String leitura;

  while(loraS.available())
 {
  leitura = loraS.readString();
  Serial.print(leitura);
 }

}
