// Função InicializaAcel
// Incializa as comunicações com o acelerômetro e calibra os offsets
// O Acelarometro têm uma escala:
// 0 = +-2G, 1 = +- 4G, 2  2 = +- 8g , 3 = +- 16g
void inicializaAcel (void)
{
  
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

String inicializaSD(void)
{
  String nome = "Teste_0";                                  //Nome base do ficheiro
  int numero = 0;
  File ficheiro;
  
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
    return nome; 
       
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
  return nome;
  
}
//Função inicializa_altímetro
//Inicializa a comunicação com altimetro e configura-o
void inicializa_altimetro()
{
  altimetro.begin(); // Liga o Altímetro

  altimetro.setOversampleRate(7); 
  altimetro.enableEventFlags();
}

//Função GuardaSD
//Guarda os dados para o cartão SD
void guardaSD(String a_guardar)
{
  File ficheiro;
  
  if(digitalRead(cardDetect))
  {
    ficheiro = SD.open(nome_ficheiro, FILE_WRITE);
    if(ficheiro)
    {
      ficheiro.println(a_guardar);
      ficheiro.close();
    }
  }
  
}

void get_altitude(float * Altitude)
{
  *Altitude = altimetro.readAltitude();
  return;
}


//Calcula o modulo da aceleração 
//return: take_off, instantes seguidos em que o modulo dos paramentros do acelerometro é maior que 3    
int accelModule(float AcXf, float AcYf, float AcZf) { //pode-se usar apenas um dos eixos do acelerometro
  if (sqrt(pow(AcXf,2)+pow(AcYf,2)+pow(AcZf,2)) > 3) {
    take_off++;
  }
  else {
    take_off=0; 
  } 
  return take_off;
}

//Filtro de Kalman
//return: altitude;
float filtro(float acel_vert, float mfr, float m, float* P, float v) {
  
    float aux1,aux2,aux3,densidade;
    float Ve,L,M,p0,R0,temperatura;
    float g=9.81, A=0, C=1;
    float Fa,Fm;
    float acel_corrigida,a_prediction, P_prediction,residual;
    float Q=1, R=2500;//corrigir o R em funcao do ruido (quadrado do desvio padrao = cov)

    Ve=1709;
    L = 0.0065;
    M = 0.0289654;
    p0 = 101325;
    R0 = 8.31447;
    temperatura = 25 + 373.15;

    aux1 = (p0 * M) / (R0 * (temperatura));
    aux2 = 1 - ((L * alt) / (temperatura));
    aux3 = ((g * M) / (R0 *.L)) - 1;
    densidade = aux1 * (pow(aux2,aux3)); //E' suposto ser aux2^aux3;
  
    Fm = mfr * Ve; //*sin(pitch) ou assim
    Fa = 0.43*densidade*(*v)*(*v)* 0.017671*0.5; //0.43 era o coeficiente de drag do blimunda, mudar para Aurora III
    if (v<0)
        Fa=-Fa;
  
    a_prediction = Fm/m - Fa/m - g;
    P_prediction = A*(*P)*A + Q;
    residual = acel_vert - C*a_prediction;
    K = (P_prediction * C)/(C * P_prediction * C + R);
    acel_corrigida = a_prediction + K*residual;
    *P = (1 - K*C)*P_prediction; 
  
   return acel_corrigida;
  
}
