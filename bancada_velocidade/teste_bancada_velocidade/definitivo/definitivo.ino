#include <PID_lib.hpp>

const bool MONITOR_SERIAL = false;

//Pinos
#define PIN_MOTOR 23
#define PIN_ENCODER 22

//PWM
const int CANAL_PWM = 0;
const int FREQUENCIA_PWM = 1000;
const int RESOLUCAO_PWM = 10;

//Parametros
const float KP = 0.14;
const float KD = 0;
const float KI = 1.28;
const float N = 1/0.0579;
const float TA = 0.08;
const float MIN_PWM = 0;
const float MAX_PWM = pow(2, RESOLUCAO_PWM)-1;
const int PULSOS_POR_VOLTA = 3;
const float COEFICIENTE_VELOCIDADE = ((1 / TA) * 60) / PULSOS_POR_VOLTA;  //(Ta[ms] * coeficienteRPM) / PulsosPorVolta
const int VELOCIDADE_MAX=9750;

//Variaveis Globais
int PULSOS_ENCODER = 0;
float REFERENCIA = 0;
float ANGULO=0;

//Controlador PID
ControladorPID CC(KP, KD, KI, N, TA, MIN_PWM, MAX_PWM);


void IRAM_ATTR interrupcao_encoder() {PULSOS_ENCODER++;}
float captura_velocidade();
void ativa_controle();
void realiza_ensaio(int pwm);
void altera_referencia(const int f, float iterador);


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
}

void loop() {
  ativa_controle();

  // altera_referencia(0, 5000);
  altera_referencia(3, 0.03);
  // altera_referencia(4, 0.1);
  // altera_referencia(5, 90);

  // realiza_ensaio(MAX_PWM);

}

float captura_velocidade() {
  //reseta contador
  PULSOS_ENCODER = 0;
  interrupts();
  delay(80);  //conta 100ms de pulsos (Ta)
  noInterrupts();
  
  return (COEFICIENTE_VELOCIDADE * PULSOS_ENCODER);
}

void ativa_controle() {
  float velocidadeAtual = captura_velocidade();
  float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA, velocidadeAtual);
  ledcWrite(CANAL_PWM, sinalControle);

  if (MONITOR_SERIAL)
    Serial.println("Referencia: " + String(REFERENCIA) + " RPM | Atual: " + String(velocidadeAtual) + " RPM | Erro: " + String(REFERENCIA - velocidadeAtual) + " RPM | Sinal de controle: " + String(sinalControle) + "bit");
  else {
    Serial.print("Max ");
    Serial.print(VELOCIDADE_MAX);
    Serial.print("Min ");
    Serial.print(0);
    Serial.print("Referenncia ");
    Serial.print(REFERENCIA);
    Serial.print(" Atual ");
    Serial.println(velocidadeAtual);
  }
}

void realiza_ensaio(int pwm) {
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

void altera_referencia(const int f, float iterador){
  switch (f){
    case 0: //degrau unitario
    REFERENCIA=iterador;
    break;

    case 1: //rampa
    REFERENCIA-=iterador;
    if (REFERENCIA <= 0)
      REFERENCIA = VELOCIDADE_MAX;
    break;

    case 2: //seno
    REFERENCIA=(VELOCIDADE_MAX/2)*sin(ANGULO)+(VELOCIDADE_MAX/2);
    ANGULO+=iterador;
    if(ANGULO>1000)
      ANGULO=0;
    break;
    
    case 3: //cos
    REFERENCIA=(VELOCIDADE_MAX/2)*cos(ANGULO)+(VELOCIDADE_MAX/2);
    ANGULO+=iterador;
    if(ANGULO>1000)
      ANGULO=0;
    break;

    case 4: //quadrada
    if(sin(ANGULO)>0)
      REFERENCIA=VELOCIDADE_MAX;
    else
      REFERENCIA=0;
    ANGULO+=iterador;
    if(ANGULO>1000)
      ANGULO=0;
    break;

    case 5: //triangular
    if(ANGULO==1)
      REFERENCIA-=iterador;
    else
      REFERENCIA+=iterador;
    if (REFERENCIA <= 0)
      ANGULO=0;
    if (REFERENCIA >= VELOCIDADE_MAX)
      ANGULO=1;
    break;

    default: //zero
    REFERENCIA=0;
  }

}
