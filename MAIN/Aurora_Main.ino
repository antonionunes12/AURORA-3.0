#include "Aurora.h"

//Inicializção variáveis ejeção (Global variables)
unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 26800; //alterar periodo
unsigned long starttime=0;
int ejecao = 0;
int counter = 0;
int counter_acel = 0;
int take_off = 0;
String nome_ficheiro;
//Editei a partir daqui
float m=12,m0=8;
float t = millis();
float v=0, h=0;
float delta;
float mfr;
//Acabei aqui ( agr vou para o loop() )

//Pin Comunicação
const int sendPin = 10; 



void setup()
{
  Wire.begin();                                 //Começa a comunicação i2c
  //Wire.setClock(400000);
  inicializaAcel();
  inicializaLora_gps();
  nome_ficheiro = inicializaSD();
}


void loop() 
{
  float acel_vert,acel_corrigida,P=0;

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
  //acel_vert = (por exemplo) (-AcXf+sin(pitch))*sin(pitch);
  acel_corrigida = filtro(acel_vert, mfr, m, &P, v, h);
  
  v=v+acel_corrigida*delta;
  h=h+v*delta + acel_corrigida*delta*delta;

 //Acabei aqui

  
  leitura = String(AcXf, 5 ) + " " + String(AcYf, 5) + " " + String(AcZf,5) + " " + String(lati, 5) + " " + String(longi, 5) + " " + String(altitudeGps, 5) + " " + String(altitude_medida, 3) ; //Compila os dados numa só string
  
  guardaSD(leitura);                                       //Guarda os dados no cartão SD
  enviaLora(leitura);                                      // Envia os dados para o Lora
  
  


  /*Verifica se a duração do tempo de voo do rocket é superior ao período estimado*/
  int flagMod = accelModule(AcXf, AcYf, AcZf);

  if(flagMod == 10  && starttime==0 && counter==0) {
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
