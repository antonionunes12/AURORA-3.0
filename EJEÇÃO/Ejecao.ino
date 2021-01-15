#include <Wire.h>
#include <math.h>

//Calcula o modulo da aceleração 
//return: 1 se modulo maior que 3, 0 se modulo inferior a 3     
int accelModule(float AcXf, float AcYf, float AcZf) {
  if (sqrt(pow(AcXf,2)+pow(AcYf,2)+pow(AcZf,2)) > 3) {
    return 1; }
  else {
    return 0; } 
}


//Inicializção variáveis ejeção (Global variables)
  unsigned long startMillis; 
  unsigned long currentMillis;
  const unsigned long period = 26800; //alterar periodo
  unsigned long starttime=0;
  int ejecao = 0;
  int counter = 0;
  int counter_acel = 0;
/*//caso seja usado o altimetro
  unsigned long last_altitude=9999.0000000;
  unsigned long current_altitude=0;
  int counter_alt = 0;
  long P0=101325; //Sea Level Pressure, em Pa
*/
  //Pin Comunicação
  const int sendPin = 9; 


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
  getAcel (&AcXf, &AcYf, &AcZf, &GyXf, &GyYf, &GyZf);
  //Valores de altitude longitude e latitude 
  gpsReadVals(&lati, &longi, &altitudeGps);
  //Eliminar ruido, valores depois de serem filtrados pela forma canonica de kalman

  //valores guardados no cartao SD
  guardaSD (AcXf, AcYf, AcZf, GyXf, GyYf, GyZf, lati, longi, altitudeGps); //alterar consoante o que for utilizado (tempBMP, pressBMP, altBMP; alt_pressao)


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


 //Caso seja usado um altímetro, retirar valor da pressão e comparar alturas
/*
  current_altitude = (1-pow((alt_pressao/P0),(1/5.25588)))*(288.15/0.0065);

  if (current_altitude < last_altitude && ejecao == 0) {
    counter_alt++;
  }
  else {
      counter_alt = 0; 
  }
  if (counter1 == 15) 
    ejecao = 1;

  last_altitude=current_altitude;
*/

  //condições de ejeção verificadas
  if (ejecao == 1)
  {
    digitalWrite(sendPin, HIGH);   
    ejecao = -1;
  }
}

