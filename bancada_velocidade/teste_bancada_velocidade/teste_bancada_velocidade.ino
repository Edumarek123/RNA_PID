#include <PID_lib.hpp>
#include <referencia_lib.hpp>
#include "rede_salva.hpp"

//Pinos
#define PIN_MOTOR 23
#define PIN_ENCODER 22

//PWM
const int CANAL_PWM = 0;
const int FREQUENCIA_PWM = 1000;
const int RESOLUCAO_PWM = 10; 

//Parametros Controlador
const float KP = 0.14;
const float KD = 0;
const float KI = 1.28;
const float N = 1/0.0579;
const float TA = 0.08;

//Parametros Sensor
const float MIN_PWM = 0;
const float MAX_PWM = pow(2, RESOLUCAO_PWM) - 1;
const int PULSOS_POR_VOLTA = 3;
const float COEFICIENTE_VELOCIDADE = ((1 / TA) * 60) / PULSOS_POR_VOLTA;  //(Ta[ms] * coeficienteRPM) / PulsosPorVolta
const int VELOCIDADE_MAX = 9750;

//Variaveis Globais
int PULSOS_ENCODER = 0;

//Controlador PID
ControladorPID CC(KP, KD, KI, N, TA, MIN_PWM, MAX_PWM);

//Referencia
Referencia REFERENCIA(0, VELOCIDADE_MAX, onda_senoidal);

void IRAM_ATTR interrupcao_encoder() {PULSOS_ENCODER++;}
float captura_velocidade();
void imprimi_informacoes(float velocidadeAtual, float sinalControle, float referencia, bool monitorSerial);
void realiza_ensaio_FFT(int pwm);

void setup() {
  //SERIAL
  Serial.begin(115200);

  //PINOS
  pinMode(PIN_ENCODER, INPUT_PULLUP);

  //PWM
  ledcAttachPin(PIN_MOTOR, CANAL_PWM);
  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcWrite(CANAL_PWM, MIN_PWM);

  //INTERRUPCAO
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER), interrupcao_encoder, FALLING);
  noInterrupts();

  //CONTROLADOR
  CC.RR->carregar_rede(PESOS_REDE_SALVA);
  CC.calcularTempoExecucao = true;

  //REFERENCIA
  REFERENCIA.velocidade = 0.05;

  delay(1000);
}

void loop() {
  float velocidadeAtual = captura_velocidade();
  float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, velocidadeAtual);
  ledcWrite(CANAL_PWM, sinalControle);

  imprimi_informacoes(velocidadeAtual, sinalControle, REFERENCIA.modulo, false);

  REFERENCIA.atualiza_referencia();  
}

float captura_velocidade() {
  //reseta contador
  PULSOS_ENCODER = 0;
  interrupts();
  delay(80);  //conta 100ms de pulsos (Ta)
  noInterrupts();
  
  return (COEFICIENTE_VELOCIDADE * PULSOS_ENCODER);
}

void imprimi_informacoes(float velocidadeAtual, float sinalControle, float referencia, bool monitorSerial) {
  if (monitorSerial)
    Serial.println("Referencia: " + String(referencia) + " RPM | Atual: " + String(velocidadeAtual) + " RPM | Erro: " + String(referencia - velocidadeAtual) + " RPM | Sinal de controle: " + String(sinalControle) + "bit");
  else {
    Serial.print("Max ");
    Serial.print(VELOCIDADE_MAX);
    Serial.print(" Min ");
    Serial.print(0);
    Serial.print(" Referenncia ");
    Serial.print(referencia);
    Serial.print(" Atual ");
    Serial.println(velocidadeAtual);
  }
}

void realiza_ensaio_FFT(int pwm) {
  float velocidadeAtual = 0;
  float velocidadePassada = 0;
  int cont = 0;

  Serial.println(velocidadeAtual);
  ledcWrite(CANAL_PWM, pwm);
  do {
    velocidadePassada = velocidadeAtual;
    velocidadeAtual = captura_velocidade();

    Serial.println(velocidadeAtual);

    if (velocidadeAtual == velocidadePassada)
      cont++;
    else
      cont = 0;
  } while (cont < 6);

  ledcWrite(CANAL_PWM, MIN_PWM);
}
