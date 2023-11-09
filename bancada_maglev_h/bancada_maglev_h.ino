#include "src/PID_lib/PID_lib.hpp"
#include "src/referencia_lib/referencia_lib.hpp"
#include "rede_salva.hpp"
#include "delimitadores.hpp"

//Pinos
#define PIN_BOBINA 25
#define PIN_ESTADO_DRIVER 33
#define PIN_A0 4

//PWM
const int CANAL_PWM = 2;
const int FREQUENCIA_PWM = 30000;
const int RESOLUCAO_PWM = 10; 
const float MIN_PWM = 0;
const float MAX_PWM = pow(2, RESOLUCAO_PWM) - 1;

//Parametros Controlador
// with Kp = 0.186, Ki = 715, Kd = 8.9e-06, Tf = 3.51e-05
double KP = 0.2;
double KD = 0.01;
double KI = 2.0;
double N = 30;
double TA = 5.0 / 1000.0;

//SENSOR
double ADC_MINIMO = 0;
double ADC_MAXIMO = 0;
double ADC_DELTA = 0;
double REGIAO_ATUACAO = 10;
double DISTANCIA_PASSADA = 0;
int NUMERO_LEITURAS = 10;

//Controlador PID
ControladorPID CC(KP, KD, KI, N, TA, 0, 1);

//Referencia
Referencia REFERENCIA(2, 6, onda_quadrada);
// Referencia REFERENCIA(2, 6, onda_senoidal);

//Variaveis Globais
long T_PASSADO = 0;

//Funcoes
double captura_distancia(int n);
void comuta_acao_controle(double valor);
void zeramento_sensor();
void ensaio();
void atualiza_manual();
void tocar_nota(float f, float a, int t1, int t2);
void sequencia_iniciacao(int AMPLITUDE);


void setup()
{
  //SERIAL
  Serial.begin(921600);

  //DRIVER
  pinMode(PIN_ESTADO_DRIVER, OUTPUT);
  digitalWrite(PIN_ESTADO_DRIVER, LOW);

  //PWM
  ledcAttachPin(PIN_BOBINA, CANAL_PWM);
  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, RESOLUCAO_PWM);
  comuta_acao_controle(0);

  //SENSOR
  pinMode(PIN_A0, INPUT);
  zeramento_sensor();

  //CONTROLADOR
  CC.RR->carregar_rede(PESOS_REDE_SALVA);
  CC.CarregarDelimitadores(DELIMITADORES_SALVOS);
  // CC.imprimirParametros = true;
  CC.atualizarParametros = true;

  //REFERENCIA
  REFERENCIA.velocidade = 0.002;

  //ENSAIO
  // ensaio();

  // sequencia_iniciacao(90);

}

void loop() 
{
  T_PASSADO = micros();
  //Leitura Sensor
  double distanciaAtual = REGIAO_ATUACAO * (captura_distancia(NUMERO_LEITURAS) - ADC_MINIMO) / ADC_DELTA;
  if(distanciaAtual < 0)
    distanciaAtual = 0;
  
  //Controle
  double sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, distanciaAtual);
  comuta_acao_controle(sinalControle);

  //Imprimi infos
  Serial.print("u 10 d 0 a ");
  Serial.print(distanciaAtual);
  Serial.print(" r ");
  Serial.println(REFERENCIA.modulo);

  //Atualizacoes
  REFERENCIA.atualiza_referencia();
  atualiza_manual();

  //Espera TA fechar em 5ms
  while(micros() - T_PASSADO < 5000);
  T_PASSADO = micros() - T_PASSADO;
}

double captura_distancia(int n)
{
  double distancia = 0;
  for(int i = 0; i < n; i++)
    distancia += analogRead(PIN_A0);
  return distancia / n;
}

void comuta_acao_controle(double valor)
{
  valor = 1 - valor;
  valor *= MAX_PWM;
  ledcWrite(CANAL_PWM, (int)valor);
}

void zeramento_sensor()
{
  comuta_acao_controle(0);
  delay(3000);
  ADC_MINIMO = captura_distancia(200);
  delay(100);

  comuta_acao_controle(1);
  delay(3000);
  ADC_MAXIMO = captura_distancia(200);

  comuta_acao_controle(0);
  ADC_DELTA = ADC_MAXIMO - ADC_MINIMO;
  Serial.println("MAX: " + String(ADC_MAXIMO) + " MIN: " + String(ADC_MINIMO) + " DELTA: " + String(ADC_DELTA));
  delay(3000);
}

void ensaio()
{
  comuta_acao_controle(0);
  delay(4000);
  T_PASSADO = micros();
  comuta_acao_controle(1);
  for(int i = 0; i < 1000; i ++)
  {
    double distanciaAtual = REGIAO_ATUACAO * (captura_distancia(20) - ADC_MINIMO) / ADC_DELTA;
    if(distanciaAtual < 0)
      distanciaAtual = 0;
    
    Serial.print("u 10 d 0 a ");
    Serial.println(distanciaAtual);
  }
  Serial.println("TEMPO TOTAL: " + String((micros() - T_PASSADO) / 1000));
}

