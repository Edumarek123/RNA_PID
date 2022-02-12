//
// Created by Edumarek on 12/02/2022.
//

#ifndef PID_LIB_HPP
#define PID_LIB_HPP
#include <iostream>
#include <vector>

struct ControladorPID{
    //parametros controlador
    float Kp;
    float Kd;
    float Ti;
    float Ta;
    //variaveis controlador
    std::vector<float> Uk; //acao de controle
    std::vector<float> Ek; //erro

    ControladorPID()=delete;
    ControladorPID(float Kp, float Kd, float Ti, float Ta);
    ControladorPID(const ControladorPID &c);

    float Calcula_Acao_Controle(float referencia, float saida);
};

#endif //PID_LIB_HPP
