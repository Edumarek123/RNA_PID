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
}

ControladorPID::ControladorPID(const ControladorPID &c){
  Kp=c.Kp;
  Kd=c.Kd;
  Ti=c.Ti;
  Ta=c.Ta;

  for(int i=0;i<3;i++){
    Uk.push_back(0);
    Ek.push_back(0);
  }
  Uk.shrink_to_fit();
  Ek.shrink_to_fit();
}

//---------------------------METODOS---------------------------//

void ControladorPID::Calcula_Erro(float referencia, float saida){
  Ek[0]=saida-referencia;
}

float ControladorPID::Calcula_Acao_Controle(){
  float a, b ,c, d;

  a=Uk[1];
  b=Ek[0]*(Kp+(Ta/(2*Ti))+(Kd/Ta));
  c=Ek[1]*(Ta/(2*Ti)-Kp-((2*Kd)/Ta));
  d=Ek[2]*(Kd/Ta);

  return (a+b+c+d);
}

//---------------------------FUNCOES---------------------------//
