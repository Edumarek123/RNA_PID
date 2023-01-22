#include <iostream>
#include "/home/dudu/Arduino/libraries/RNA_PID/rede_neural_lib.hpp"


int main(){
    int hl[]={10};
    RedeNeural *RR=new RedeNeural(8, 4, hl, sizeof(hl)/sizeof(int), 0.01);
	// RR->AlteraFuncaoAtivacao("tanh", RR->numero_layers-2);

    RR->TreinarRede("data/entradas.txt", "data/saidasEsperadas.txt", 10, 2000);

    delete(RR);

    return 0;
}
