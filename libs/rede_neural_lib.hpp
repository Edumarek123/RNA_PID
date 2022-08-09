//
// Created by Edumarek on 27/06/2021.
//

#ifndef REDE_NEURAL_LIB_HPP
#define REDE_NEURAL_LIB_HPP

#include "matriz_lib.hpp"

struct RedeNeural{
    float LEARNING_RATE;
    int numero_layers;
    int tam_layer_x;
    int tam_layer_y;
    int *tam_layers;

    Matriz **weights_layers;
    Matriz **bias_layers;
    Matriz **erros;
    Matriz **saidas;

    RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, float alfa_learn);
    RedeNeural()=delete;
    ~RedeNeural();

    void imprimir_rede();
    void carregar_rede2(float *v);

    void FeedFoward(Matriz input_layer);
    void Calcula_Erros(Matriz saidas_esperadas);
    void BackPropagation(Matriz entradas);
};

#endif //REDE_NEURAL_LIB_HPP
