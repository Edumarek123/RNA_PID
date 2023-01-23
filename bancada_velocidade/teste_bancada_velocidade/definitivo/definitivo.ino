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

//REDE TREINADA
_ponto PESOS[]={0.751792, 3.84069, 6.89558, 2.04191, 0.779793, -0.660983, 0.437468, -2.93044, -2.10311, -0.245297, -2.00174, -7.36014, -7.61603, -7.08688, -1.01621, -2.43428, -0.633493, 0.944367, 4.87135, 3.12316, -3.26788, -5.70889, 6.6175, 3.73513, 6.84438, 1.78071, -0.141556, 4.32254, 4.75714, 3.35538, -1.68012, 3.12601, -0.725801, 2.18317, 7.10223, 1.5815, -2.46281, -3.01948, 7.20149, 0.586525, -2.88719, -3.49813, -5.97146, 3.33688, 0.432788, -1.47864, -10.006, 5.97595, 0.739286, -0.614011, -1.92048, -2.64962, -2.58609, -6.48759, 0.180596, 0.380114, 4.0852, 3.53381, -5.87889, -2.8257, -2.62424, -1.37498, 5.63822, 1.6534, -1.57217, 1.03091, -2.78364, -11.4907, -8.02382, -16.9415, 0.635831, -0.816864, -5.16629, -2.41852, 1.87352, 3.14073, 0.654564, -2.69449, 0.136797, 3.23045, 2.83155, 7.12093, 4.30775, -5.52609, 2.80338, 6.17491, -5.58868, 2.98371, -7.37452, -4.50211, 3.21659, 7.06925, 4.3891, -5.92603, 3.10407, 6.75293, -5.92258, 3.47704, -7.45123, -4.84654, 0.307518, -1.0082, -1.63251, -1.77858, -1.59453, -0.756273, -1.74787, -0.933898, -0.401118, 0.351102, -3.73388, -9.16829, -4.32922, 8.8645, -4.30492, -7.53415, 5.51861, -6.33419, 10.6719, 7.15754, -5.88905, 15.2367, -3.10754, -11.481, -6.03222, 10.3213, 8.64534, -4.23506, 21.373, -1.02109, -3.28901, -3.89231, -1.74565, -0.0932667};

//Variaveis Globais
int PULSOS_ENCODER = 0;
float REFERENCIA = 0;
int contREFERENCIA1 = 0;
int contREFERENCIA2 = 0;
float ANGULO=0;
int contAtualizarParametros=0;

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

  //CONTROLADOR
  CC.RR->carregar_rede(PESOS);

  delay(400);
}

void loop() {
  ativa_controle();

  switch (contREFERENCIA2){
    case 0:
      altera_referencia(0, 5000);
    break;
    case 1:
      altera_referencia(3, 0.03);
    break;
    case 2:
      altera_referencia(4, 0.1);
    break;
    case 3:
      altera_referencia(5, 90);
    break;
    default:
      REFERENCIA=REFERENCIA;
  }

  // realiza_ensaio(MAX_PWM);
  
  contREFERENCIA1++;
  if(contREFERENCIA1>150){
    contREFERENCIA1=0;
    contREFERENCIA2++;
  }
  if(contREFERENCIA2>3)
    contREFERENCIA2=0;

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
    Serial.print(" Min ");
    Serial.print(0);
    Serial.print(" Referenncia ");
    Serial.print(REFERENCIA);
    Serial.print(" Atual ");
    Serial.println(velocidadeAtual);
  }

  // CC.Atualiza_Parametros();
  CC.imprimir_parametros();
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
      REFERENCIA=VELOCIDADE_MAX/2;
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
