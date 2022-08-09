#include <iostream>
#include "../matriz_lib.hpp"
#include "../rede_neural_lib.hpp"

float v[15]={0.697166, -0.946053, -0.676279, 0.240732, -0.776667, -0.633455, 0.306689, 0.893748, 0.776167, -0.316701, 0.74726, -0.0425729, 0.661536, 0.115643, -0.154432};

int main() {
  float LEARNING_RATE=0.5;
  int tam_Lx=2, tam_Ly=1;
  int tams_Lh[2]={2, 2};

  RedeNeural* NN=new RedeNeural(tam_Lx, tam_Ly, tams_Lh, (sizeof(tams_Lh)/sizeof(tams_Lh[0])), LEARNING_RATE);
  NN->carregar_rede2(v);

  delete NN;

  return 0;
}
