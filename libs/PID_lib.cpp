//
// Created by Edumarek on 12/02/2022.
//

 #include"PID_lib.hpp"

 //------------------CONSTRUTORES&DESTRUTORES-------------------//
ControladorPID::ControladorPID(float tA, float min_uK, float max_uK){
  Ta=tA;
  min_Uk=min_uK;
  max_Uk=max_uK;

  int hl[2]={5, 5};
  RR=new RedeNeural(5, 3, hl, 2, 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(float kP, float kD, float tI, float tA, float min_uK, float max_uK){
  Kp=kP;
  Kd=kD;
  Ti=tI;
  Ta=tA;
  min_Uk=min_uK;
  max_Uk=max_uK;

  int hl[2]={5, 5};
  RR=new RedeNeural(5, 3, hl, 2, 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c){
  Kp=c.Kp;
  Kd=c.Kd;
  Ti=c.Ti;
  Ta=c.Ta;
  min_Uk=c.min_Uk;
  max_Uk=c.max_Uk;

  RR=c.RR;
}

ControladorPID::~ControladorPID(){
  delete RR;
}

//---------------------------METODOS---------------------------//

#if defined __linux__
void ControladorPID::imprimir_parametros(){
  std::cout<<"Kp: "<<Kp<<" | Kd: "<<Kd<<" | Ti: "<<Ti<<" | Ta: "<<Ta<<std::endl;

}
#else
void ControladorPID::imprimir_parametros(){
  Serial.println("Kp: "+String(Kp)+" | Kd: "+String(Kd)+" | Ti: "+String(Ti)+" | Ta: "+String(Ta));
}
#endif


float ControladorPID::Calcula_Acao_Controle(float referencia, float saida){
  float a, b ,c, d;

  //Calcula erro
  Yk[0]=saida;
  Ek[0]=saida-referencia;

  //Calcula acao de controle
  a=Uk[1];
  b=Ek[0]*(Kp+(Ta/(2*Ti))+(Kd/Ta));
  c=Ek[1]*(Ta/(2*Ti)-Kp-((2*Kd)/Ta));
  d=Ek[2]*(Kd/Ta);

  Uk[0]=a+b+c+d;

  //Limitador acao controle
  if(Uk[0]>max_Uk)
    Uk[0]=max_Uk;
  if(Uk[0]<=min_Uk)
    Uk[0]=min_Uk;

  //Atualiza erros
  Ek[2]=Ek[1];
  Ek[1]=Ek[0];

  //Atualiza acoes de controle
  Uk[1]=Uk[0];

  //Atualiza saida
  Yk[1]=Yk[0];

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
