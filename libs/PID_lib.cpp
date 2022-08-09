//
// Created by Edumarek on 12/02/2022.
//

 #include"PID_lib.hpp"


 //------------------CONSTRUTORES&DESTRUTORES-------------------//

ControladorPID::ControladorPID(float kP, float kD, float tI, float tA){
  Kp=kP;
  Kd=kD;
  Ti=tI;
  Ta=tA;

  for(int i=0;i<3;i++){
    Uk.push_back(0);
    Ek.push_back(0);
  }
  Uk.shrink_to_fit();
  Ek.shrink_to_fit();

  r=new RedeNeural(7, 3, {5, 5}, 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c){
  Kp=c.Kp;
  Kd=c.Kd;
  Ti=c.Ti;
  Ta=c.Ta;

  for(int i=0;i<3;i++)
    Ek.push_back(0);
  Ek.shrink_to_fit();

  for(int i=0;i<2;i++){
    Uk.push_back(0);
    Yk.push_back(0);
    Ck.push_back(0);
  }
  Uk.shrink_to_fit();
  Yk.shrink_to_fit();
  Ck.shrink_to_fit();
}

ControladorPID::~ControladorPID(){
  delete r;
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

  return Uk[0];
}

void ControladorPID::Atualiza_Parametros(){
  Matriz m(7, 1);

  m.matriz[0][0]=Uk[0];
  m.matriz[0][0]=Uk[1];
  m.matriz[0][0]=Yk[0];
  m.matriz[0][0]=Yk[1];
  m.matriz[0][0]=Ck[0];
  m.matriz[0][0]=Ck[1];
  m.matriz[0][0]=Ek[0];

  r->feedfoward(*m);

  Kp=r->saidas[r->numero_layers-2]->matriz[0];
  Kd=r->saidas[r->numero_layers-2]->matriz[1];
  Ti=r->saidas[r->numero_layers-2]->matriz[2];
}
