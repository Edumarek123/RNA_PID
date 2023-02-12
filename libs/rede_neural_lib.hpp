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
    _ponto LEARNING_RATE;
    int numero_layers;
    int tam_layer_x;
    int tam_layer_y;
    int *tam_layers;
    funType *fun_layers;
    funType *Dfun_layers;
    int index_resposta;
    int bytesAlocados;

    Matriz **weights_layers;
    Matriz **bias_layers;
    Matriz **erros;
    Matriz **saidas;

    RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, _ponto alfa_learn);
    RedeNeural()=delete;
    ~RedeNeural();

    void imprimir_rede();
    void carregar_rede(_ponto *v);
    void salvar_rede(const char *s);
    int memoria_alocada();
    void FeedFoward(Matriz entradas); 
    void CalculaErros(Matriz saidas_esperadas);
    void BackPropagation(Matriz entradas);  
    bool RealizarTeste(const Matriz *entradas,const Matriz *saidasEsperadas);
    void AlteraFuncaoAtivacao(const char* F, int L);

    #if defined __linux__
    float TreinarRede(const std::string &entradas, const std::string &saidas_esperadas, const float quantidadePercentualTestes, const int epochs);
    #else
    float TreinarRede(const Matriz *entradas,const Matriz *saidasEsperadas, const int numeroDados, const float quantidadePercentualTestes, const int epochs);
    #endif
};

bool s_iguais(const char *A, const char *B);
int memoria_alocada(const RedeNeural *NN);
Matriz* separa_dados(const std::string linha);
int contagem_dados(const std::string caminho);

#endif //REDE_NEURAL_LIB_HPP
