//
// Created by Edumarek on 12/02/2022.
//

#ifndef PID_LIB_HPP
#define PID_LIB_HPP
#include"rede_neural_lib.hpp"
#include"matriz_lib.hpp"

struct ControladorPID{
    //auto tuning
    RedeNeural* RR;
    //parametros controlador
    float Kp;
    float Kd;
    float Ti;
    float Ta;
    //variaveis controlador
    float Ek[3]={0, 0, 0}; //erro
    float Uk[2]={0, 0}; //acao de controle
    float Yk[2]={0, 0}; //Saida
    float Ck=0; //sinal se controle

    ControladorPID()=delete;
    ControladorPID(float kP, float kD, float tI, float tA);
    ControladorPID(const ControladorPID &c);
    ~ControladorPID();

    float Calcula_Acao_Controle(float referencia, float saida);
    void Atualiza_Parametros();
};

#endif //PID_LIB_HPP
