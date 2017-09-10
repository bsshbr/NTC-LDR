// Include das Bibliotecas
#include <LiquidCrystal.h>

// Definindo o Botão
const int botao = 8;

// Definindo os pinos da interface (LCD)(RS, Enable, D4,D5,D6,D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Definindo as pinos dos Sensores
const float SensorDeLuz  = 0;   // Define o pino do sensor de luminosidade como o pino analogico 0
const float SensorDeTemp  = 1;  // Define o pino do sensor de temperatura como o pino analogico 1
float ValorSensorLuz = 0;       // Variavel auxiliar ao valor do sensor de luminosidade
float ValorSensorTemp = 0;      // Variavel auxiliar ao valor do sensor de temperatura

// Definindo os pinos dos LEDs
int LedVerde = 9;        //LED vermelho do sensor de luz no pino 9
int LedVermelho = 10;    // LED verde do sensor de luz no pino 10

// Definindo a temperatura para resistência nominal (25ºC)
const float TemperaturaNominal = 25;

// Definindo o valor do termistor a 25 ºC
const float TermistorNominal = 10000;

// Definindo a quantidade de amostras ultilizadas para a media da temperatura
const int NumeroDeAmostras = 5;

// Definindo o coeficiente beta do termistor
const float CoeficienteBeta = 3435;

// Definindo o valor do Resistor em Serie com o Sensor de Temperatura (10K)
const float Resistor = 10000;

// Definindo as amostras como um vetor de valores inteiros
int Amostras[NumeroDeAmostras];

// Definindo as Variaveis que auxiliarão na função printa
int i;
float MediaT;
float TemperaturaCelsius;
float LuminosidadePorcentagem;

// Inicialização da Interface
void setup() {
  // Definindo o número de colunas (16) e linhas (2) no LCD
  lcd.begin(16, 2);

  //Definindo os LEDS como saidas
  pinMode(LedVermelho, OUTPUT);
  pinMode(LedVerde, OUTPUT);

  // Definindo a taxa de comunicação em bits por segundo
  Serial.begin(9600);

  // Valor de tensão aplicada no pino AREF, usada como referência externa
  analogReference(EXTERNAL);

  // Transforma o valor do Termistor
  delay(10);

}

// Função Loop
void loop() {

  // Chama a função printa se o botão for pressionado
  if (digitalRead (botao) == HIGH)
  printa();
  delay(10);

  // Ler o valor atual do Sensor de Luz
  ValorSensorLuz = analogRead(SensorDeLuz);
  delay(10);
   
  // Colocando em Porcetagem
  ValorSensorLuz = (1023 - ValorSensorLuz);
  LuminosidadePorcentagem = (ValorSensorLuz / 1023) * 100;

  // Pega o valor do sensor de temperatura e armazena em amostras
  for (i = 0; i < NumeroDeAmostras; i++) {
    Amostras[i] = analogRead(SensorDeTemp);
    delay(10);
  }

  // Media de Todas as amostras de temperatura
  MediaT = 0;
  for (i = 0; i < NumeroDeAmostras; i++) {
    MediaT += Amostras[i];
  }
  MediaT /= NumeroDeAmostras;
  
  // Escreve a media na porta serial
  Serial.print("Media da Temperatura  ");
  Serial.println(MediaT);

  // Converte o valor para Resistência
  MediaT = 1023 / MediaT - 1;
  MediaT = Resistor / MediaT;

  // Escreve a resistência na porta serial
  Serial.print("Resistencia do Termistor: ");
  Serial.println(MediaT);

  // Calcula a Temperatura em Celsius
  TemperaturaCelsius = MediaT / TermistorNominal;               // (R/Ro)
  TemperaturaCelsius = log(TemperaturaCelsius);                 // ln(R/Ro)
  TemperaturaCelsius /= CoeficienteBeta;                        // 1/B * ln(R/Ro)
  TemperaturaCelsius += 1.0 / (TemperaturaNominal + 273.15);    // + (1/To)
  TemperaturaCelsius = 1.0 / TemperaturaCelsius;                // Inverte
  TemperaturaCelsius -= 273.15;                                 // Converte para Cº

  // Escreve a temperatura na porta serial
  Serial.print("Temperatura em Celsius ");
  Serial.print(TemperaturaCelsius);
  Serial.println(" C");

  // Escreve a luminosidade na porta serial
  Serial.print("Luminosidade Relativa ");
  Serial.println(LuminosidadePorcentagem);
  delay(10);

  // Condições para que os LEDs que indicam os niveis de luminosidade acendam e apaguem
  // Nivel Baixo: de 0% a 40% (Acende só o LED verde)
  // Nivel Medio: de 40% a 80% (Acende o LED vermelho e o verde
  // Nivel Alto: de 80% a 100% (Acende só o LED vermelho)
  if (LuminosidadePorcentagem > 40) {
        digitalWrite(LedVerde,LOW);
            if (LuminosidadePorcentagem >= 40 && LuminosidadePorcentagem < 80){
                digitalWrite(LedVerde,HIGH);
                digitalWrite(LedVermelho,HIGH);
          }
               if (LuminosidadePorcentagem >= 80) {
                    digitalWrite(LedVermelho,HIGH);
                    digitalWrite(LedVerde,LOW);
              } 
    }
  else {
    digitalWrite(LedVerde,HIGH);
    digitalWrite(LedVermelho,LOW);
  }

  // Escreve o valor da Temperatura
  lcd.setCursor(0, 0); //Define o cursor na 1º linha e 1º coluna
  lcd.print("Temp.: ");
  lcd.print(TemperaturaCelsius);
  lcd.write(B11011111);
  lcd.print("C");

  // Escreve o valor da Luminosidade
  lcd.setCursor(0, 1); //Define o cursor na 2º linha e 1º coluna
  lcd.print("Lumi.: ");
  lcd.print(LuminosidadePorcentagem); 
  lcd.print("%");
  delay(1000);

}

// Função que escreve os valores de temperatura e luminosidade
void printa() {

  // Escreve o valor da Temperatura
  lcd.setCursor(0, 0); //Define o cursor na 1º linha e 1º coluna
  lcd.print("Temp.: ");
  lcd.print(TemperaturaCelsius);
  lcd.write(B11011111);
  lcd.print("C");

  // Escreve o valor da Luminosidade
  lcd.setCursor(0, 1); //Define o cursor na 2º linha e 1º coluna
  lcd.print("Lumi.: ");
  lcd.print(LuminosidadePorcentagem); 
  lcd.print("%");
  delay(1000);

  // Se o botão continuar pressionado ele fica "preso" dentro da função printa
  // Se o botão for solto voltamos para a função loop
  if (digitalRead (botao) == HIGH)
  printa();
  else
  loop();
  delay(10);  
  
}
