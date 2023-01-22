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

    matriz=(_ponto**)malloc(linhas*sizeof (_ponto*));

    for (int i = 0; i < linhas; i++)
        matriz[i] = (_ponto *) malloc(colunas * sizeof(_ponto));
    carrega_valores(1, true);
}

Matriz::Matriz(const Matriz &m){
  linhas=m.linhas;
  colunas=m.colunas;
  matriz=(_ponto**)malloc(linhas*sizeof (_ponto*));

  for (int i = 0; i < linhas; i++)
      matriz[i] = (_ponto *) malloc(colunas * sizeof(_ponto));

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

void Matriz::carrega_valores(const _ponto n, bool aleatorio){
    if(aleatorio)
        for(int i=0;i<linhas;i++)
            for(int j=0;j<colunas;j++)
                do{matriz[i][j]=((_ponto)(rand()%21-10)/10);}while(matriz[i][j]==0);
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

void Matriz::multiplicar_escalar(const _ponto n){
    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++)
            matriz[i][j]*=n;
}

void Matriz::map(const funType f){
    _ponto value;

    for(int i=0;i<linhas;i++)
        for(int j=0;j<colunas;j++){
            value=matriz[i][j];
            matriz[i][j]=f(value);
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
    _ponto soma=0;
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

Matriz multiplicar_escalar(const Matriz *m, const _ponto n){
    Matriz resultado(m->linhas, m->colunas);

    for(int i=0;i<m->linhas;i++)
        for(int j=0;j<m->colunas;j++)
            resultado.matriz[i][j]=m->matriz[i][j]*n;
    return (resultado);
}

Matriz multiplicar(const Matriz *A, const Matriz *B){
    assert(A->colunas==B->linhas);
    Matriz resultado(A->linhas, B->colunas);
    _ponto produto;

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

Matriz map(const Matriz *m , const funType f){
    _ponto value;
    Matriz resultado(m->linhas, m->colunas);

    for(int i=0;i<resultado.linhas;i++)
        for(int j=0;j<resultado.colunas;j++){
            value=m->matriz[i][j];
            resultado.matriz[i][j]=f(value);
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
    
    _ponto soma=0;
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

_ponto loss(const Matriz *A, const Matriz *B){
    _ponto resultado=0;

    Matriz L(subtrair(A, B));

    for(int i=0;i<L.linhas;i++)
        for(int j=0;j<L.colunas;j++)
            resultado+=L.matriz[i][j];
    resultado/=(L.linhas*L.colunas);

    return resultado*resultado;
}

//----------------------FUNCOES DE ATIVACAO--------------------//
_ponto sigmoide(_ponto x){
    return 1/(1+exp(-x));
}

_ponto Dsigmoide(_ponto x){
    return x*(1-x);
}

_ponto ReLU(_ponto x){
    if(x>0)
        return x;
    else
        return 0;
}

_ponto DReLU(_ponto x){
    if(x>0)
        return 1;
    else
        return 0;
}

_ponto Tanh(_ponto x){
    return tanh(x);
}

_ponto DTanh(_ponto x){
    return 1-(tanh(x)*tanh(x));
}


//----------------------FUNCOES AUXILIARES---------------------//