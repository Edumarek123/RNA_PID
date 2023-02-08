 //
// Created by Edumarek on 27/06/2021.
//

#include "rede_neural_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//

RedeNeural::RedeNeural(int tam_Lx, int tam_Ly, int *tams_hl, int num_hl, _ponto alfa_learn){
    tam_layer_x = tam_Lx;
    tam_layer_y = tam_Ly;
    numero_layers = num_hl + 2;
    index_resposta = numero_layers - 2;

    LEARNING_RATE = alfa_learn;

    tam_layers = (int* )malloc(numero_layers * sizeof(int));
    tam_layers[0] = tam_layer_x;

    for(int i = 1; i < numero_layers - 1; i++)
        tam_layers[i] = tams_hl[i-1];
    tam_layers[numero_layers - 1] = tam_layer_y;

    fun_layers = (funType* )malloc((numero_layers - 1) * sizeof(funType));
    Dfun_layers = (funType* )malloc((numero_layers - 1) * sizeof(funType));

    for(int i = 0; i < numero_layers - 1; i++)
        AlteraFuncaoAtivacao("sigmoide", i);

    bias_layers = (Matriz** )malloc((numero_layers - 1) * sizeof(Matriz*));
    for(int i = 0; i < numero_layers - 1; i++)
        bias_layers[i] = new Matriz(tam_layers[i + 1], 1);

    weights_layers = (Matriz** )malloc((numero_layers - 1) * sizeof(Matriz*));
    for(int i = 0; i < numero_layers - 1; i++)
        weights_layers[i] = new Matriz(tam_layers[i + 1], tam_layers[i]);

    erros = (Matriz** )malloc((numero_layers - 1) * sizeof(Matriz*));
    for(int i = 0; i < numero_layers - 1; i++)
        erros[i] = new Matriz(tam_layers[i + 1], 1);

    saidas = (Matriz** )malloc((numero_layers - 1) * sizeof(Matriz*));
    for(int i = 0; i < numero_layers - 1; i++)
        saidas[i] = new Matriz(tam_layers[i + 1], 1);

    bytesAlocados = memoria_alocada();
}

