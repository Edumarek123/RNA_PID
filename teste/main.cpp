#include <iostream>
#include <bits/stdc++.h>

#include "../libs/matriz_lib.hpp"
#include "../libs/rede_neural_lib.hpp"


int main() {
  float LEARNING_RATE=0.5;
  int tam_Lx=2, tam_Ly=1;
  int tams_Lh[2]={2, 2};

  clock_t start, end;

  RedeNeural* NN=new RedeNeural(tam_Lx, tam_Ly, tams_Lh, (sizeof(tams_Lh)/sizeof(tams_Lh[0])), LEARNING_RATE);

  Matriz m(2, 1);
  Matriz n(1, 1);

  start = clock();

  NN->FeedFoward(m);
  NN->Calcula_Erros(n);
  NN->BackPropagation(m);

  end = clock();

  // Calculating total time taken by the program.
  double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
  std::cout << "Time taken by program is : " << std::fixed
       << time_taken << std::setprecision(5);
  std::cout << " sec " << std::endl;

  delete NN;

  return 0;
}
