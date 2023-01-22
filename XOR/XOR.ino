#include<rede_neural_lib.hpp>

#define PIN_C 26
#define PIN_B 33
#define PIN_A 32

Matriz DADOS_Entrada[]={Matriz(2, 1), Matriz(2, 1), Matriz(2, 1), Matriz(2, 1)};
Matriz DADOS_SAIDAESPERADAS[]={Matriz(1, 1), Matriz(1, 1), Matriz(1, 1), Matriz(1, 1)};
int hl[]={2};
RedeNeural RR(2, 1, hl, 1, 1);


int TREINAR=0;

void setup() {
  //SERIAL
  Serial.begin(115200);

  //PINOS
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);

  //REDE NEURAL
  RR.AlteraFuncaoAtivacao("tanh", 1);

  //CRIA DATASET PORTA LOGICA XOR
  DADOS_Entrada[0].matriz[0][0]=0;
  DADOS_Entrada[0].matriz[1][0]=0;
  DADOS_SAIDAESPERADAS[0].matriz[0][0]=0;

  DADOS_Entrada[1].matriz[0][0]=0;
  DADOS_Entrada[1].matriz[1][0]=1;
  DADOS_SAIDAESPERADAS[1].matriz[0][0]=1;

  DADOS_Entrada[2].matriz[0][0]=1;
  DADOS_Entrada[2].matriz[1][0]=0;
  DADOS_SAIDAESPERADAS[2].matriz[0][0]=1;

  DADOS_Entrada[3].matriz[0][0]=1;
  DADOS_Entrada[3].matriz[1][0]=1;
  DADOS_SAIDAESPERADAS[3].matriz[0][0]=0;

  //TREINA A REDE
  RR.TreinarRede(DADOS_Entrada, DADOS_SAIDAESPERADAS, 4, 0, 1000);    

  delay(500);
}

void loop() {
  for(int i=0;i<4;i++){
    digitalWrite(PIN_A, (bool)DADOS_Entrada[i].matriz[0][0]);
    digitalWrite(PIN_B, (bool)DADOS_Entrada[i].matriz[1][0]);

    RR.FeedFoward(DADOS_Entrada[i]);
    RR.saidas[RR.index_resposta]->binary();

    digitalWrite(PIN_C, (bool)RR.saidas[RR.index_resposta]->matriz[0][0]);

    delay(500);
  }
  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
}
