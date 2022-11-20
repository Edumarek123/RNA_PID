#include "../libs/PID_lib.hpp"



int main(){
    int hl[2]={10, 10};
    RedeNeural r(10, 10, hl, 2, 0.1);

    r.imprimir_rede();


    return 0;
}
