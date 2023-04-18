#include "src/PID_lib/PID_lib.hpp"
#include "src/referencia_lib/referencia_lib.hpp"
// #include "rede_salva.hpp"
// #include "delimitadores_salvos.hpp"

//Pinos
#define PIN_MOTOR1 25
#define PIN_MOTOR2 33
#define PIN_A0 4
#define PIN_A1 2
#define PIN_A2 15

//Parametros Planta
const double R = 2.4; //Ω
const double I_LINEARIZACAO = 1.22; //A
const double X_LINEARIZACAO = 7.5; //mm
const double TENSAO_LINEARIZACAO = I_LINEARIZACAO * R; //V
const double TENSAO_MAXIMA = 12; //V

//PWM
const int CANAL_PWM = 0;
const int FREQUENCIA_PWM = 20000;
const int RESOLUCAO_PWM = 10; 
const float MIN_PWM = 0;
const float MAX_PWM = (pow(2, RESOLUCAO_PWM) - 1);

//Parametros Controlador
const double KP = 0.94099;
const double KD = 0.03468;
const double KI = 0.0;
const double N = 13763.0024;
const double TA = 1.5 / 1000;

// Sensor
#define ITERACOES_CALIBRACAO 100
#define DT_ITERACOES_CALIBRACAO 5 //ms
const double REGIAO_ATUACAO = 30; // mm
const double OFFSET_BOBINA = 0;   // mm
const double OFFSET_CORPO = 4.3;   // mm
double DISTANCIA_ANTERIOR = REGIAO_ATUACAO + OFFSET_BOBINA;
double SINAL_MAX; //bits
double SINAL_MIN; //bits
double SINAL_A0_MAX; //bits
double SINAL_A0_MIN; //bits
double SINAL_A2_MAX; //bits
double SINAL_A2_MIN; //bits

//Variaveis Globais
const double DISTANCIA_MAX = REGIAO_ATUACAO / 2;
const double DISTANCIA_MIN = -DISTANCIA_MAX;

//Controlador PID
ControladorPID CC(KP, KD, KI, N, TA, -TENSAO_LINEARIZACAO, TENSAO_MAXIMA - TENSAO_LINEARIZACAO);

//Referencia
Referencia REFERENCIA(DISTANCIA_MIN, DISTANCIA_MAX, degrau_unitario);

//Funcoes
double media_sinal(int ax);
void rotina_calibracao(int iteracoes);
double captura_distancia();
void imprimi_informacoes(float distanciaAtual, float sinalControle, float referencia, bool monitorSerial);
void tocar_nota(float f, float a, int t1, int t2);
void sequencia_iniciacao(int AMPLITUDE);

void setup()
{
  //SERIAL
  Serial.begin(500000);

  //PINOS
  pinMode(PIN_MOTOR2, OUTPUT);
  digitalWrite(PIN_MOTOR2, HIGH);

  //PWM
  ledcAttachPin(PIN_MOTOR1, CANAL_PWM);
  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcWrite(CANAL_PWM, MAX_PWM - MIN_PWM);

  //SENSOR
  rotina_calibracao();

  //CONTROLADOR
  // CC.RR->carregar_rede(PESOS_REDE_SALVA);
  // CC.CarregarDelimitadores(DELIMITADORES_SALVOS);
  // CC.imprimirParametros = true;
  CC.atualizarParametros = false;

  //REFERENCIA
  REFERENCIA.velocidade = 0.5;

  sequencia_iniciacao(65);

  delay(1000);
}

void loop() 
{
  float distanciaAtual = captura_distancia() - X_LINEARIZACAO;
  
  double sinalControle;
  sinalControle = TENSAO_LINEARIZACAO + CC.Calcula_Acao_Controle(0, distanciaAtual);
  sinalControle *= MAX_PWM / TENSAO_MAXIMA;
  sinalControle = MAX_PWM - sinalControle;

  ledcWrite(CANAL_PWM, sinalControle);
  
  imprimi_informacoes(distanciaAtual, 100 - (100 * sinalControle / MAX_PWM), REFERENCIA.modulo, false);

  REFERENCIA.atualiza_referencia();
}

double media_sinal(int ax)
{
  double sinal = 0;
  for (int i = 0; i < ITERACOES_CALIBRACAO; i++)
  {
    if(ax == 210)
      sinal += analogRead(PIN_A0) + analogRead(PIN_A1) + analogRead(PIN_A2);
    if(ax == 0)
      sinal += analogRead(PIN_A0);
    if(ax == 2)
      sinal += analogRead(PIN_A2);

    delay(DT_ITERACOES_CALIBRACAO);
  }
  return (sinal / ITERACOES_CALIBRACAO);
}

void rotina_calibracao()
{
  SINAL_MIN = media_sinal(210);
  SINAL_A0_MIN = media_sinal(0);
  SINAL_A2_MIN = media_sinal(2);

  Serial.println("Tampe os LDRs");

  // AGUARDA MUDANCA
  while (analogRead(PIN_A0) + analogRead(PIN_A1) + analogRead(PIN_A2) < SINAL_MIN * 1.35)
    delay(DT_ITERACOES_CALIBRACAO);
  delay(2000);  

  SINAL_MAX = media_sinal(210);
  SINAL_A0_MAX = media_sinal(0);
  SINAL_A2_MAX = media_sinal(2);

  Serial.println("Sinal min: " + String(SINAL_MIN) + " | Sinal max: " + String(SINAL_MAX));
}

double captura_distancia()
{
  double distancia = 0;

  int A0 = analogRead(PIN_A0);
  int A1 = analogRead(PIN_A1);
  int A2 = analogRead(PIN_A2);
  
  double M_A0 = (A0 - SINAL_A0_MIN) / (SINAL_A0_MAX - SINAL_A0_MIN);
  double M_A2 = (A2 - SINAL_A2_MIN) / (SINAL_A2_MAX - SINAL_A2_MIN);

  distancia = REGIAO_ATUACAO * ((A0 + A1 + A2 - SINAL_MIN) / (SINAL_MAX - SINAL_MIN));

  //verifica regiao
  if(M_A2 > M_A0 + 0.02)
    distancia = REGIAO_ATUACAO - distancia;
  
  //verifica vazio
  if(A0 + A1 + A2 < SINAL_MIN * 1.1)
    distancia = REGIAO_ATUACAO;
  
  //limitador
  if(distancia > REGIAO_ATUACAO)
    distancia = REGIAO_ATUACAO;
  if(distancia < 0)
    distancia = 0;
  if(distancia - DISTANCIA_ANTERIOR > REGIAO_ATUACAO / 2 && A0 + A1 + A2 > SINAL_MIN * 1.1)
    distancia = DISTANCIA_ANTERIOR;

  DISTANCIA_ANTERIOR = distancia;

  return distancia + OFFSET_BOBINA + OFFSET_CORPO;
}

void imprimi_informacoes(float distanciaAtual, float sinalControle, float referencia, bool monitorSerial)
{
  Serial.print("Max ");
  Serial.print(DISTANCIA_MAX);
  Serial.print(" Min ");
  Serial.print(-DISTANCIA_MIN);
  Serial.print(" Referenncia ");
  Serial.print(referencia);
  Serial.print(" Atual ");
  Serial.print(distanciaAtual);
  Serial.print(" Sinalcontrole ");
  Serial.print(sinalControle);
  Serial.println("");
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
}

