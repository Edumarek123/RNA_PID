//
// Created by Edumarek on 27/06/2021.
//

#ifndef REDE_NEURAL_LIB_HPP
#define REDE_NEURAL_LIB_HPP
#include "matriz_lib.h"
#include <vector>

using namespace std;

struct RedeNeural{
    float LEARNING_RATE;
    int numero_layers;
    int tam_layer_x;
    int tam_layer_y;
    vector<int> tam_layers;

    vector<Matriz*> weights_layers;
    vector<Matriz*> bias_layers;
    vector<Matriz*> erros;
    vector<Matriz*> saidas;

    RedeNeural(int tam_Lx, int tam_Ly, vector<int> tams_hl, float alfa_learn);
    RedeNeural()=delete;
    ~RedeNeural();
    RedeNeural(const RedeNeural &r);


    void imprimir_rede();
    void salvar_rede();
    void carregar_rede(const std::string path);

    void FeedFoward(Matriz input_layer);
    void Calcula_Erros(Matriz saidas_esperadas);
    void BackPropagation(Matriz entradas);

    void TreinarRede(const std::string entradas,const std::string saidas_esperadas, int porcentagem_testes, float threshold);
};

void covolucao_saidas(Matriz* m);

int contagem_dados(std::string path_arquivo);
Matriz* separa_dados(std::string linha);

#endif //REDE_NEURAL_LIB_HPP
