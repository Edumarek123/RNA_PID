//
// Created by Edumarek on 27/06/2021.
//

#ifndef REDE_NEURAL_LIB_HPP
#define REDE_NEURAL_LIB_HPP

#if defined __linux__
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <sstream>
    #include <assert.h>
#else
    #include<Arduino.h>
#endif
#include "matriz_lib.hpp"

struct RedeNeural{
    float LEARNING_RATE;
    int numero_layers;
    int tam_layer_x;
    int tam_layer_y;
    int *tam_layers;
    char **fun_layers;
    char **Dfun_layers;
    int index_resposta;

    Matriz **weights_layers;
    Matriz **bias_layers;
    Matriz **erros;
    Matriz **saidas;

    RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, float alfa_learn);
    RedeNeural()=delete;
    ~RedeNeural();

    void imprimir_rede();
    void carregar_rede(float *v);

    void FeedFoward(Matriz entradas); 
    void CalculaErros(Matriz saidas_esperadas);
    void BackPropagation(Matriz entradas);  
    bool RealizarTeste(const Matriz entrada, Matriz saidaEsperada);
    void AlteraFuncaoAtivacao(const char* F, int L, bool iniciando);
    
    #if defined __linux__
    float TreinarRede(const std::string &entradas, const std::string &saidas_esperadas, const float quantidadePercentualTestes, const int epochs);
    #else
    float TreinarRede(Matriz entradas, Matriz saidas_esperadas, int quantidadePercentualTestes);
    #endif
};

int memoria_alocada(const RedeNeural *NN);
Matriz* separa_dados(const std::string linha);
int contagem_dados(const std::string caminho);

#endif //REDE_NEURAL_LIB_HPP
