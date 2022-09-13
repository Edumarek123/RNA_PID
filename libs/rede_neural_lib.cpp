//
// Created by Edumarek on 27/06/2021.
//

#include "rede_neural_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//

RedeNeural::RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, float alfa_learn){
	tam_layer_x=tam_Lx;
	tam_layer_y=tam_Ly;
	numero_layers=num_hl+2;

	LEARNING_RATE=alfa_learn;

	tam_layers=(int* )malloc(numero_layers*sizeof(int));
	tam_layers[0]=tam_layer_x;


	for(int i=1;i<numero_layers-1;i++)
		tam_layers[i]=tams_hl[i-1];
	tam_layers[numero_layers-1]=tam_layer_y;


	bias_layers=(Matriz** )malloc((numero_layers-1)*sizeof(Matriz*));
	for(int i=0;i<numero_layers-1;i++)
		bias_layers[i]=new Matriz(tam_layers[i+1],1);

	weights_layers=(Matriz** )malloc((numero_layers-1)*sizeof(Matriz*));
	for(int i=0;i<numero_layers-1;i++)
		weights_layers[i]=new Matriz(tam_layers[i+1],tam_layers[i]);

	erros=(Matriz** )malloc((numero_layers-1)*sizeof(Matriz*));
	for(int i=0;i<numero_layers-1;i++)
		erros[i]=new Matriz(tam_layers[i+1], 1);

	saidas=(Matriz** )malloc((numero_layers-1)*sizeof(Matriz*));
	for(int i=0;i<numero_layers-1;i++)
		saidas[i]=new Matriz(tam_layers[i+1], 1);
}

RedeNeural::~RedeNeural(){
	for(int i=0;i<numero_layers-1;i++){
		delete bias_layers[i];
		delete weights_layers[i];
		delete erros[i];
		delete saidas[i];
	}

	free(bias_layers);
	free(weights_layers);
	free(erros);
	free(saidas);
	free(tam_layers);
}

//---------------------------METODOS---------------------------//

void RedeNeural::imprimir_rede() {
	std::cout << std::endl << "Neural Network Structure( I | Wn | O )" << std::endl << std::endl;

	std::cout<<"Tamanho layers"<<std::endl<<std::endl;
	std::cout<<"|  X  | ";
	for(int i=0;i<numero_layers-2;i++)
		std::cout<<"HL"<<i<<" | ";
	std::cout<<" Y  |"<<std::endl<<"| ";
	for(int i=0;i<numero_layers;i++)
		std::cout<<" "<<tam_layers[i]<<"  | ";
	std::cout<<std::endl<<std::endl;

	std::cout<<"Vizualizacao | Entradas | Pesos | Saidas |"<<std::endl<<std::endl;
	for (int i = 0; i < tam_layer_x; ++i)
		std::cout<<"X"<<std::endl;
	std::cout << std::endl;

	for (int i = 0; i < numero_layers-1; ++i) {
		weights_layers[i]->imprimir();
		std::cout << std::endl;
	}

	for (int i = 0; i < tam_layer_y; ++i)
		std::cout<<"Y"<<std::endl;
	std::cout << std::endl;

	std::cout << std::endl<< "Bias" << std::endl << std::endl;
	for (int i = 0; i < (numero_layers-1); ++i) {
		bias_layers[i]->imprimir();
		std::cout << std::endl;
	}
}


// void RedeNeural::imprimir_rede() {
// 	Serial.print("\nNeural Network Structure( I | Wn | O )\n\n");

// 	Serial.print("Tamanho layers\n\n");
// 	Serial.print("| X |");
// 	for(int i=0;i<numero_layers-2;i++){
// 		Serial.print("HL");
// 		Serial.print(i);
// 		Serial.print(" | ");

// 	}

// 	Serial.print(" Y |\n| ");
// 	for(int i=0;i<numero_layers;i++){
// 		Serial.print(" ");
// 		Serial.print(tam_layers[i]);
// 		Serial.print("  | ");
// 	}
// 	Serial.print("\n\n");

