#include <NeoSWSerial.h>


NeoSWSerial loraS(3,2);
void setup() 
{
loraS.begin(9600);
Serial.begin(9600);
}

void loop() {

    
    if(digitalRead(4) != 0)                         
    {   
      loraS.print("\n Teste");
    }
}
