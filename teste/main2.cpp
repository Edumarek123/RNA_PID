#include <iostream>
#include "../libs/matriz_lib.hpp"
#include "../libs/PID_lib.hpp"


float v[15]={0.697166, -0.946053, -0.676279, 0.240732, -0.776667, -0.633455, 0.306689, 0.893748, 0.776167, -0.316701, 0.74726, -0.0425729, 0.661536, 0.115643, -0.154432};

int main() {
	float kp=1, kd=1, ti=1, ta=1;
	ControladorPID* CC=new ControladorPID(kp, kd, ti, ta);

	 //CC->RR->imprimir_rede();

	std::cout<<CC->Calcula_Acao_Controle(1, 5)<<std::endl<<std::endl;

	CC->Atualiza_Parametros();

	std::cout<<std::endl<<CC->Calcula_Acao_Controle(2, 3)<<std::endl;

	delete CC;

	return 0;
}
