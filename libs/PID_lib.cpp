//
// Created by Edumarek on 12/02/2022.
//

 #include"PID_lib.hpp"
#include <iostream>

 //------------------CONSTRUTORES&DESTRUTORES-------------------//

ControladorPID::ControladorPID(float kP, float kD, float tI, float tA){
  Kp=kP;
  Kd=kD;
  Ti=tI;
  Ta=tA;

  int hl[2]={5, 5};
  RR=new RedeNeural(5, 3, hl, 2, 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c){
  Kp=c.Kp;
  Kd=c.Kd;
  Ti=c.Ti;
  Ta=c.Ta;
  
  RR=c.RR;
}

ControladorPID::~ControladorPID(){
  delete RR;
}

//---------------------------METODOS---------------------------//

float ControladorPID::Calcula_Acao_Controle(float referencia, float saida){
  float a, b ,c, d;

  Yk[0]=saida;
  Ek[0]=saida-referencia;

  a=Uk[1];
  b=Ek[0]*(Kp+(Ta/(2*Ti))+(Kd/Ta));
  c=Ek[1]*(Ta/(2*Ti)-Kp-((2*Kd)/Ta));
  d=Ek[2]*(Kd/Ta);

  Uk[0]=a+b+c+d;

  //Atualiza Erros
  Ek[2]=Ek[1];
  Ek[1]=Ek[0];

  //Atualiza acoes de controle
  Uk[1]=Uk[0];

  //Atualiza saida
  Yk[1]=Yk[0];

  //Limitador acao controle
  if(Uk[0]>1)
    Uk[0]=1;
  if(Uk[0]<=0)
    Uk[0]=0;

  return Uk[0];
}

void ControladorPID::Atualiza_Parametros(){
  Matriz m(5, 1);

  m.matriz[0][0]=Uk[0];
  m.matriz[0][0]=Uk[1];
  m.matriz[0][0]=Yk[0];
  m.matriz[0][0]=Yk[1];
  m.matriz[0][0]=Ek[0];

  RR->FeedFoward(m);

  Kp=RR->saidas[RR->numero_layers-2]->matriz[0][0];
  Kd=RR->saidas[RR->numero_layers-2]->matriz[0][1];
  Ti=RR->saidas[RR->numero_layers-2]->matriz[0][2];
}
