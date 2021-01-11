#include <SoftwareSerial.h>

SoftwareSerial loraS(5,4);
void setup() {
  // put your setup code here, to run once:
loraS.begin(9600);
}

void loop() {

  loraS.write("Ola");
  // put your main code here, to run repeatedly:
delay(3000);
}
