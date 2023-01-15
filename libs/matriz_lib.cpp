//
// Created by Edumarek on 26/06/2021.
//
#include "matriz_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//

Matriz::Matriz(){
  linhas=0;
  colunas=0;
  matriz=NULL;
}

Matriz::Matriz(int lin, int col){
    linhas=lin;
    colunas=col;

    matriz=(float**)malloc(linhas*sizeof (float*));

    for (int i = 0; i < linhas; i++)
        matriz[i] = (float *) malloc(colunas * sizeof(float));
    carrega_valores(0, true);
}

Matriz::Matriz(const Matriz &m){
  linhas=m.linhas;
  colunas=m.colunas;
  matriz=(float**)malloc(linhas*sizeof (float*));

  for (int i = 0; i < linhas; i++)
      matriz[i] = (float *) malloc(colunas * sizeof(float));

  for(int i=0;i<linhas;i++)
    for(int j=0;j<colunas;j++)
      matriz[i][j]=m.matriz[i][j];
}

Matriz::~Matriz(){
   for(int i=0;i<linhas;i++)
       free(matriz[i]);
   free(matriz);
}

//---------------------------METODOS---------------------------//

void Matriz::carrega_valores(float n, bool aleatorio){
    if(aleatorio)
        for(int i=0;i<linhas;i++)
            for(int j=0;j<colunas;j++)
                do{matriz[i][j]=((float)(rand()%21-10)/10);}while(matriz[i][j]==0);
    else
        for(int i=0;i<linhas;i++)
            for(int j=0;j<colunas;j++)
                matriz[i][j]=n;
}

#if defined __linux__
void Matriz::imprimir(){
    for(int i=0;i<linhas;i++){
        for(int j=0;j<colunas;j++)
            std::cout<< matriz[i][j] << " ";
        std::cout<<std::endl;
    }
}
#else
void Matriz::imprimir(){
    for(int i=0;i<linhas;i++){
        for(int j=0;j<colunas;j++)
            Serial.print(String(matriz[i][j])+" ");
        Serial.println("");
    }
}
#endif

void Matriz::somar(const Matriz *m){
    assert(linhas==m->linhas && colunas==m->colunas);

    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++)
            matriz[i][j]+=m->matriz[i][j];
}

void Matriz::multiplicar_Hadamard(const Matriz *m){
    assert(linhas==m->linhas && colunas==m->colunas);

    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++)
            matriz[i][j]*=m->matriz[i][j];
}

void Matriz::map(const char *fun_type){
    float value;

    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++){
            value=matriz[i][j];
            matriz[i][j]=fun_ction(value, fun_type);
        }
}

bool Matriz::verificar_igualdade(const Matriz *m){
    assert(linhas==m->linhas && colunas==m->colunas);

    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++)
            if(matriz[i][j]!=m->matriz[i][j])
              return(false);
    return(true);
}

void Matriz::softmax(){
    float soma=0;
    if(linhas==1){
        for(int i=0;i<colunas;i++)
            soma+=exp(matriz[0][i]);
        
        for(int i=0;i<colunas;i++)
            matriz[0][i]=exp(matriz[0][i])/soma;
    }
    else if(colunas==1){
        for(int i=0;i<linhas;i++)
            soma+=exp(matriz[i][0]);
        
        for(int i=0;i<linhas;i++)
            matriz[i][0]=exp(matriz[i][0])/soma;
    }
    else
        assert(1==0);
}

void Matriz::binary(){   
    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++){
            if(matriz[i][j]>0.5)
                matriz[i][j]=1;
            else
                matriz[i][j]=0;
        }
}

//---------------------------FUNCOES---------------------------//

Matriz subtrair(const Matriz *A, const Matriz *B){
    assert(A->linhas==B->linhas && A->colunas==B->colunas);
    Matriz resultado(A->linhas, A->colunas);

    for(int i=0;i<A->linhas;i++)
        for(int j=0;j<A->colunas;j++)
            resultado.matriz[i][j]=A->matriz[i][j]-B->matriz[i][j];
    return (resultado);
}

