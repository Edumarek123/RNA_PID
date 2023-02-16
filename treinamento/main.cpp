#include <iostream>
#include "rede_neural_lib/rede_neural_lib.hpp"

int main(){
    int hl[]={10};
    RedeNeural *RR=new RedeNeural(8, 4, hl, sizeof(hl)/sizeof(int), 0.01);
    //RR->carregar_rede(PESOS_REDE_SALVA);

    RR->TreinarRede("../data/entradas.txt", "../data/saidasEsperadas.txt", 10, 500);

    RR->salvar_rede("../data/rede_salva.hpp");

    delete(RR);

    return 0;
}
