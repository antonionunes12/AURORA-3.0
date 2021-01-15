

// Biblioteca para a comunicacao I2C
#include<Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
float AcX,AcY,AcZ;

void setup()
{
  
 Serial.begin(9600);
 Wire.begin();
 Wire.setClock(400000);
 mpu.initialize();
 mpu.setFullScaleAccelRange(3);
 mpu.setXAccelOffset(769);
 mpu.setYAccelOffset(1115);
 mpu.setZAccelOffset(-1625);
  
  //Inicializa o MPU-6050 em modo normal
 
}

void loop()
{

AcX = mpu.getAccelerationX() / 1024.0;
AcY = mpu.getAccelerationY() / 1024.0;
AcZ = mpu.getAccelerationZ() / 1024.0;

Serial.println(AcX);
Serial.println(AcY);
Serial.println(AcZ);
delay(1000);
  
}
