#include <iostream>
#include "../libs/rede_neural_lib.hpp"

int main(){
    int hl[]={2};

    RedeNeural *RR=new RedeNeural(2, 1, hl, sizeof(hl)/sizeof(int), 1);

    RR->AlteraFuncaoAtivacao("tanh", 1, false);

    RR->TreinarRede("data2/entradas.txt", "data2/saidas_esperadas.txt", 10, 20);

    delete(RR);

    return 0;
}
