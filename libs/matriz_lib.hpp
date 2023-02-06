//
// Created by Edumarek on 21/06/2021.
//

#ifndef MATRIZ_LIB_HPP
#define MATRIZ_LIB_HPP

#include <math.h>
#include <assert.h>
#if defined __linux__
    #include<iostream>
#else
    #include<Arduino.h>
#endif

typedef double _ponto;
typedef _ponto (*funType)(_ponto);

struct Matriz{
    size_t linhas;
    size_t colunas;
    _ponto **matriz;

    Matriz();
    Matriz(size_t lin, size_t col); //alterar carrega_valores()
    Matriz(const Matriz &m);
    ~Matriz();

    void carrega_valores(const _ponto n, bool aleatorio);
    void imprimir();

    void somar(const Matriz *m);
    void multiplicar_Hadamard(const Matriz *m);
    void multiplicar_escalar(const _ponto n);

    void map(const funType f);
    bool verificar_igualdade(const Matriz *m);
    void softmax();
    void binary();
};

Matriz subtrair(const Matriz *A, const Matriz *B);
Matriz multiplicar_escalar(const Matriz *m,const _ponto n);
Matriz multiplicar(const Matriz *A, const Matriz *B);
Matriz transposta(const Matriz *L);
Matriz map(const Matriz *m, const funType f);
bool verificar_igualdade(const Matriz *A, const Matriz *B);
Matriz softmax(const Matriz *m);
Matriz binary(const Matriz *m);
_ponto loss(const Matriz *A, const Matriz *B);

_ponto sigmoide(_ponto x);
_ponto Dsigmoide(_ponto x);
_ponto ReLU(_ponto x);
_ponto DReLU(_ponto x);
_ponto Tanh(_ponto x);
_ponto DTanh(_ponto x);
_ponto seno(_ponto x);
    
#endif //MATRIZ_LIB_HPP
