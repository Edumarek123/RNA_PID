//
// Created by Edumarek on 27/06/2021.
//

#include "rede_neural_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//

RedeNeural::RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, float alfa_learn){
    tam_layer_x=tam_Lx;
    tam_layer_y=tam_Ly;
    numero_layers=num_hl+2;
    index_resposta=numero_layers-2;

    LEARNING_RATE=alfa_learn;

    tam_layers=(int* )malloc(numero_layers*sizeof(int));
    tam_layers[0]=tam_layer_x;

    for(int i=1;i<numero_layers-1;i++)
        tam_layers[i]=tams_hl[i-1];
    tam_layers[numero_layers-1]=tam_layer_y;

    fun_layers=(char** )malloc((numero_layers-1)*sizeof(char*));
    Dfun_layers=(char** )malloc((numero_layers-1)*sizeof(char*));

    for(int i=0;i<numero_layers-1;i++)
        AlteraFuncaoAtivacao("sigmoide", i, true);


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
        free(fun_layers[i]);
        free(Dfun_layers[i]);
    }

    free(bias_layers);
    free(weights_layers);
    free(erros);
    free(saidas);
    free(tam_layers);
    free(fun_layers);
    free(Dfun_layers);
}

//---------------------------METODOS---------------------------//

#if defined __linux__
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

#else
void RedeNeural::imprimir_rede() {
    Serial.print("\nNeural Network Structure( I | Wn | O )\n\n");

    Serial.print("Tamanho layers\n\n");
    Serial.print("| X |");
    for(int i=0;i<numero_layers-2;i++){
        Serial.print("HL");
        Serial.print(i);
        Serial.print(" | ");

    }

    Serial.print(" Y |\n| ");
    for(int i=0;i<numero_layers;i++){
        Serial.print(" ");
        Serial.print(tam_layers[i]);
        Serial.print("  | ");
    }
    Serial.print("\n\n");

    Serial.print("Vizualizacao | Entradas | Pesos | Saidas |\n\n");
    for (int i = 0; i < tam_layer_x; ++i)
        Serial.print("X\n");
    Serial.print("\n");

    for (int i = 0; i < numero_layers-1; ++i) {
        weights_layers[i]->imprimir();
        Serial.print("\n");
    }

    for (int i = 0; i < tam_layer_y; ++i)
        Serial.print("Y\n");
    Serial.print("\n");

    Serial.print("\nBias\n\n");
    for (int i = 0; i < (numero_layers-1); ++i) {
        bias_layers[i]->imprimir();
        Serial.print("\n");
    }
}
#endif

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

void RedeNeural::FeedFoward(Matriz entradas){
    //z_l=W_l.X+B
    //z_l=W_l.a_(l-1)+B
    //a_l=6z_l

    for (int i=0;i<(numero_layers-1);i++) delete saidas[i];
    saidas[0]=new Matriz(multiplicar(weights_layers[0], &entradas));
    saidas[0]->somar(bias_layers[0]);
    saidas[0]->map(fun_layers[0]); //"f"
    // saidas[0]->map("sigmoide"); //"f"

    for (int i = 1; i < (numero_layers-1); i++) {
        saidas[i]=new Matriz(multiplicar(weights_layers[i], saidas[i-1]));
        saidas[i]->somar(bias_layers[i]);
        saidas[i]->map(fun_layers[i]); //"f"
        // saidas[i]->map("sigmoide"); //"f"
    }
}

void RedeNeural::CalculaErros(Matriz saidas_esperadas) {
    //e_L = (A_L - Y) 0 d6(z_L)
    //e_l = ((W_(l+1))T(erro_(l+1))) 0 d6(z_l)

    delete erros[numero_layers-2];
    erros[numero_layers-2]=new Matriz(subtrair(&saidas_esperadas, saidas[numero_layers-2]));
    Matriz aux1(map(saidas[numero_layers-2], Dfun_layers[numero_layers-2])); //"Df"
    // Matriz aux1(map(saidas[numero_layers-2], "Dsigmoide")); //"Df"
    erros[numero_layers-2]->multiplicar_Hadamard(&aux1); 

    for (int i=numero_layers-3; i>=0; i--){
        delete erros[i];
        Matriz aux2(transposta(weights_layers[i+1]));
        erros[i]=new Matriz(multiplicar(&aux2, erros[i+1]));
        Matriz aux3(map(saidas[i], Dfun_layers[i])); //"Df"
        // Matriz aux3(map(saidas[i], "Dsigmoide")); //"Df"
        erros[i]->multiplicar_Hadamard(&aux3); 
    }
}

