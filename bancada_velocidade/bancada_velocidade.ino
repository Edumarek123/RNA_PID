#include "src/PID_lib/PID_lib.hpp"
#include "src/referencia_lib/referencia_lib.hpp"
#include "rede_salva.hpp"
#include "delimitadores_salvos.hpp"

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
const int TA_S = TA * 1000;

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
Referencia REFERENCIA(0, VELOCIDADE_MAX, onda_triangular);

//Funcoes
void IRAM_ATTR interrupcao_encoder() {PULSOS_ENCODER++;}
float captura_velocidade();
void imprimi_informacoes(float velocidadeAtual, float sinalControle, float referencia, bool monitorSerial);
void roda_continuo();
void realiza_ensaio_FFT(int pwm);
void realiza_ensaio_matlab();
void realiza_ensaio_dinamico(unsigned long tSimulacao, refType funcaoEntrada, _ponto velocidade, _ponto anguloInicial);


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
    CC.CarregarDelimitadores(DELIMITADORES_SALVOS);
    CC.calcularTempoExecucao = true;
    CC.imprimirParametros = true;
    CC.atualizarParametros = true;

    //REFERENCIA
    REFERENCIA.velocidade = 0.05;

    delay(1000);
}

void loop() {
    //roda_continuo();

    //realiza_ensaio_FFT(MAX_PWM);

    //realiza_ensaio_matlab();

    //realiza_ensaio_dinamico(15, degrau_unitario, 0.05, 0); //tempo de simulacao, funcao entrada, velocidade referencia, angulo inicial referencia
    //realiza_ensaio_dinamico(15, rampa, 0.05, 0); //tempo de simulacao, funcao entrada, velocidade referencia, angulo inicial referencia
    //realiza_ensaio_dinamico(15, onda_quadrada, 0.05, 0); //tempo de simulacao, funcao entrada, velocidade referencia, angulo inicial referencia
    //realiza_ensaio_dinamico(15, onda_triangular, 0.05, 0); //tempo de simulacao, funcao entrada, velocidade referencia, angulo inicial referencia
    //realiza_ensaio_dinamico(15, onda_senoidal, 0.05, 0); //tempo de simulacao, funcao entrada, velocidade referencia, angulo inicial referencia
}

float captura_velocidade() {
    //reseta contador
    PULSOS_ENCODER = 0;
    interrupts();
    delay(TA_S);  //conta 100ms de pulsos (Ta)
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

void roda_continuo(){
    float velocidadeAtual = captura_velocidade();
    float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, velocidadeAtual);
    ledcWrite(CANAL_PWM, sinalControle);

    imprimi_informacoes(velocidadeAtual, sinalControle, REFERENCIA.modulo, false);

    REFERENCIA.atualiza_referencia();  
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

void realiza_ensaio_matlab(){
    unsigned long tSimulacao = 15 * 1000;
    unsigned long tInicio = millis();
    unsigned long tAtual = 0;

    REFERENCIA.entrada = onda_quadrada;
    do{
        float velocidadeAtual = captura_velocidade();
        float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, velocidadeAtual);
        ledcWrite(CANAL_PWM, sinalControle);

        imprimi_informacoes(velocidadeAtual, sinalControle, REFERENCIA.modulo, false);

        REFERENCIA.atualiza_referencia();

        tAtual = millis() - tInicio;

        REFERENCIA.entrada = onda_quadrada;
        if(tAtual > tSimulacao / (3))
            REFERENCIA.entrada = onda_senoidal;
        if(tAtual > tSimulacao / (3 / 2))
            REFERENCIA.entrada = onda_triangular;

    }while(tAtual <= tSimulacao);

    while(true);
}

void realiza_ensaio_dinamico(unsigned long tSimulacao, refType funcaoEntrada, _ponto velocidade, _ponto anguloInicial){
    tSimulacao *= 1000;

    REFERENCIA.entrada = funcaoEntrada;
    REFERENCIA.angulo = anguloInicial;
    REFERENCIA.velocidade = velocidade;

    unsigned long tInicio = millis();
    unsigned long tAtual = 0;
    do{
        float velocidadeAtual = captura_velocidade();
        float sinalControle = CC.Calcula_Acao_Controle(REFERENCIA.modulo, velocidadeAtual);
        ledcWrite(CANAL_PWM, sinalControle);

        imprimi_informacoes(velocidadeAtual, sinalControle, REFERENCIA.modulo, false);

        REFERENCIA.atualiza_referencia();


        REFERENCIA.entrada = onda_quadrada;
        if(tAtual > tSimulacao / (3))
            REFERENCIA.entrada = onda_senoidal;
        if(tAtual > tSimulacao / (3 / 2))
            REFERENCIA.entrada = onda_triangular;

        tAtual = millis() - tInicio;
    }while(tAtual <= tSimulacao);

    while(true);
}

