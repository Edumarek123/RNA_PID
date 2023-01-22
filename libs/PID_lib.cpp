//
// Created by Edumarek on 12/02/2022.
//

 #include"PID_lib.hpp"

 //------------------CONSTRUTORES&DESTRUTORES-------------------//
ControladorPID::ControladorPID(float tA, float min_uK, float max_uK){
  Ta=tA;
  min_Uk=min_uK;
  max_Uk=max_uK;

  int hl[]={5, 5};
  RR=new RedeNeural(8, 4, hl, sizeof(hl)/sizeof(int), 1); //entradas, saidas, camadas ocultas, numero de camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(float kP, float kD, float kI, float n, float tA, float min_uK, float max_uK){
  Kp=kP;
  Kd=kD;
  Ki=kI;
  Ta=tA;
  N=n;
  min_Uk=min_uK;
  max_Uk=max_uK;

  int hl[]={5, 5};
  RR=new RedeNeural(8, 4, hl, sizeof(hl)/sizeof(int), 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c){
  Kp=c.Kp;
  Kd=c.Kd;
  Ki=c.Ki;
  N=c.N;
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
  std::cout<<"Kp: "<<Kp<<" | Kd: "<<Kd<<" | Ki: "<<Ki<<" | N: "<<N<<" | Ta: "<<Ta<<std::endl;

}
#else
void ControladorPID::imprimir_parametros(){
  Serial.println("Kp: "+String(Kp)+" | Kd: "+String(Kd)+" | Ki: "+String(Ki)+" | N: "+String(N)+" | Ta: "+String(Ta));
}
#endif


float ControladorPID::Calcula_Acao_Controle(float referencia, float saida){
  double a, b ,c, d, e;

  //Calcula erro
  Yk[0]=saida;
  Ek[0]=referencia-saida;

  //Calcula acao de controle
  a=Kp*(1+(N*Ta))+(Ki*Ta)*(1+(N*Ta))+(Kd*N);
  b=-(Kp*(2+(N*Ta))+(Ki*Ta)+(2*Kd*N));
  c=Kp+(Kd*N);
  d=1+(N*Ta);
  e=-(2+(N*Ta));

  Uk[0]=(float)((-e*Uk[1])-(1*Uk[2])+(a*Ek[0])+(b*Ek[1])+(c*Ek[2]))/d; //by mateus

  //Limitador acao controle
  if(Uk[0]>max_Uk)
    Uk[0]=max_Uk;
  if(Uk[0]<=min_Uk)
    Uk[0]=min_Uk;

  //Atualiza Saidas
  Yk[1]=Yk[0];

  //Atualiza erros
  Ek[2]=Ek[1];
  Ek[1]=Ek[0];

  //Atualiza acoes de controle
  Uk[1]=Uk[0];
  Uk[2]=Uk[1];

  return Uk[0];
}

void ControladorPID::Atualiza_Parametros(){
  Matriz m(8, 1);

  m.matriz[0][0]=Uk[0];
  m.matriz[1][0]=Uk[1];
  m.matriz[2][0]=Uk[2];
  m.matriz[3][0]=Yk[0];
  m.matriz[4][0]=Yk[1];
  m.matriz[5][0]=Ek[0];
  m.matriz[6][0]=Ek[1];
  m.matriz[7][0]=Ek[2];

  RR->FeedFoward(m);

  Kp=RR->saidas[RR->numero_layers-2]->matriz[0][0];
  Kd=RR->saidas[RR->numero_layers-2]->matriz[0][1];
  Ki=RR->saidas[RR->numero_layers-2]->matriz[0][2];
  N=RR->saidas[RR->numero_layers-2]->matriz[0][3];
}