void RedeNeural::BackPropagation(Matriz entradas){
    //∇Cw_l = a_(l-1) * e_l
    //∇Cb_l = e_l
    for(int i=numero_layers-2;i>=1;i--){
        Matriz aux1(transposta(saidas[i-1]));
        Matriz aux2(multiplicar(erros[i], &aux1));
        Matriz aux3(multiplicar_escalar(&aux2, LEARNING_RATE));
        weights_layers[i]->somar(&aux3);

        Matriz aux4(multiplicar_escalar(erros[i], LEARNING_RATE));
        bias_layers[i]->somar(&aux4);
    }
    Matriz aux1(transposta(&entradas));
    Matriz aux2(multiplicar(erros[0], &aux1));
    Matriz aux3(multiplicar_escalar(&aux2, LEARNING_RATE));
    weights_layers[0]->somar(&aux3);

    Matriz aux4(multiplicar_escalar(erros[0], LEARNING_RATE));
    bias_layers[0]->somar(&aux4);
}

#if defined __linux__
float RedeNeural::TreinarRede(const std::string &Pentradas,const std::string &PsaidasEsperadas,const float quantidadePercentualTestes, const int epochs){ 
    std::ifstream entradas;
    std::ifstream saidasEsperadas;
    entradas.open(Pentradas); 
    saidasEsperadas.open(PsaidasEsperadas); 
    int numeroDados=contagem_dados(Pentradas);
    int numeroDadosTestes=(int)((numeroDados*quantidadePercentualTestes)/100); //x% para testes ao final
    int numeroDadosTestesTreino=(int)(numeroDados*0.05); //5% para testes durante treinamento
    int numeroDadosTreinamento=numeroDados-numeroDadosTestes-numeroDadosTestesTreino; //o restante para treinamento

    int cont=0;
    float accuracy=0;
    float loss=0;   
    std::string Sentradas, SsaidasEsperadas;
    if (entradas.is_open() && saidasEsperadas.is_open()) {
        for(int e=0;e<epochs;e++){
            if(accuracy<1){
                entradas.clear();
                saidasEsperadas.clear();
                entradas.seekg (0);  
                saidasEsperadas.seekg (0);  

                cont=0;
                accuracy=0;
                loss=0;
                while (entradas && saidas && cont<numeroDadosTreinamento) {
                    std::getline (entradas, Sentradas);
                    std::getline (saidasEsperadas, SsaidasEsperadas);
                    if(Sentradas.size()!=0 || SsaidasEsperadas.size()!=0){
                        cont++;
            
                        Matriz *Mentradas(separa_dados(Sentradas));
                        Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                        FeedFoward(*Mentradas);
                        CalculaErros(*MsaidasEsperadas);
                        BackPropagation(*Mentradas);

                        delete Mentradas;
                        delete MsaidasEsperadas;
                    }
                }

                while (entradas && saidas && cont<numeroDadosTreinamento+numeroDadosTestesTreino) {
                    std::getline (entradas, Sentradas);
                    std::getline (saidasEsperadas, SsaidasEsperadas);
                    if(Sentradas.size()!=0 || SsaidasEsperadas.size()!=0){
                        cont++;
                        
                        Matriz *Mentradas(separa_dados(Sentradas));
                        Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                        if(RealizarTeste(*Mentradas, *MsaidasEsperadas))
                            accuracy++;                

                        delete Mentradas;
                        delete MsaidasEsperadas;
                    }   
                }
                accuracy/=numeroDadosTestesTreino;
                std::cout<<e+1<<"/"<<epochs<<" | "<<"accuracy: "<<accuracy<<std::endl;
            }
        }
        accuracy=0;
        while (entradas && saidas && cont<numeroDadosTreinamento+numeroDadosTestesTreino+numeroDadosTestes) {
            std::getline (entradas, Sentradas);
            std::getline (saidasEsperadas, SsaidasEsperadas);
            if(Sentradas.size()!=0 || SsaidasEsperadas.size()!=0){
                cont++;
                
                Matriz *Mentradas(separa_dados(Sentradas));
                Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                if(RealizarTeste(*Mentradas, *MsaidasEsperadas))
                    accuracy++;                

                delete Mentradas;
                delete MsaidasEsperadas;            
            }   
        }
        std::cout<<"Accuracy testes: "<<accuracy/numeroDadosTestes<<std::endl;

        entradas.close();
        saidasEsperadas.close();
    }
    else
        std::cout<<"erro ao abrir arquivos"<<std::endl;
    return cont;
}

