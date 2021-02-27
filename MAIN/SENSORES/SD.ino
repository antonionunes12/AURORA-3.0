#include <SPI.h>
#include <SD.h>

//Defines SD
#define cardDetect 7
#define chipSelect 6

String nome_ficheiro;

String inicializaSD(void)
{
  String nome = "Teste_0.txt" ;//Nome base do ficheiro
  int numero = 0;
  File ficheiro;
  
  pinMode(cardDetect, INPUT);                               //Bloqueia o Pino CardDetect do cartão SD no modo Input.
  
  Serial.println("A inicializar SD");  
  while(!digitalRead(cardDetect))                           //Verifica se o  cartão está inserido no leitor.
  {
    Serial.println("ERRO - Cartão SD NÃO detetado");
    delay(1000);
  }
  if(!SD.begin(chipSelect))
  {
            
    Serial.println("Incialização Falhada! - A tentar outra vez");
    inicializaSD();
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
      Serial.println("Erro a abrir o ficheiro");
    }
   }
    
  Serial.print("SD inicializado com Sucesso!");
  delay(1000);
  
}

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

void setup() {
Serial.begin(9600);
nome_ficheiro = inicializaSD();
guardaSD("Esta função também funciona Horayy!");
}

void loop() {
Serial.println(nome_ficheiro); 

}
