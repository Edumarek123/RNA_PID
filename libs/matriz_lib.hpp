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

struct Matriz{
    int linhas;
    int colunas;
    float **matriz;

    Matriz();
    Matriz(int lin, int col); //alterar carrega_valores()
    Matriz(const Matriz &m);
    ~Matriz();

    void carrega_valores(float n, bool aleatorio);
    void imprimir();

    void somar(const Matriz *m);
    void multiplicar_Hadamard(const Matriz *m);
    void map(const char *fun_type);
    bool verificar_igualdade(const Matriz *m);
    void softmax();
    void binary();
};

Matriz subtrair(const Matriz *A, const Matriz *B);
Matriz multiplicar_escalar(const Matriz *m, float n);
Matriz multiplicar(const Matriz *A, const Matriz *B);
Matriz transposta(const Matriz *L);
Matriz map(const Matriz *m, const char *fun_type);
bool verificar_igualdade(const Matriz *A, const Matriz *B);
Matriz softmax(const Matriz *m);
Matriz binary(const Matriz *m);

bool s_iguais(const char *A, const char *B);
float fun_ction(float x, const char *fun_type);

#endif //MATRIZ_LIB_HPP