#else
float RedeNeural::TreinarRede(Matriz entradas, Matriz saidas_esperadas, int quantidadePercentualTestes){
    int numeroAcertos=0;
    float porcentagemAcertos;

    return porcentagemAcertos;
}
#endif

void RedeNeural::AlteraFuncaoAtivacao(const char *F, int L, bool iniciando){
    if(!iniciando){
        free(fun_layers[L]);
        free(Dfun_layers[L]);
    }
    int T=0;
    while(F[T]!='\0')
        T++;

    fun_layers[L]=(char* )malloc((T+1)*sizeof(char));
    Dfun_layers[L]=(char* )malloc((T+2)*sizeof(char));

    Dfun_layers[L][0]='D';
    for(int i=0;i<T+1;i++){
        fun_layers[L][i]=F[i];
        Dfun_layers[L][i+1]=F[i];
    }
    fun_layers[L][T]='\0';
    Dfun_layers[L][T+1]='\0';
}


bool RedeNeural::RealizarTeste(const Matriz entrada, const Matriz saidaEsperada){
    FeedFoward(entrada);
    saidas[index_resposta]->binary();

    return verificar_igualdade(saidas[index_resposta], &saidaEsperada);
}

//----------------------FUNCOES AUXILIARES---------------------//

int memoria_alocada(const RedeNeural *NN){
    int bytes=0;

    //atributos rede
    bytes+=4*(sizeof(float));
    bytes+=(NN->numero_layers)*(sizeof(int)); //tam_layers
    bytes+=(NN->numero_layers-1)*(sizeof(int)); //fun_layers

    for(int i=0; i<NN->numero_layers-1; i++)
        bytes+=8*(sizeof(float));   //=numero_de_matrizes*ziseof(linhas+colunas) 
                                    //matrizes
    for(int i=0;i<NN->numero_layers-1;i++){
        bytes+=(NN->weights_layers[i]->linhas*NN->weights_layers[i]->colunas)*(sizeof(float));
        bytes+=(NN->bias_layers[i]->linhas*NN->bias_layers[i]->colunas)*(sizeof(float));
        bytes+=(NN->erros[i]->linhas*NN->erros[i]->colunas)*(sizeof(float));
        bytes+=(NN->saidas[i]->linhas*NN->saidas[i]->colunas)*(sizeof(float));
    }

    return bytes;
}

#if defined __linux__
int contagem_dados(const std::string caminho){
	int cont=0;
	std::fstream arquivo;
	std::string linha;

    arquivo.open(caminho); 

	do{
		getline(arquivo,linha);
		cont++;
	}while(arquivo);

	return cont-1;
} 

Matriz* separa_dados(const std::string linha){
	std::istringstream iss(linha);
	std::string valor;
    float *dados=(float*)malloc(sizeof(float));
	int i = 0;

	while (std::getline(iss, valor, ' ')) {
        if(i!=0)		
            dados=(float*)realloc(dados, (i+1)*sizeof(float));
        dados[i]=(std::stof(valor));
		i++;
	}
	Matriz* coleta=new Matriz(i, 1);

	for(int j=0;j<i;j++)
		coleta->matriz[j][0]=dados[j];

    free(dados);

	return coleta;
}

#endif
