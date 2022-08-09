#include <avr/io.h>
#include <util/delay.h>
#include <matriz_lib.hpp>
#include <rede_neural_lib.hpp>

//float v[15]={0.697166, -0.946053, -0.676279, 0.240732, -0.776667, -0.633455, 0.306689, 0.893748, 0.776167, -0.316701, 0.74726, -0.0425729, 0.661536, 0.115643, -0.154432};

int main()
{
  Serial.begin(115200);
  
  float LEARNING_RATE=0.5;
  int tam_Lx=1, tam_Ly=1;
  int tams_Lh[1]={1};

  RedeNeural* NN=new RedeNeural(tam_Lx, tam_Ly, tams_Lh, (sizeof(tams_Lh)/sizeof(tams_Lh[0])), LEARNING_RATE);
//  NN->carregar_rede2(v);


  //NN->weights_layers[0]->carrega_valores(1, false);
  for(int i=0; i<NN->weights_layers[0]->linhas;i++){
    for(int j=0;j<NN->weights_layers[0]->colunas;j++){
      Serial.print(NN->weights_layers[0]->matriz[i][j]);
      Serial.print(" ");  
    }
    Serial.print("\n");
  }
//  delete NN;

  return 0;
}
