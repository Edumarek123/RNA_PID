#include <PID_lib.hpp>


//Pinos
#define PIN_MOTOR 5
#define PIN_ENCODER 3

//Parametros
const float MIN_PWM=0;
const float MAX_PWM=255;

//Variaveis Globais
int PULSOS_ENCODER=0;
float REFERENCIA=6000;


//Controlador PID
int HL[1]={3};
RedeNeural RR(2, 1, HL, 1, 0.1);

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

  // REFERENCIA-=25;
  // if(REFERENCIA<=0)
  //   REFERENCIA=9000;
}

void interrupcao_encoder(){
  PULSOS_ENCODER++;
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
  
  Matriz entradas(2, 1);
  entradas.matriz[0][0]=REFERENCIA;
  entradas.matriz[1][0]=velocidadeAtual;
  RR.FeedFoward(entradas);

  float sinalControle=RR.saidas[RR.numero_layers-2]->matriz[0][0];
  if(sinalControle>MAX_PWM)
    sinalControle=MAX_PWM;
  if(sinalControle<=MIN_PWM)
    sinalControle=MIN_PWM;
  
  analogWrite(PIN_MOTOR, sinalControle);

  Serial.println("Referencia: "+String(REFERENCIA)+" RPM | Atual: "+String(velocidadeAtual)+" RPM | Sinal de controle: "+String(sinalControle)+"bit");
}

void realiza_ensaio(int pwm){
  float velocidadeAtual=0;
  float velocidadePassada=0;
  int cont=0;
  
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