RedeNeural::~RedeNeural(){
    for(int i = 0; i < numero_layers - 1; i++){
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
    free(fun_layers);
    free(Dfun_layers);
}

//---------------------------METODOS---------------------------//

#if defined __linux__
void RedeNeural::imprimir_rede() {
    std::cout << std::endl << "Neural Network Structure( I | Wn | O )" << std::endl << std::endl;

    std::cout<<"Tamanho layers"<<std::endl<<std::endl;
    std::cout<<"|  X  | ";
    for(int i = 0; i < numero_layers - 2; i++)
        std::cout<<"HL"<<i<<" | ";
    std::cout<<" Y  |"<<std::endl<<"| ";
    for(int i = 0; i < numero_layers; i++)
        std::cout<<" "<<tam_layers[i]<<"  | ";
    std::cout<<std::endl<<std::endl;

    std::cout<<"Vizualizacao | Entradas | Pesos | Saidas |"<<std::endl<<std::endl;
    for (int i = 0; i < tam_layer_x; i++)
        std::cout<<"X"<<std::endl;
    std::cout << std::endl;

    for (int i = 0; i < numero_layers - 1; i++) {
        weights_layers[i]->imprimir();
        std::cout << std::endl;
    }

    for (int i = 0; i < tam_layer_y; i++)
        std::cout<<"Y"<<std::endl;
    std::cout << std::endl;

    std::cout << std::endl<< "Bias" << std::endl << std::endl;
    for (int i = 0; i < numero_layers - 1; i++) {
        bias_layers[i]->imprimir();
        std::cout << std::endl;
    }
}

#else
void RedeNeural::imprimir_rede() {
    Serial.print("\nNeural Network Structure( I | Wn | O )\n\n");

    Serial.print("Tamanho layers\n\n");
    Serial.print("| X |");
    for(int i = 0; i < numero_layers - 2; i++){
        Serial.print("HL");
        Serial.print(i);
        Serial.print(" | ");

    }

    Serial.print(" Y |\n| ");
    for(int i = 0; i < numero_layers; i++){
        Serial.print(" ");
        Serial.print(tam_layers[i]);
        Serial.print("  | ");
    }
    Serial.print("\n\n");

    Serial.print("Vizualizacao | Entradas | Pesos | Saidas |\n\n");
    for (int i = 0; i < tam_layer_x; i++)
        Serial.print("X\n");
    Serial.print("\n");

    for (int i = 0; i < numero_layers - 1; i++) {
        weights_layers[i]->imprimir();
        Serial.print("\n");
    }

    for (int i = 0; i < tam_layer_y; i++)
        Serial.print("Y\n");
    Serial.print("\n");

    Serial.print("\nBias\n\n");
    for (int i = 0; i < numero_layers - 1; i++) {
        bias_layers[i]->imprimir();
        Serial.print("\n");
    }
}
#endif

void RedeNeural::carregar_rede(_ponto *p){
    long int cont = 0;

    for (int i = 0; i < numero_layers - 1; i++)
        for (int j = 0; j < weights_layers[i]->linhas; j++)
            for (int k = 0; k < weights_layers[i]->colunas; k++){
                weights_layers[i]->matriz[j][k] = p[cont];
                cont++;
            }

    for (int i = 0; i < numero_layers - 1; i++)
        for (int j = 0; j < bias_layers[i]->linhas; j++)
            for (int k = 0; k < bias_layers[i]->colunas; k++){
                bias_layers[i]->matriz[j][k] = p[cont];
                cont++;
            }
}

#if defined __linux__
void RedeNeural::salvar_rede(const char *s){
    std::ofstream arquivo(s, std::ofstream::out);

    if (arquivo.is_open()){
        arquivo<<"#ifndef REDE_SALVA_HPP\n#define REDE_SALVA_HPP\n\n";
        arquivo<<"#include <matriz_lib.hpp>\n\n";
        arquivo<<"_ponto PESOS_REDE_SALVA[] = {";

        for (int i = 0; i < numero_layers - 1; i++)
            for (int j = 0; j < weights_layers[i]->linhas; j++)
                for (int k = 0; k < weights_layers[i]->colunas; k++)
                    arquivo<<weights_layers[i]->matriz[j][k]<<", ";

        for (int i = 0; i < numero_layers - 1; i++)
            for (int j = 0; j < bias_layers[i]->linhas; j++)
                for (int k = 0; k < bias_layers[i]->colunas; k++)
                    if(i == numero_layers - 2 && j == bias_layers[i]->linhas - 1 && k == bias_layers[i]->colunas - 1)
                        arquivo<<bias_layers[i]->matriz[j][k];
                    else
                        arquivo<<bias_layers[i]->matriz[j][k]<<", ";

        arquivo<<"};\n\n";
        arquivo<<"#endif";
    }
    arquivo.close();
}
#endif

int RedeNeural::memoria_alocada(){
    int bytes = 0;

    return bytes;
}

void RedeNeural::FeedFoward(Matriz entradas){
    //z_l=W_l.X+B
    //z_l=W_l.a_(l-1)+B
    //a_l=6z_l

    for (int i = 0; i < numero_layers - 1; i++) delete saidas[i];
    saidas[0] = new Matriz(multiplicar(weights_layers[0], &entradas));
    saidas[0]->somar(bias_layers[0]);
    saidas[0]->map(fun_layers[0]); //"f"

    for (int i = 1; i < numero_layers - 1; i++) {
        saidas[i] = new Matriz(multiplicar(weights_layers[i], saidas[i - 1]));
        saidas[i]->somar(bias_layers[i]);
        saidas[i]->map(fun_layers[i]); //"f"
    }
}

void RedeNeural::CalculaErros(Matriz saidas_esperadas) {
    //e_L = (A_L - Y) 0 d6(z_L)
    //e_l = ((W_(l+1))T(erro_(l+1))) 0 d6(z_l)

    delete erros[numero_layers - 2];
    erros[numero_layers - 2] = new Matriz(subtrair(&saidas_esperadas, saidas[numero_layers - 2]));
    Matriz aux1(map(saidas[numero_layers - 2], Dfun_layers[numero_layers - 2])); //"Df"
    erros[numero_layers - 2]->multiplicar_Hadamard(&aux1); 

    for (int i = numero_layers - 3; i >= 0; i--){
        delete erros[i];
        Matriz aux2(transposta(weights_layers[i + 1]));
        erros[i] = new Matriz(multiplicar(&aux2, erros[i + 1]));
        Matriz aux3(map(saidas[i], Dfun_layers[i])); //"Df"
        erros[i]->multiplicar_Hadamard(&aux3); 
    }
}

void RedeNeural::BackPropagation(Matriz entradas){
    //∇Cw_l = a_(l-1) * e_l
    //∇Cb_l = e_l

    for(int i = numero_layers - 2; i >= 1; i--){
        Matriz aux1(transposta(saidas[i - 1]));
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
    int numeroDados = contagem_dados(Pentradas);
    int numeroDadosTestes = (int)((numeroDados * quantidadePercentualTestes) / 100); //x% para testes ao final
    int numeroDadosTestesTreino = (int)(numeroDados * 0.05); //5% para testes durante treinamento
    int numeroDadosTreinamento = numeroDados - numeroDadosTestes - numeroDadosTestesTreino; //o restante para treinamento
    _ponto limiteLoss = 0.008;

    int cont = 0;
    float accuracy = 0;
    _ponto _loss = 1;
    _ponto _val_loss = 1;
    std::string Sentradas, SsaidasEsperadas;
    if (entradas.is_open() && saidasEsperadas.is_open()) {
        for(int e = 0;e < epochs; e++){
            if(accuracy < 1){
                entradas.clear();
                saidasEsperadas.clear();
                entradas.seekg(0);  
                saidasEsperadas.seekg(0);  

                cont = 0;
                _loss = 0;
                while (entradas && saidas && cont < numeroDadosTreinamento) {
                    std::getline (entradas, Sentradas);
                    std::getline (saidasEsperadas, SsaidasEsperadas);
                    if(Sentradas.size() != 0 || SsaidasEsperadas.size() != 0){
                        cont++;
            
                        Matriz *Mentradas(separa_dados(Sentradas));
                        Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                        FeedFoward(*Mentradas);
                        CalculaErros(*MsaidasEsperadas);
                        BackPropagation(*Mentradas);
                        _loss += loss(MsaidasEsperadas, saidas[index_resposta]);                            

                        delete Mentradas;
                        delete MsaidasEsperadas;
                    }
                }
                _loss /= numeroDadosTreinamento;

                accuracy = 0;
                _val_loss = 0;
                while (entradas && saidas && cont < numeroDadosTreinamento + numeroDadosTestesTreino) {
                    std::getline (entradas, Sentradas);
                    std::getline (saidasEsperadas, SsaidasEsperadas);
                    if(Sentradas.size() != 0 || SsaidasEsperadas.size() != 0){
                        cont++;
                        
                        Matriz *Mentradas(separa_dados(Sentradas));
                        Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                        FeedFoward(*Mentradas);

                        _ponto aux_loss = loss(MsaidasEsperadas, saidas[index_resposta]);
                        if(aux_loss < limiteLoss)
                            accuracy++;                
                        _val_loss += aux_loss;

                        // std::cout<<"---saidasEsperadas---"<<std::endl;
                        // MsaidasEsperadas->imprimir();
                        // std::cout<<"---saidas---"<<std::endl;
                        // saidas[index_resposta]->imprimir();
                        // std::cout<<std::endl;
                        // std::cout<<std::endl;

                        delete Mentradas;
                        delete MsaidasEsperadas;
                    }   
                }
                accuracy /= numeroDadosTestesTreino;
                _val_loss /= numeroDadosTestesTreino;
                std::cout<<e + 1<<"/"<<epochs<<" | accuracy: "<<accuracy<<" | loss: "<<_loss<<" | val_loss: "<<_val_loss<<std::endl;
            }
        }
        accuracy = 0;
        _val_loss = 0;
        if(numeroDadosTestes > 0){
            while(entradas && saidas && cont < numeroDadosTreinamento + numeroDadosTestesTreino + numeroDadosTestes) {
                std::getline (entradas, Sentradas);
                std::getline (saidasEsperadas, SsaidasEsperadas);
                if(Sentradas.size() != 0 || SsaidasEsperadas.size() != 0){
                    cont++;
                    
                    Matriz *Mentradas(separa_dados(Sentradas));
                    Matriz *MsaidasEsperadas(separa_dados(SsaidasEsperadas));

                    FeedFoward(*Mentradas);

                    _ponto aux_loss = loss(MsaidasEsperadas, saidas[index_resposta]);
                    if(aux_loss < limiteLoss)
                        accuracy++;                
                    _val_loss += aux_loss;

                    delete Mentradas;
                    delete MsaidasEsperadas;            
                }   
            }
            accuracy /= numeroDadosTestes;
            _val_loss /= numeroDadosTestes;
            std::cout<<"Accuracy testes: "<<accuracy<<" | val_loss: "<<_val_loss<<std::endl;
        }
        
        entradas.close();
        saidasEsperadas.close();
    }
    else
        std::cout<<"erro ao abrir arquivos"<<std::endl;
    return cont;
}

#else
float RedeNeural::TreinarRede(const Matriz *entradas,const Matriz *saidasEsperadas, const int numeroDados, const float quantidadePercentualTestes, const int epochs){
    int numeroDadosTestes = (int)((numeroDados * quantidadePercentualTestes) / 100); //x% para testes ao final
    int numeroDadosTestesTreino = (int)(numeroDados * 0.05); //5% para testes durante treinamento
    int numeroDadosTreinamento = numeroDados - numeroDadosTestes - numeroDadosTestesTreino; //o restante para treinamento

    int cont = 0;
    float accuracy = 0;
    float _loss = 1;
    for(int e=0;e<epochs;e++){
        if((accuracy < 1 && _loss > 0.001) || numeroDados < 50){
        cont = 0;
        accuracy = 0;
        _loss = 0;
        while (cont < numeroDadosTreinamento) {
            FeedFoward(entradas[cont]);
            CalculaErros(saidasEsperadas[cont]);
            BackPropagation(entradas[cont]);
            cont++;
        }

        while (cont < numeroDadosTreinamento + numeroDadosTestesTreino) {
        //     if(RealizarTeste(entradas[cont], saidasEsperadas[cont]))
        //         accuracy++;

        //     _loss+=loss(saidasEsperadas[cont], saidas[index_resposta]);
        //     cont++;
        }
        accuracy /= numeroDadosTestesTreino;
        _loss /= numeroDadosTestesTreino;
        Serial.println(String(e + 1) + "/"+String(epochs) + " | accuracy: " + String(accuracy)+" | val_loss: " + String(_loss));
        }
    }
    accuracy = 0;
    _loss = 0;
    if(numeroDadosTestes > 0){
        while(cont < numeroDadosTreinamento + numeroDadosTestesTreino + numeroDadosTestes) {
            // if(RealizarTeste(entradas[cont], saidasEsperadas[cont]))
            //     accuracy++;                

            // _loss+=loss(saidasEsperadas[cont], saidas[index_resposta]);

            // cont++;
        }
        accuracy /= numeroDadosTestes;
        _loss /= numeroDadosTestes;
        Serial.println("Accuracy testes: " + String(accuracy / numeroDadosTestes) + " | loss: " + String(_loss));
    }
    return cont;
}
#endif

void RedeNeural::AlteraFuncaoAtivacao(const char *F, int L){
    if(s_iguais(F, "sigmoide")){
        fun_layers[L] = sigmoide;
        Dfun_layers[L] = Dsigmoide;
    }
    else if(s_iguais(F, "ReLU")){
        fun_layers[L] = ReLU;
        Dfun_layers[L] = DReLU;
    }
    else if(s_iguais(F, "tanh")){
        fun_layers[L] = Tanh;
        Dfun_layers[L] = DTanh;
    }
    else
      assert(false);
}


bool RedeNeural::RealizarTeste(const Matriz *entradas, const Matriz *saidasEsperadas){
    FeedFoward(*entradas);

    // saidas[index_resposta]->binary();

    if(loss(saidasEsperadas, saidas[index_resposta]) < 0.008)
        return true;
    return false;
}

//----------------------FUNCOES AUXILIARES---------------------//
bool s_iguais(const char *A, const char *B){
    int i=0;

    while(A[i] != '\0' || B[i] != '\0'){
        if(A[i] != B[i])
            return false;
        i++;
    }

    return true;
}

#if defined __linux__
int contagem_dados(const std::string caminho){
	int cont = 0;
	std::fstream arquivo;
	std::string linha;

    arquivo.open(caminho); 

	do{
		getline(arquivo,linha);
		cont++;
	}while(arquivo);

	return cont - 1;
} 

Matriz* separa_dados(const std::string linha){
	std::istringstream iss(linha);
	std::string valor;
    _ponto *dados = (_ponto*)malloc(sizeof(_ponto));
	int i = 0;

	while (std::getline(iss, valor, ' ')) {
        if(i != 0)		
            dados = (_ponto*)realloc(dados, (i + 1) * sizeof(_ponto));
        dados[i] = (std::stof(valor));
		i++;
	}
	Matriz* coleta = new Matriz(i, 1);

	for(int j = 0; j < i; j++)
		coleta->matriz[j][0] = dados[j];

    free(dados);

	return coleta;
}
#endif
