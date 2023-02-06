#include <iostream>
//#include "/home/dudu/Arduino/libraries/RNA_PID/rede_neural_lib.hpp"
#include "/home/dudu/Arduino/libraries/RNA_PID/referencia_lib.hpp"


int main(){
    std::cout<<"\n\n\n";
    //int hl[]={10};
    //RedeNeural *RR=new RedeNeural(8, 4, hl, sizeof(hl)/sizeof(int), 0.01);
    //RR->carregar_rede(PESOS);

    //RR->TreinarRede("data/entradas.txt", "data/saidasEsperadas.txt", 10, 500);

    //RR->salvar_rede("teste_bancada_velocidade/rede_salva.hpp");

    //delete(RR);

    Referencia RR(0, 1, onda_senoidal);
    RR.velocidade = 0.05;

    for(_ponto i = 0; i < 2 * PpIi; i+=RR.velocidade){
        RR.atualiza_referencia();
        std::cout<<RR.modulo<<"\n";
    }

    return 0;
}
