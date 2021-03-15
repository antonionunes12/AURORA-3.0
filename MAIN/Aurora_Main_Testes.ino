#include "Aurora_arranjar.h"

//Valores Ejecção

unsigned long startMillis; 
unsigned long starttime=0;
byte ejecao = 0;
byte counter = 0;
byte counter_acel = 0;

//Valores Cartão SD

String nome_ficheiro;

//Valores BNO

double AcXf, AcYf, AcZf,rot_x,rot_y,rot_z;
sensors_event_t orientationData , accelerometerData;

//GPS

double latitude;
double longitude;

//Altimetro

float altitude_medida;
float m=12,m0=8;
float t = millis();
float v=0, h=0, P = 0;
float delta;
float mfr;
float acel_vert;
float acel_corrigida;

String leitura;
char receivedChars[10];
boolean newData = false; 

void setup()
{
 
 Wire.begin();
 inicializa_LORA();
 inicializa_BNO();
 inicializa_GPS();
 nome_ficheiro = inicializa_SD();
 inicializa_altimetro();
 
 delay(1000);
 while(1)
 {
  delay(200);
  recvWithEndMarker();
  if(newData == true)
  {
    if(strcmp(receivedChars, "Launch") == 0)
    {
      break;
    }
  }
  faz_leitura_BNO( &accelerometerData, &orientationData);
  retira_dados_acc(&accelerometerData, &AcXf,&AcYf, &AcZf);
  retira_dados_GPS(&latitude, &longitude); 
  
  leitura = String(AcXf, 5 ) + " " + String(AcYf, 5) + " " + String(AcZf,5);
  envia_LORA(leitura);
  guardaSD(&AcXf, &AcYf,&AcZf);
  envia_LORA(" ");
  envia_LORA(String(latitude, 5));
  envia_LORA(" ");
  envia_LORA(String(longitude, 5));
  envia_LORA("\n");
  
}
 while(1)
 {
  envia_LORA(F("Estou no Launch MODE\n"));
  delay(400);
 }
}



void loop() 
{
  
}
