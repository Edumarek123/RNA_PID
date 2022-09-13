//
// Created by Edumarek on 21/06/2021.
//

#ifndef MATRIZ_LIB_HPP
#define MATRIZ_LIB_HPP


struct Matriz{
    int linhas;
    int colunas;
    float **matriz;

    Matriz();
    Matriz(int lin, int col);
    Matriz(const Matriz &m);
    ~Matriz();

    void carrega_valores(float n, bool aleatorio);
    void imprimir();

    void somar(Matriz m);
    void multiplicar_Hadamard(Matriz m);
    void map(const int fun_type);
    bool vericar_igualdade(Matriz m);
};

Matriz subtrair(Matriz A, Matriz B);
Matriz multiplicar_escalar(Matriz m, float n);
Matriz multiplicar(Matriz A, Matriz B);
Matriz transposta(Matriz L);
Matriz map(Matriz m, const int fun_type);
bool vericar_igualdade(Matriz A, Matriz B);

float fun_ction(float x, const int fun_type);

#endif //MATRIZ_LIB_HPP
