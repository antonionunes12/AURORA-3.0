#include <Wire.h>
#include <MPU6050.h>
#include <math.h>
#include <NeoSWSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include "SparkFunMPL3115A2.h"
//Defines LORA

#define loraRXD 3
#define loraTXD 2
#define loraaux 3

//Defines GPS

#define gpsRXD 8
#define gpsTXD 9

//Defines SD
#define cardDetect 7
#define chipSelect 6

#define Minimo_acc 2 //g's
#define Minimo_temp 300 // milisegundos 
#define numero_amostragens_ejecao 10 //Número de Amostragens
#define tempo_seguranca 10000 //milisegundos

NeoSWSerial loraS(loraRXD,loraTXD);  // RXD, TXD  Ao contrário nos dispositivos
NeoSWSerial gpsSerial(gpsRXD,gpsTXD);
MPU6050 mpu;
TinyGPSPlus gps;
MPL3115A2 altimetro;