Matriz multiplicar_escalar(const Matriz *m, float n){
    Matriz resultado(m->linhas, m->colunas);

    for(int i=0;i<m->linhas;i++)
        for(int j=0;j<m->colunas;j++)
            resultado.matriz[i][j]=m->matriz[i][j]*n;
    return (resultado);
}

Matriz multiplicar(const Matriz *A, const Matriz *B){
    assert(A->colunas==B->linhas);
    Matriz resultado(A->linhas, B->colunas);
    float produto;

    for(int i=0;i<A->linhas;i++) {
        for (int j = 0; j < B->colunas; j++) {
            produto = 0;
            for(int k=0;k<A->colunas; k++)
                produto += (A->matriz[i][k] * B->matriz[k][j]);
            resultado.matriz[i][j] = produto;
        }
    }
    return(resultado);
}

Matriz transposta(const Matriz *L) {
    Matriz T(L->colunas, L->linhas);

    for(int i=0;i<L->colunas;i++)
        for(int j=0;j<L->linhas;j++)
            T.matriz[i][j]=L->matriz[j][i];
    return (T);
}

Matriz map(const Matriz *m , const char *fun_type){
    float value;
    Matriz resultado(m->linhas, m->colunas);

    for(int i=0;i<resultado.linhas;i++)
        for(int j=0;j<resultado.colunas;j++){
            value=m->matriz[i][j];
            resultado.matriz[i][j]=fun_ction(value, fun_type);
        }
    return resultado;
}

bool verificar_igualdade(const Matriz *A, const Matriz *B){
    assert(A->linhas==B->linhas && A->colunas==B->colunas);

    for(int i=0;i<A->linhas;i++)
        for(int j=0;j<A->colunas;j++)
            if(A->matriz[i][j]!=B->matriz[i][j])
                return(false);
    return(true);
}

Matriz softmax(const Matriz *m){
    Matriz resultado(m->linhas, m->colunas);
    
    float soma=0;
    if(m->linhas==1){
        for(int i=0;i<m->colunas;i++)
            soma+=exp(m->matriz[0][i]);
        
        for(int i=0;i<m->colunas;i++)
            resultado.matriz[0][i]=exp(m->matriz[0][i])/soma;
    }
    else if(m->colunas==1){
        for(int i=0;i<m->linhas;i++)
            soma+=exp(m->matriz[i][0]);
        
        for(int i=0;i<m->linhas;i++)
            resultado.matriz[i][0]=exp(m->matriz[i][0])/soma;
    }
    else
        assert(1==0);

    return resultado;
}

Matriz binary(const Matriz *m){   
    Matriz resultado(m->linhas, m->colunas);

    for(int i=0;i<m->linhas;i++)
        for(int j=0;j<m->colunas;j++){
            if(m->matriz[i][j]>0.5)
                resultado.matriz[i][j]=1;
            else
                resultado.matriz[i][j]=0;
        }

    return resultado;
}

//----------------------FUNCOES AUXILIARES---------------------//

bool s_iguais(const char *A, const char *B){
    int i=0;

    while(A[i]!='\0' || B[i]!='\0'){
        if(A[i]!=B[i])
            return false;
        i++;
    }

    return true;
}

float fun_ction(float x, const char *fun_type){
    float y;

    if(s_iguais(fun_type, "sigmoide")){
      y=(1/(1+exp(-x)));
    }
    else if(s_iguais((const char* )fun_type, "Dsigmoide")){
      y=x*(1-x);
    }
    else if(s_iguais(fun_type, "ReLU")){
        if(x>0)
            y=x;
        else
            y=0;
    }
    else if(s_iguais(fun_type, "DReLU")){
        if(x>0)
            y=1;
        else
            y=0;
    }
    else if(s_iguais(fun_type, "tanh")){
      y=tanh(x);
    }
    else if(s_iguais(fun_type, "Dtanh")){
      y=1-(tanh(x)*tanh(x));
    }
    else
      assert(1==0);

    return(y);
}
