/*NOTA: 
O código não foi testado e invoca funções de obtenção de valores dos sensores
Ainda nao pesquisei que biblitecas é que é suposto incluir
Só uma tentativa para começar, podes alterar o que quiseres, estás à vontade
3 formas diferentes para termos o maior numero de dados possiveis, ainda so me dediquei ao metodo 1 e não sei se há mais alguma forma pra criar redundância
E espero que esteja mais ou menos bem feito eheheeh
*/


#include <math.h>

//Inicialização de variáveis
//Para que a carga de ejeção possa ser ativada, é preciso ter o máximo de redundância possíveis para que não tomemos decisões precipitadas
//Testar por pelo menos 3 métodos
int ativar_ejecao=0;


//Para o método 1
int ejecao1=0;
int ejecao1_GPS=0;
//Sea Level Pressure, em Pa, usada para calcular a altura a partir dos dados do sensor BMP (Método1)
long P0=101325;
//Altitude inicializada com um valor muito superior à altitude atual para que a condição de Método1 nao seja confirmada logo no primeiro ciclo
unsigned long last_altitude=9999.0000000;
unsigned long last_altitude_GPS=9999.0000000;
unsigned long current_altitude=0;
//Contadores para o Método 1, usado para se saber em quantos ciclos de amostragem seguidos a altura anterior é superior à altura atual
int counter1=0;
int counter1_GPS=0;


//Para o método 2
int ejecao2=0; 


//Para o método 3, nada feito também
int ejecao3=0; 


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
  //Valores BMP
  float tempBMP = 0.00;
  float pressBMP = 0.00;
  float altBMP = 0.00;
  //GPS
  double lati = 0.0000000;
  double longi = 0.0000000;
  double altitudeGps = 0.0000000;

//Funcoes do main
//Obter aceleracoes
getAcel (&AcXf, &AcYf, &AcZf, &GyXf, &GyYf, &GyZf);
//obter os valores do sensor bmp (Temperatura, PregpsSerialao, Altitude)
getValsBMP(&tempBMP, &pressBMP, &altBMP);
//Valores de altitude longitude e latitude 
gpsReadVals(&lati, &longi, &altitudeGps);
//Eliminar ruido, valores depois de serem filtrados pela forma canonica de kalman
//valores guardados no cartao SD
guardaSD (AcXf, AcYf, AcZf, GyXf, GyYf, GyZf, tempBMP, pressBMP, altBMP, lati, longi, altitudeGps);




/*Método 1 - obter altitude através dos valores retirados do sensor BPM (temperatura e pressao). 
Se a altitude atual for menor do que a altitude no instante de amostragem anterior e se tal se verificar durante pelo menos 3(?) instantes de amostragens seguidos
ejecao1=1 */
//Hypsometric Formula
//é preciso verificar se a temperatura no sensor é dada em graus celsius
current_altitude = ((pow((P0/pressBMP),(1/5.257) - 1)*(tempBMP + 273.15))/0.0065); //será que faz com que haja muitas incertezas se forem feitas estas contas??

if (current_altitude < last_altitude)
	counter1++;
else {
	if  (ejecao1 == 0)
		counter1 = 0; //caso a relação nao se verifique, o contador é reinicializado, visto que é necessário que esta seja verdadeira em ciclos consecutivos
}

if (counter1 >= 3) //3 instantes de amostragem seguidos, talvez deviam ser mais(?)
	ejecao1 = 1;

last_altitude=current_altitude;


//O GPS também tem o outpup da altura, dupla verificação por este método?
if (altitudeGps < last_altitude_GPS)
	counter1_GPS++;
else {
	if  (ejecao1_GPS == 0)
		counter1_GPS = 0; //caso a condicao nao se verifique, o contador é reinicializado, visto que é necessário que esta seja verdadeira em ciclo consecutivos
}
if (counter1_GPS >= 3) //3 instantes de amostragem seguidos, talvez deviam ser mais(?)
	ejecao1_GPS = 1;

last_altitude_GPS = altitudeGps;



/*Método 2 - calcular o tempo previsto de subida do rocket. É inicializado um cronómetro assim que o processo de subida comece.
Se o periodo de tempo atual é maior de que os previstos pelos cálculos, entao
ejecao2=1
*/


 
/*Método 3 - tirar conclusoes atraves dos valores retirados do acelerometro.
ejecao3=1
*/


//A ejecao pode ser ativada se as 3/4 condicoes suprarreferidas se verificarem
if ((ejecao1 == 1) && (ejecao1_GPS == 1) && (ejecao2 == 1) && (ejecao3 == 1))
	ativar_ejecao=1;

}
