#include <iostream>
#include "../libs/matriz_lib.hpp"
#include "../libs/rede_neural_lib.hpp"


int main() {
  float LEARNING_RATE=0.5;
  int tam_Lx=2, tam_Ly=1;
  int tams_Lh[2]={2, 2};

  RedeNeural* NN=new RedeNeural(tam_Lx, tam_Ly, tams_Lh, (sizeof(tams_Lh)/sizeof(tams_Lh[0])), LEARNING_RATE);

  Matriz m(2, 1);
  Matriz n(1, 1);

  NN->FeedFoward(m);
  NN->Calcula_Erros(n);
  NN->BackPropagation(m);

  delete NN;

  return 0;
}
