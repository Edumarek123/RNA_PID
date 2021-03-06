//
// Created by Edumarek on 12/02/2022.
//

#ifndef PID_LIB_HPP
#define PID_LIB_HPP
#include <iostream>
#include <vector>
#include"rede_neural_lib.hpp"
#include"matriz_lib.hpp"

struct ControladorPID{
    //auto tuning
    RedeNeural r;
    //parametros controlador
    float Kp;
    float Kd;
    float Ti;
    float Ta;
    //variaveis controlador
    std::vector<float> Ek; //erro
    std::vector<float> Uk; //acao de controle
    std::vector<float> Yk; //Saida
    std::vector<float> Ck; //sinal se controle

    ControladorPID()=delete;
    ControladorPID(float Kp, float Kd, float Ti, float Ta);
    ControladorPID(const ControladorPID &c);
    ~ControladorPID();

    float Calcula_Acao_Controle(float referencia, float saida);
    void Atualiza_Parametros();
};

#endif //PID_LIB_HPP
