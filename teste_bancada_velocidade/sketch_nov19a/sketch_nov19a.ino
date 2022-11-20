#include <PID_lib.hpp>


//Pinos
#define PIN_MOTOR 5
#define PIN_ENCODER 3

//Parametros
const float KP=-0.1335;
const float KD=-0.0066;
const float TI=-0.0146;
const float TA=0.1;
const float MIN_PWM=0;
const float MAX_PWM=255;

//Variaveis Globais
int PULSOS_ENCODER=0;
float REFERENCIA=9000;

//Controlador PID
ControladorPID CC(KP, KD, TI, TA, MIN_PWM, MAX_PWM);

void interrupcao_encoder();
float captura_velocidade();
void ativa_controle();
void realiza_ensaio(int pwm);

void setup() {
  //SERIAL
  Serial.begin(115200);

  //PINOS
  pinMode(PIN_MOTOR, OUTPUT);
  pinMode(PIN_ENCODER, INPUT);

  //INTERRUPCAO
  cli();
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER), interrupcao_encoder, FALLING);

  //PWM
  analogWrite(PIN_MOTOR, MIN_PWM);

}

void loop() {
  ativa_controle();

  REFERENCIA-=25;
  if(REFERENCIA<=0)
    REFERENCIA=9000;
}

void interrupcao_encoder(){
  PULSOS_ENCODER++;
  if(PULSOS_ENCODER>=5000)
    PULSOS_ENCODER=0;
}

float captura_velocidade(){
  //reseta contador
  PULSOS_ENCODER=0;
  sei();
  delay(100); //conta 100ms de pulsos
  cli();
  return(60*PULSOS_ENCODER*10);
}

void ativa_controle(){
  float velocidadeAtual=captura_velocidade();
  float sinalControle=CC.Calcula_Acao_Controle(REFERENCIA, velocidadeAtual);
  analogWrite(PIN_MOTOR, sinalControle);

  Serial.println("Referencia: "+String(REFERENCIA)+" RPM | Atual: "+String(velocidadeAtual)+" RPM | Sinal de controle: "+String(sinalControle)+"bit");
}

void realiza_ensaio(int pwm){
  float velocidadeAtual=0;
  float velocidadePassada=0;
  int cont=0;

  Serial.println(velocidadeAtual);
  Serial.println(velocidadeAtual);
  Serial.println(velocidadeAtual);
  Serial.println(velocidadeAtual);
  analogWrite(PIN_MOTOR, pwm);
  do{
    velocidadePassada=velocidadeAtual;
    velocidadeAtual=captura_velocidade();

    Serial.println(velocidadeAtual);

    if(velocidadeAtual==velocidadePassada)
      cont++;
    else
      cont=0;
  }while(cont<6);
  
  analogWrite(PIN_MOTOR, MIN_PWM);
}
