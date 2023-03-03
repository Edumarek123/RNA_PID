#include <Wire.h>
#include <VL53L0X.h>
#include "src/PID_lib/PID_lib.hpp"
#include "src/referencia_lib/referencia_lib.hpp"

//Pinos
#define PIN_MOTOR1 25
#define PIN_MOTOR2 33

//PWM
const int CANAL_PWM = 0;
const int FREQUENCIA_PWM = 1000;
const int RESOLUCAO_PWM = 10; 
const float MIN_PWM = 0;
const float MAX_PWM = pow(2, RESOLUCAO_PWM) - 1;

//Parametros Controlador
const double KP = 0.0008;
const double KD = 0.0002;
const double KI = 0;
const double N = 1/0.00278;
const double TA = 0.02;

//Variaveis Globais
float DISTANCIA_MAX = 245;
float DISTANCIA_MIN = 185;


//Parametros Sensor
VL53L0X sensor;

//Controlador PID
ControladorPID CC(KP, KD, KI, N, TA, MIN_PWM, MAX_PWM);

//Referencia
Referencia REFERENCIA(DISTANCIA_MIN, DISTANCIA_MAX, degrau_unitario);

//Funcoes
void imprimi_informacoes(float distanciaAtual, float sinalControle, float referencia, bool monitorSerial);
float le_distancia(){return sensor.readRangeContinuousMillimeters();};
void roda_continuo();

void setup() {
  //SERIAL
  Serial.begin(115200);

  //PINOS
  pinMode(PIN_MOTOR2, OUTPUT);
  digitalWrite(PIN_MOTOR2, LOW);

  //PWM
  ledcAttachPin(PIN_MOTOR1, CANAL_PWM);
  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcWrite(CANAL_PWM, MIN_PWM);

  //SENSOR
  Wire.begin();
  sensor.setTimeout(500);
  if(!sensor.init()){
    Serial.println("erro");
    while(1);
  }
  sensor.startContinuous(20);

  //CONTROLADOR
  // CC.RR->carregar_rede(PESOS_REDE_SALVA);
  // CC.CarregarDelimitadores(DELIMITADORES_SALVOS);
  // CC.calcularTempoExecucao = true;
  CC.imprimirParametros = true;
  CC.atualizarParametros = false;

  //REFERENCIA
  REFERENCIA.velocidade = 0.9;
  
  delay(1000);
}

void loop() {
    roda_continuo();
}


void imprimi_informacoes(float distanciaAtual, float sinalControle, float referencia, bool monitorSerial) {
  if (monitorSerial)
      Serial.println("Referencia: " + String(referencia) + " mm | Atual: " + String(distanciaAtual) + " mm | Erro: " + String(referencia - distanciaAtual) + " mm | Sinal de controle: " + String(sinalControle) + "bit");
  else {
      Serial.print("Max ");
      Serial.print(DISTANCIA_MAX);
      Serial.print(" Min ");
      Serial.print(DISTANCIA_MIN);
      Serial.print(" Referenncia ");
      Serial.print(referencia);
      Serial.print(" Atual ");
      Serial.println(distanciaAtual);
      Serial.println(" SinalControle: " + String(sinalControle));
    }
}

void roda_continuo(){
  float distanciaAtual = le_distancia();
  float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, distanciaAtual);
  ledcWrite(CANAL_PWM, sinalControle);

  imprimi_informacoes(distanciaAtual, sinalControle, REFERENCIA.modulo, false);

  REFERENCIA.atualiza_referencia();
}
