#include <Wire.h>
#include <MPU6050.h>
#include <math.h>
#include <NeoSWSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

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


NeoSWSerial loraS(loraRXD,loraTXD);  // RXD, TXD  Ao contrário nos dispositivos
NeoSWSerial gpsSerial(gpsRXD,gpsTXD);
MPU6050 mpu;
TinyGPSPlus gps;

//Inicializção variáveis ejeção (Global variables)
unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 26800; //alterar periodo
unsigned long starttime=0;
int ejecao = 0;
int counter = 0;
int counter_acel = 0;

//Endereço Ficheiro
File ficheiro;

//Pin Comunicação
const int sendPin = 10; 

// Função InicializaAcel
// Incializa as comunicações com o acelerômetro e calibra os offsets
// O Acelarometro têm uma escala:
// 0 = +-2G, 1 = +- 4G, 2  2 = +- 8g , 3 = +- 16g
void inicializaAcel (void)
{
  
 Wire.begin();  
 Wire.setClock(400000);
 mpu.initialize();
 mpu.setFullScaleAccelRange(3);                       // Ajusta a escala do acelarometro para 3 
 mpu.setXAccelOffset(769);                            // Ajusta os offsets específicos ao nosso MPU
 mpu.setYAccelOffset(1115);
 mpu.setZAccelOffset(-1625);
  
}

//Função getAcel
//Obtem-se os dados mais recentes do acelerômetro e guarda nas variáveis respetivas.
//Para obter os dados em g's divide-se por uma sensibilidade dependendo da escala. *ver incializaACel
//Sensibilidades:
//0 ±2g 16384 LSB/g
//1 ±4g 8192 LSB/g
//2 ±8g 4096 LSB/g
//3 ±16g 2048 LSB/g

 void getAcel (float *AcXf, float *AcYf, float *AcZf )
{
   
  *AcXf = mpu.getAccelerationX() / 2048.0;
  *AcYf = mpu.getAccelerationY() / 2048.0;
  *AcZf = mpu.getAccelerationZ() / 2048.0;
    
}
//Função inicializaLora_gp 
//Inicia a comunicção entre o arduino, o Lora e o gps
//Envia uma mensagem de Teste

void inicializaLora_gps(void)
{
  loraS.begin(9600);
  pinMode(loraaux, INPUT);
  gpsSerial.begin(9600);
  enviaLora("Comunicações OK");
  delay(1000);
}

//Função inicializaLora_gp 
//Ordena o Lora a enviar o que está na string a_enviar
void enviaLora(String a_enviar)
{
  if(digitalRead(loraaux) != 0)      //define (pin 4)                   
   {   
      loraS.print(a_enviar);
   }
}
//Função gpsReadVals
//Obtem as coordenadas atuais e guarda-as nas variáveis respetivas

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
        *longitude_val = gps.location.lng();
        *altitude_val = gps.altitude.meters();
        return;
      }
    }
  return;
}

//Função inicializaSD
//Inicializa a comunicação com o módulo leitor de cartão SD e certifica-se que consegue escrever dados para o cartão.

void inicializaSD(void)
{
  String nome = "Teste_0";                                  //Nome base do ficheiro
  int numero = 0;
  
  pinMode(cardDetect, INPUT);                               //Bloqueia o Pino CardDetect do cartão SD no modo Input.
  
  enviaLora("A inicializar SD");  
  while(!digitalRead(cardDetect))                           //Verifica se o  cartão está inserido no leitor.
  {
    enviaLora("ERRO - Cartão SD NÃO detetado");
    delay(1000);
  }
  if(!SD.begin(chipSelect))
  {
            
    enviaLora("Incialização Falhada! - A tentar outra vez");
    inicializaSD();
    return; 
       
   }
   
   while (SD.exists(nome))
   {
    nome = "Teste_";
    ++numero;
    nome = String(nome + numero); 
   }
   ficheiro = SD.open(nome, FILE_WRITE );
   if(ficheiro)
   {
     ficheiro.println("----------------AURORA 3.0 - Inicio de Dados-----------------\n");
     ficheiro.close();
   }
   else
   {
    while(1)
    {
    enviaLora("Erro a abrir o ficheiro");
    }
   }
    
  enviaLora("SD inicializado com Sucesso!");
  delay(1000);
  
}


//Calcula o modulo da aceleração 
//return: 1 se modulo maior que 1, 0 se modulo inferior a 1     
int accelModule(float AcXf, float AcYf, float AcZf) {
  if (sqrt(pow(AcXf,2)+pow(AcYf,2)+pow(AcZf,2)) > 1) {
    return 1; }
  else {
    return 0; } 
}


void setup()
{
  inicializaAcel();
  inicializaLora_gps();
  
}


void loop() 
{

  //Valores Acel
  float AcXf = 0.00;
  float AcYf = 0.00;
  float AcZf = 0.00;
  float modAc = 0.00;
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
  //guardaSD (AcXf, AcYf, AcZf, lati, longi, altitudeGps); //alterar consoante o que for utilizado (tempBMP, pressBMP, altBMP; alt_pressao)


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
