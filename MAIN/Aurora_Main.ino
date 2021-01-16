#include <Wire.h>
#include <MPU6050.h>
#include <math.h>
#include <

//Inicializção variáveis ejeção (Global variables)
  unsigned long startMillis; 
  unsigned long currentMillis;
  const unsigned long period = 26800; //alterar periodo
  unsigned long starttime=0;
  int ejecao = 0;
  int counter = 0;
  int counter_acel = 0;
  float AcXf,AcYf,AcZf;
  
  //Pin Comunicação
  const int sendPin = 9; 


void getAcel (&AcXf, &AcYf, &AcZf)
{
  
  *AcXf = mpu.getAccelerationX() / 1024.0;
  *AcYf = mpu.getAccelerationY() / 1024.0;
  *AcZf = mpu.getAccelerationZ() / 1024.0;  

}
void inicializaAcel (void)
{
  
 Wire.begin();
 Wire.setClock(400000);
 mpu.initialize();
 mpu.setFullScaleAccelRange(3);
 mpu.setXAccelOffset(769);
 mpu.setYAccelOffset(1115);
 mpu.setZAccelOffset(-1625);
  
}

void inicializaLora(void)
{
 loraS.begin(9600);
}

void enviaLora(String a_enviar)
{
  if(digitalRead(4) != 0)                         
   {   
      loraS.print(a_enviar);
   }
}

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


//Calcula o modulo da aceleração 
//return: 1 se modulo maior que 1, 0 se modulo inferior a 1     
int accelModule(float AcXf, float AcYf, float AcZf) {
  if (sqrt(pow(AcXf,2)+pow(AcYf,2)+pow(AcZf,2)) > 1) {
    return 1; }
  else {
    return 0; } 
}


void loop() 
{

  //Valores Acel
  float AcXf = 0.00;
  float AcYf = 0.00;
  float AcZf = 0.00;
  float modAc = 0.00;
  //Valores Girosc
  float GyXf = 0.00;
  float GyYf = 0.00;
  float GyZf = 0.00;
  //GPS
  double lati = 0.0000000;
  double longi = 0.0000000;
  double altitudeGps = 0.0000000;
  /*//Valores Altimetro
  //float alt_pressao;
  //Valores BMP
  //float tempBMP = 0.00;        
  //float pressBMP = 0.00;
  //float altBMP = 0.00; */

  //Funcoes do main
  //Obter aceleracoes
  getAcel (&AcXf, &AcYf, &AcZf);
  //Valores de altitude longitude e latitude 
  gpsReadVals(&lati, &longi, &altitudeGps);
  //Eliminar ruido, valores depois de serem filtrados pela forma canonica de kalman

  //valores guardados no cartao SD
  guardaSD (AcXf, AcYf, AcZf, lati, longi, altitudeGps); //alterar consoante o que for utilizado (tempBMP, pressBMP, altBMP; alt_pressao)


  /*Verifica se a duração do tempo de voo do rocket é superior ao período estimado*/
  int flagMod = accelModule(AcXf, AcYf, AcZf);

  if(flagMod == 1  && starttime==0 && counter==0) {
    starttime=1;
    counter=1; 
  }
  if(starttime==1) {
    startMillis = millis();  //initial start time
    starttime = 0; 
  } 
  currentMillis = millis();

  //Se ultrapassou o tempo
  if ((currentMillis - startMillis) >= period){
    ejecao=1;
    startMillis = currentMillis; }

  

  //Caso o rocket não atinja a altitude estimada, verificar se está no processo de descida pelos valores do acelerómetro (10 instantes de amostragem)
  if(AcXf<0 && ejecao == 0) { //a escolha do eixo da condiçao depende de como o acelerometro estiver no circuito
    counter_acel++;
  }
  else {
    counter_acel=0;
  }
  if(ejecao == 0 && counter_acel==10) {
    ejecao=1;
  }


  //condições de ejeção verificadas
  if (ejecao == 1)
  {
    digitalWrite(sendPin, HIGH);   
    ejecao = -1;
  }
}
