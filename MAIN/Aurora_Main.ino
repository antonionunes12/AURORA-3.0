#include "Aurora.h"

//Inicializção variáveis ejeção (Global variables)
unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 26800; //alterar periodo
unsigned long starttime=0;
int ejecao = 0;
int counter = 0;
int counter_acel = 0;
String nome_ficheiro;
//Valores Acel
float AcXf = 0.00;
float AcYf = 0.00;
float AcZf = 0.00;
//GPS
double lati = 0.0000000;
double longi = 0.0000000;
double altitudeGps = 0.0000000;
//Altimetro
float altitude_medida;
String leitura;
float m=12,m0=8;
float t = millis();
float v=0, h=0, P = 0;
float delta;
float mfr;
float acel_vert;
float acel_corrigida;


//Pin Comunicação
const int sendPin = 10; 



void setup()
{
  Wire.begin();                                 //Começa a comunicação i2c
  //Wire.setClock(400000);
  inicializaAcel();
  inicializaLora_gps();
  nome_ficheiro = inicializaSD();
  while(1)                                        //Ciclo de StandBy
  {
    if(loraS.available())
    {
      if(loraS.readString() == "Launch Mode")
      {
        guardaSD("Entrei em Launch Mode!");
        enviaLora("Entrei em Launch Mode!");
        break;
      }
    }
  getAcel (&AcXf, &AcYf, &AcZf);
  //Valores de altitude longitude e latitude 
  gpsReadVals(&lati, &longi, &altitudeGps);
  //Obtem o valor da altitude
  get_altitude(&altitude_medida);  
  
  leitura = String(AcXf, 5 ) + " " + String(AcYf, 5) + " " + String(AcZf,5) + " " + String(lati, 5) + " " + String(longi, 5) + " " + String(altitudeGps, 5) + " " + String(altitude_medida, 3) + " " + String(0);
  enviaLora(leitura);  // Envia os dados para o Lora    
  }
  startMillis = millis();
  while(1) // Launch Mode!
  {    
  getAcel (&AcXf, &AcYf, &AcZf);
  //Valores de altitude longitude e latitude 
 
  
  if((millis() - startMillis) > Minimo_temp)
  {
    guardaSD("Entrei no Flight Mode!");
    enviaLora("Entrei no Flight Mode!");
    starttime=1;
    counter=1; 
    break;
    
  }
  else if (accelModule( AcXf, AcYf, AcZf) == false)
  {
    startMillis = millis();
  }
  
   gpsReadVals(&lati, &longi, &altitudeGps);
  //Obtem o valor da altitude
  get_altitude(&altitude_medida);
  
  leitura = String(AcXf, 5 ) + " " + String(AcYf, 5) + " " + String(AcZf,5) + " " + String(lati, 5) + " " + String(longi, 5) + " " + String(altitudeGps, 5) + " " + String(altitude_medida, 3) + " " + String(0);
  enviaLora(leitura);  // Envia os dados para o Lora   
 
 }
}
void loop() 
{

  //Funcoes do main
  //Obter aceleracoes
  getAcel (&AcXf, &AcYf, &AcZf);
  //Valores de altitude longitude e latitude 
  gpsReadVals(&lati, &longi, &altitudeGps);
  //Obtem o valor da altitude
  get_altitude(&altitude_medida);
  //Eliminar ruido, valores depois de serem filtrados pela forma canonica de kalman
  
  
  //Aqui estou eu a editar
  delta = millis() - t;
  t=millis();
  
  if (m>m0){    //Se ainda houver combustivel (m > m0), a massa do rocket vai diminuindo
                //'a medida em que se vai queimando o combustivel 
            
      mfr=0.5; //Corrigir este valor!
      m=m-mfr*delta;
  }
  else{       //Se ja nao houver combustivel nao ha mass flow rate e a forca do motor sera' 0
      mfr=0;
      }
      
  acel_vert = (-AcXf+sin((PI/2))*sin(PI/2));
  
  acel_corrigida = filtro(acel_vert, mfr, m, &P, v,h);
  
  v=v+acel_corrigida*delta;
  h=h+v*delta + acel_corrigida*delta*delta;
    
  /*Verifica se a duração do tempo de voo do rocket é superior ao período estimado*//*
  if(starttime==1) {
    startMillis = millis();  //initial start time
    starttime = 0; 
  } */
  //Se ultrapassou o tempo
  if ( ejecao == -1)
  {
      if ((millis() - startMillis) >= period && ejecao == 0){
    ejecao=1;
    iniciar_ejecao(); 
    }
  
  //Caso o rocket não atinja a altitude estimada, verificar se está no processo de descida pelos valores do acelerómetro (10 instantes de amostragem)
  if(v<=0 && ejecao == 0 && ( millis()- startMillis) > tempo_seguranca) { //a escolha do eixo da condiçao depende de como o acelerometro estiver no circuito
    counter_acel++;
  }
  else {
   
    counter_acel=0;
  }
  if(ejecao == 0 && counter_acel== numero_amostragens_ejecao) {
    ejecao = 1;
    iniciar_ejecao();
  }

  leitura = String(AcXf, 5 ) + " " + String(AcYf, 5) + " " + String(AcZf,5) + " " + String(lati, 5) + " " + String(longi, 5) + " " + String(altitudeGps, 5) + " " + String(altitude_medida, 3)+ " "+ String(v,3) ; //Compila os dados numa só string
  
  guardaSD(leitura);                                       //Guarda os dados no cartão SD
  enviaLora(leitura);                                      // Envia os dados para o Lora
}
  }
