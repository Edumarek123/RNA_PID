#include <iostream>
#include "../libs/rede_neural_lib.hpp"



int main(){
    int hl[2]={5, 5};

    RedeNeural *RR=new RedeNeural(9, 4, hl, 2, 0.1);


    RR->TreinarRede("data/entradas.txt", "data/saidas_esperadas.txt", 10, 15);
    




    return 0;
}