void atualiza_manual()
{
  // att manual
  String valores = "";
  if(Serial.available()){
    valores = Serial.readString();
  }
    
  if(valores != ""){
    String s = valores.substring(1);

    if(valores.lastIndexOf("p") != -1){
      CC.Kp = s.toDouble();
    }

    else if(valores.lastIndexOf("i") != -1){
      CC.Ki = s.toDouble();
    } 
    else if(valores.lastIndexOf("d") != -1){
      CC.Kd = s.toDouble();
    }
    else if(valores.lastIndexOf("n") != -1){
      CC.N = s.toDouble();
    }
    else if(valores.lastIndexOf("c") != -1){
      comuta_acao_controle(s.toDouble());
      Serial.println(s.toDouble());
    }
    else if(valores.lastIndexOf("r") != -1){
      REFERENCIA.modulo = s.toDouble();
    }
    else if(valores.lastIndexOf("f") != -1){
      CC.atualizarParametros = !CC.atualizarParametros;
      delay(400);
      Serial.println(CC.atualizarParametros);
      CC.Kp = KP;
      CC.Ki = KI;
      CC.Kd = KD;
      CC.N = N;
    }
    else if(valores.lastIndexOf("a") != -1){
      CC.imprimirParametros = !CC.imprimirParametros;    
    }
    else if(valores.lastIndexOf("v") != -1){
      REFERENCIA.velocidade = s.toDouble();
    }
    else{
      //nada
    }
  }
}





void tocar_nota(float f, float a, int t1, int t2)
{
  ledcSetup(CANAL_PWM, f, RESOLUCAO_PWM);
  float pwm = MAX_PWM - ((a * MAX_PWM) / 100);
  ledcWrite(CANAL_PWM ,(int)pwm);
  for(int i = 0; i < t1 / 1.5; i++)
    delay(1);
  ledcWrite(CANAL_PWM, MAX_PWM - MIN_PWM);
  for(int i = 0; i < t2 / 1.5; i++)
    delay(1);
}

void sequencia_iniciacao(int AMPLITUDE)
{
  //Marcha Imperial
  tocar_nota(440, AMPLITUDE, 500, 300); //A4
  tocar_nota(440, AMPLITUDE, 500, 300); //A4
  tocar_nota(440, AMPLITUDE, 500, 300); //A4
  tocar_nota(349, AMPLITUDE, 350, 200); //F4
  tocar_nota(292, AMPLITUDE, 150, 100); //C#4
  tocar_nota(440, AMPLITUDE, 500, 300); //A4
  tocar_nota(349, AMPLITUDE, 350, 200); //F4
  tocar_nota(292, AMPLITUDE, 150, 100); //C#4
  tocar_nota(440, AMPLITUDE, 600, 400); //A4

  delay(500);

  tocar_nota(329, AMPLITUDE, 500, 300); //E#4
  tocar_nota(329, AMPLITUDE, 500, 300); //E#4
  tocar_nota(329, AMPLITUDE, 500, 300); //E#4
  tocar_nota(370, AMPLITUDE, 350, 200); //F#4
  tocar_nota(292, AMPLITUDE, 150, 100); //C#4
  tocar_nota(415, AMPLITUDE, 500, 300); //G#4
  tocar_nota(349, AMPLITUDE, 350, 200); //F4
  tocar_nota(292, AMPLITUDE, 150, 100); //C#4
  tocar_nota(440, AMPLITUDE, 600, 400); //A4

  delay(500);

  tocar_nota(466, AMPLITUDE, 500, 300); //A#4
  tocar_nota(440, AMPLITUDE, 300, 150); //A4
  tocar_nota(440, AMPLITUDE, 150, 100); //A4
  tocar_nota(466, AMPLITUDE, 500, 300); //A#4
  tocar_nota(415, AMPLITUDE, 325, 200); //G#4
  tocar_nota(415, AMPLITUDE, 175, 100); //G#4
  tocar_nota(349, AMPLITUDE, 125, 100); //F4
  tocar_nota(370, AMPLITUDE, 125, 100); //F#4
  tocar_nota(349, AMPLITUDE, 125, 100); //F4
  tocar_nota(370, AMPLITUDE, 250, 100); //F#4

  delay(325);
  tocar_nota(455, AMPLITUDE, 250, 100); //As4
  tocar_nota(622, AMPLITUDE, 500, 300); //D4sh
  tocar_nota(587, AMPLITUDE, 325, 200); //D4h
  tocar_nota(554, AMPLITUDE, 175, 100); //C4sh
  tocar_nota(523, AMPLITUDE, 125, 100); //C4h
  tocar_nota(494, AMPLITUDE, 125, 100); //B4
  tocar_nota(523, AMPLITUDE, 250, 100); //C4h

  delay(500);

  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcWrite(CANAL_PWM, MAX_PWM);
}