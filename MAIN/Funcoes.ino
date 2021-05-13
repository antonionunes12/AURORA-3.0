

/*---------------------------------------------------------------------------
                              Inicializa BNO
                              
Inicia a comunicação.
Verifica se a conecção foi bem sucedida.

---------------------------------------------------------------------------*/

void incializa_BNO(void)
{
  if (!bno.begin())
  {
    // Problema a detectar o BNO055... Verificar as conecções
    Serial.print("Ooops, BNO055 nao detectado ... Verificar as coneccoes ou o endereco I2C!");
    while (1);
  }
  
  delay(1000);
}



/*---------------------------------------------------------------------------
                         Retira Dados da Aceleração
                         
Obtem os valores da aceleração para os eixos X, Y e Z.
Guarda os valoes.

---------------------------------------------------------------------------*/

void retira_dados_acc(sensors_event_t* event, double* a_x, double* a_y, double *a_z) 
{
    *a_x = event->acceleration.x;
    *a_y= event->acceleration.y;
    *a_z= event->acceleration.z;
}



/*---------------------------------------------------------------------------
                         Retira Dados da Rotação
                         
Obtem os valores da rotação para os eixos X, Y e Z.
Guarda os valoes.

---------------------------------------------------------------------------*/

void retira_dados_rota(sensors_event_t* event, double* rot_x, double* rot_y, double *rot_z) 
{
    *rot_x = event->orientation.x;
    *rot_y = event->orientation.y;
    *rot_z = event->orientation.z;
}



/*---------------------------------------------------------------------------
                         Faz Leitura do BNO
                         
Le os valores da aceleração e da rotação obtidos pelo BNO.
Actualiza os valores.

---------------------------------------------------------------------------*/

void faz_leitura_BNO(sensors_event_t* accel_data, sensors_event_t* rot_data)
{
  
 bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
 bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  
}



/*---------------------------------------------------------------------------
                         Inicializa LORA
                         
Inicializa a comunicação entre o Arduino e o LORA.
Imprime uma mensagem de teste.

---------------------------------------------------------------------------*/

void inicializa_LORA(void)
{
  Serial.begin(9600);
  pinMode(lora_aux, INPUT);
  envia_LORA("Comunicações OK");
  delay(1000);
}



/*---------------------------------------------------------------------------
                           Envia LORA
                         
Ordena o LoRa a enviar a string: a_enviar.

---------------------------------------------------------------------------*/

void envia_LORA(String a_enviar)
{
  if(digitalRead(lora_aux) != 0)      //define (pin 4)                   
   {   
      Serial.println(a_enviar);
   }
}



/*---------------------------------------------------------------------------
                           Inicializa GPS
                         
Inicializa a comunicação entre o Arduino e o GPS.
Inicializa o protocolo do GPS (GLL).

---------------------------------------------------------------------------*/

void inicializa_GPS (void)
{
  envia_LORA("Estou no GPS");
  DEV_Set_Baudrate(9600);
  
  L76X_Send_Command(SET_POS_FIX_100MS);
  delay(500);
  
  L76X_Send_Command(SET_NMEA_OUTPUT);
  delay(500);
  
  clearbuffer();
}



/*---------------------------------------------------------------------------
                            Retira Dados do GPS
                         
Retira os dados da latitude e da longitude obtidos pelo GPS.

---------------------------------------------------------------------------*/

void retira_dados_GPS(double *latitude, double *longitude)
{
  char data[150];
  char tratamento_lat[10] = "\0";
  char tratamento_long[10] = "\0";
  byte contador = 5;
  byte avanca = 0;
  byte copia_lat = 0;
  byte copia_long = 0;
  DEV_Uart_ReceiveString(data,150);
  
  for(;data[avanca] != '$'; ++avanca);  
  for(contador = 5; contador > 0; ++avanca)
  {
    if(data[avanca] == ',')
    {
      --contador;
      ++avanca;
    }
    switch(contador)
    {
      case 3:
      {
        tratamento_lat[copia_lat] = data[avanca];
        ++copia_lat;
        
        break;
      }
      case 1:
      {
        tratamento_long[copia_long] = data[avanca];
        ++copia_long;
        
        break;
      }
    }
  }
  
  *latitude = atof(tratamento_lat);
  *longitude = atof(tratamento_long);
  
  *latitude = *latitude/100.0;
  *longitude = *longitude/100.0;
  
  clearbuffer();
}




//Função inicializaSD
//Inicializa a comunicação com o módulo leitor de cartão SD e certifica-se que consegue escrever dados para o cartão.

String inicializa_SD(void)
{
  String nome = "Teste_0.txt";                                  //Nome base do ficheiro
  int numero = 0;
  File ficheiro;
  
  pinMode(cardDetect, INPUT);                               //Bloqueia o Pino CardDetect do cartão SD no modo Input.
  
  envia_LORA("A inicializar SD");  
  while(!digitalRead(cardDetect))                           //Verifica se o  cartão está inserido no leitor.
  {
    envia_LORA("ERRO - Cartão SD NÃO detetado");
    delay(1000);
  }
  if(!SD.begin(chipSelect))
  {
            
    envia_LORA("Incialização Falhada! - A tentar outra vez");
    inicializa_SD();
    return nome; 
       
   }
   
   while (SD.exists(nome))
   {
    nome = "Teste_";    
    ++numero;
    nome = String(nome + numero);
    nome = String (nome + ".txt");   
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
    envia_LORA("Erro a abrir o ficheiro");
    }
   }
    
  envia_LORA("SD inicializado com Sucesso!");
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
bool accelModule(float AcXf, float AcYf, float AcZf) { //pode-se usar apenas um dos eixos do acelerometro
  bool take_off;
  if (sqrt(pow(AcXf,2)+pow(AcYf,2)+pow(AcZf,2)) > Minimo_acc) {
    take_off = true;
  }
  else {
    take_off= false; 
  } 
  return take_off;
}

void iniciar_ejecao()
{
  digitalWrite(sendPin, HIGH);   //Arde o Nicrómio
  ejecao = -1;
}

//Filtro de Kalman
//return: altitude;
//Filtro de Kalman
//return: altitude;
float filtro(float acel_vert, float mfr, float m, float* P, float v, float alt) {
  
    float aux1,aux2,aux3,densidade;
    float Ve,L,M,p0,R0,temperatura;
    float g=9.81, A=0, C=1, K;
    float Fa,Fm;
    float acel_corrigida,a_prediction, P_prediction,residual;
    float Q=1, R=2500;//corrigir o R em funcao do ruido (quadrado do desvio padrao = cov)

    Ve=1709;
    L = 0.0065;
    M = 0.0289654;
    p0 = 101325;
    R0 = 8.31447;
    temperatura = 25 + 273.15;

    aux1 = (p0 * M) / (R0 * (temperatura));
    aux2 = 1 - ((L * alt) / (temperatura));
    aux3 = ((g * M) / (R0 *L)) - 1;
    densidade = aux1 * (pow(aux2,aux3)); //E' suposto ser aux2^aux3;
  
    Fm = mfr * Ve; //*sin(pitch) ou assim
    Fa = 0.43*densidade*(v)*(v)* 0.017671*0.5; //0.43 era o coeficiente de drag do blimunda, mudar para Aurora III
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