// 	Serial.print("Vizualizacao | Entradas | Pesos | Saidas |\n\n");
// 	for (int i = 0; i < tam_layer_x; ++i)
// 		Serial.print("X\n");
// 	Serial.print("\n");

// 	for (int i = 0; i < numero_layers-1; ++i) {
// 		weights_layers[i]->imprimir();
// 		Serial.print("\n");
// 	}

// 	for (int i = 0; i < tam_layer_y; ++i)
// 		Serial.print("Y\n");
// 	Serial.print("\n");

// 	std::cout << std::endl<< "Bias" << std::endl << std::endl;
// 	for (int i = 0; i < (numero_layers-1); ++i) {
// 		bias_layers[i]->imprimir();
// 		Serial.print("\n");
// 	}
// }

void RedeNeural::carregar_rede(float *v){
	long int cont=0;

	for (int i = 0; i < numero_layers - 1; i++)
		for (int j = 0; j < weights_layers[i]->linhas; j++)
			for (int k = 0; k < weights_layers[i]->colunas; k++){
				weights_layers[i]->matriz[j][k]=v[cont];
				cont++;
			}

	for (int i = 0; i < numero_layers - 1; i++)
		for (int j = 0; j < bias_layers[i]->linhas; j++)
			for (int k = 0; k < bias_layers[i]->colunas; k++){
				bias_layers[i]->matriz[j][k]=v[cont];
				cont++;
			}
}

void RedeNeural::FeedFoward(Matriz input_layer){
	//z_l=W_l.X+B
	//z_l=W_l.a_(l-1)+B
	//a_l=6z_l

	for (int i=0;i<(numero_layers-1);i++)
		delete saidas[i];

	saidas[0]=new Matriz(multiplicar(*weights_layers[0], input_layer));
	saidas[0]->somar(*bias_layers[0]);
	saidas[0]->map(0); //"sigmoide"

	for (int i = 1; i < (numero_layers-1); i++) {
		saidas[i]=new Matriz(multiplicar(*weights_layers[i], *saidas[i-1]));
		saidas[i]->somar(*bias_layers[i]);
		saidas[i]->map(0); //"sigmoide"
	}
}

void RedeNeural::Calcula_Erros(Matriz saidas_esperadas) {
	//e_L = (A_L - Y) 0 d6(z_L)
	//e_l = ((W_(l+1))T(erro_(l+1))) 0 d6(z_l)

	delete erros[numero_layers-2];
	erros[numero_layers-2]=new Matriz(subtrair(saidas_esperadas, *saidas[numero_layers-2]));
	erros[numero_layers-2]->multiplicar_Hadamard(map(*saidas[numero_layers-2], 3)); //"Dsigmoide"

	for (int i=numero_layers-3; i>=0; i--){
		delete erros[i];
		erros[i]=new Matriz(multiplicar(transposta(*weights_layers[i+1]), *erros[i+1]));
		erros[i]->multiplicar_Hadamard(map(*saidas[i], 3)); //"Dsigmoide"
	}
}

void RedeNeural::BackPropagation(Matriz entradas){
	//∇Cw_l = a_(l-1) * e_l
	//∇Cb_l = e_l
	for(int i=numero_layers-2;i>=1;i--){
		weights_layers[i]->somar(multiplicar_escalar((multiplicar(*erros[i], transposta(*saidas[i-1]))), LEARNING_RATE));
		bias_layers[i]->somar(multiplicar_escalar(*erros[i], LEARNING_RATE));
	}
	weights_layers[0]->somar(multiplicar_escalar((multiplicar(*erros[0], transposta(entradas))), LEARNING_RATE));
	bias_layers[0]->somar(multiplicar_escalar(*erros[0], LEARNING_RATE));
}

//----------------------FUNCOES AUXILIARES---------------------//
