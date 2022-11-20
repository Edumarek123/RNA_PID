//
// Created by Edumarek on 12/02/2022.
//

#ifndef PID_LIB_HPP
#define PID_LIB_HPP

#if __linux__
    #include<iostream>
#else
    #include<Arduino.h>
#endif
#include"rede_neural_lib.hpp"
#include"matriz_lib.hpp"

struct ControladorPID{
    //auto tuning
    RedeNeural* RR;
    //parametros controlador
    float Kp=0;
    float Kd=0;
    float Ti=0;
    float Ta;
    float min_Uk=0;
    float max_Uk=1;
    //variaveis controlador
    float Ek[3]={0, 0, 0}; //erro
    float Uk[2]={0, 0}; //acao de controle
    float Yk[2]={0, 0}; //Saida

    ControladorPID()=delete;
    ControladorPID(float tA, float min_uK, float max_uK);
    ControladorPID(float kP, float kD, float tI, float tA, float min_uK, float max_uK);
    ControladorPID(const ControladorPID &c);
    ~ControladorPID();

    void imprimir_parametros();
    float Calcula_Acao_Controle(float referencia, float saida);
    void Atualiza_Parametros();
};

#endif //PID_LIB_HPP
