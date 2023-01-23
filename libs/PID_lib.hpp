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
    _ponto Kp=0;
    _ponto Kd=0;
    _ponto Ki=0;
    _ponto N=1;
    _ponto Ta=1;
    _ponto min_Uk=0;
    _ponto max_Uk=1;
    //variaveis controlador
    _ponto Ek[3]={0, 0, 0}; //erro
    _ponto Uk[3]={0, 0, 0}; //acao de controle
    _ponto Yk[2]={0, 0}; //Saida

    ControladorPID()=delete;
    ControladorPID(_ponto tA, _ponto min_uK, _ponto max_uK);
    ControladorPID(_ponto kP, _ponto kD, _ponto tI, _ponto N, _ponto tA, _ponto min_uK, _ponto max_uK);
    ControladorPID(const ControladorPID &c);
    ~ControladorPID();

    void imprimir_parametros();
    _ponto Calcula_Acao_Controle(_ponto referencia, _ponto saida);
    void Atualiza_Parametros();
};

#endif //PID_LIB_HPP
