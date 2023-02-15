//
// Created by Edumarek on 12/02/2022.
//

#include"PID_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//
ControladorPID::ControladorPID(_ponto tA, _ponto min_uK, _ponto max_uK){
    Ta = tA;
    min_Uk = min_uK;
    max_Uk = max_uK;

    int hl[] = {10};
    RR = new RedeNeural(8, 4, hl, sizeof(hl) / sizeof(int), 1); //entradas, saidas, camadas ocultas, numero de camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(_ponto kP, _ponto kD, _ponto kI, _ponto n, _ponto tA, _ponto min_uK, _ponto max_uK){
    Kp = kP;
    Kd = kD;
    Ki = kI;
    Ta = tA;
    N = n;
    min_Uk = min_uK;
    max_Uk = max_uK;

    int hl[] = {10};
    RR = new RedeNeural(8, 4, hl, sizeof(hl) / sizeof(int), 1); //entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c){
    Kp = c.Kp;
    Kd = c.Kd;
    Ki = c.Ki;
    N = c.N;
    Ta = c.Ta;
    min_Uk = c.min_Uk;
    max_Uk = c.max_Uk;

    limiteIteracoesAcaoControle = c.limiteIteracoesAcaoControle;

    RR = c.RR;
}

ControladorPID::~ControladorPID(){
    delete RR;

    free(delimitadorMaxX);
    free(delimitadorMinY);
    free(delimitadorMaxY);
}

//---------------------------METODOS---------------------------//

#if defined __linux__
void ControladorPID::Imprimir_Parametros(){
    std::cout<<"Kp: "<<Kp<<" | Kd: "<<Kd<<" | Ki: "<<Ki<<" | N: "<<N<<" | Ta: "<<Ta<<std::endl;

}
#else
void ControladorPID::Imprimir_Parametros(){
    Serial.println("Kp: "+String((double)Kp)+" | Kd: "+String((double)Kd)+" | Ki: "+String((double)Ki)+" | N: "+String((double)N));
}
#endif

void ControladorPID::CarregarDelimitadores(_ponto *p){
    delimitadorMinX = (_ponto* )malloc(RR->tam_layer_x * sizeof(_ponto));
    delimitadorMaxX = (_ponto* )malloc(RR->tam_layer_x * sizeof(_ponto));

    delimitadorMinY = (_ponto* )malloc(RR->tam_layer_y * sizeof(_ponto));
    delimitadorMaxY = (_ponto* )malloc(RR->tam_layer_y * sizeof(_ponto));

    int k = 0;
    for(int i = 0; i < RR->tam_layer_x; i++){
        delimitadorMinX[i] = p[k];
        k ++;
    }

    for(int i = 0; i < RR->tam_layer_y; i++){
        delimitadorMinY[i] = p[k];
        k ++;
    }

    for(int i = 0; i < RR->tam_layer_x; i++){
        delimitadorMaxX[i] = p[k];
        k ++;
    }

    for(int i = 0; i < RR->tam_layer_y; i++){
        delimitadorMaxY[i] = p[k];
        k ++;
    }
}

#if defined __linux__
_ponto ControladorPID::Calcula_Acao_Controle(_ponto referencia, _ponto saida){
    clock_t tInicio = clock();
    double a, b ,c, d, e;

    //Calcula erro
    Yk[0] = saida;
    Ek[0]  = referencia - saida;

    //Calcula acao de controle
    a = Kp * (1 + (N * Ta)) + (Ki * Ta) * (1 + (N * Ta)) + (Kd * N);
    b = -(Kp * (2 + (N * Ta)) + (Ki * Ta)+(2 * Kd * N));
    c = Kp + (Kd * N);
    d = 1 + (N * Ta);
    e = -(2 + (N * Ta));

    Uk[0] = (_ponto)((-e * Uk[1]) - (1 * Uk[2]) + (a * Ek[0]) + (b * Ek[1]) + (c * Ek[2])) / d; //by mateus

    //Limitador acao controle
    if(Uk[0] > max_Uk)
        Uk[0] = max_Uk;
    if(Uk[0] <= min_Uk)
        Uk[0] = min_Uk;

    //Atualiza Saidas
    Yk[1] = Yk[0];

    //Atualiza erros
    Ek[2] = Ek[1];
    Ek[1] = Ek[0];

    //Atualiza acoes de controle
    Uk[1] = Uk[0];
    Uk[2] = Uk[1];

    //Atualizacao parametros
    iteracoesAcaoControle++;
    if(iteracoesAcaoControle >= limiteIteracoesAcaoControle){
        iteracoesAcaoControle = 0;
        Atualiza_Parametros();
        Imprimir_Parametros();
    }

    if(calcularTempoExecucao)
        tExecucao = (unsigned long)((1000 * (tInicio - clock())) / CLOCKS_PER_SEC);

    return Uk[0];
}

#else
_ponto ControladorPID::Calcula_Acao_Controle(_ponto referencia, _ponto saida){
    unsigned long tInicio=micros();
    _ponto a, b ,c, d, e;

    //Calcula erro
    Yk[0] = saida;
    Ek[0]  = referencia - saida;

    //Calcula acao de controle
    a = Kp * (1 + (N * Ta)) + (Ki * Ta) * (1 + (N * Ta)) + (Kd * N);
    b = -(Kp * (2 + (N * Ta)) + (Ki * Ta)+(2 * Kd * N));
    c = Kp + (Kd * N);
    d = 1 + (N * Ta);
    e = -(2 + (N * Ta));

    Uk[0] = (_ponto)((-e * Uk[1]) - (1 * Uk[2]) + (a * Ek[0]) + (b * Ek[1]) + (c * Ek[2])) / d; //by mateus

    //Limitador acao controle
    if(Uk[0] > max_Uk)
        Uk[0] = max_Uk;
    if(Uk[0] <= min_Uk)
        Uk[0] = min_Uk;

    //Atualiza Saidas
    Yk[1] = Yk[0];

    //Atualiza erros
    Ek[2] = Ek[1];
    Ek[1] = Ek[0];

    //Atualiza acoes de controle
    Uk[1] = Uk[0];
    Uk[2] = Uk[1];

    //Atualizacao parametros
    iteracoesAcaoControle++;
    if(iteracoesAcaoControle >= limiteIteracoesAcaoControle && atualizarParametros){
        iteracoesAcaoControle = 0;
        Atualiza_Parametros();
        if(imprimirParametros)
            Imprimir_Parametros();
    }

    if(calcularTempoExecucao)
        tExecucao = micros() - tInicio;

    return Uk[0];
}
#endif

void ControladorPID::Atualiza_Parametros(){
    Matriz m(8, 1);

    m.matriz[0][0] = Uk[0];
    m.matriz[1][0] = Uk[1];
    m.matriz[2][0] = Uk[2];
    m.matriz[3][0] = Yk[0];
    m.matriz[4][0] = Yk[1];
    m.matriz[5][0] = Ek[0];
    m.matriz[6][0] = Ek[1];
    m.matriz[7][0] = Ek[2];

    for(int i = 0; i < m.linhas; i++)
        m.matriz[i][0] = (m.matriz[i][0] - delimitadorMinX[i]) / (delimitadorMaxX[i] - delimitadorMinX[i]);

    RR->FeedFoward(m);

    for(int i = 0;i < RR->saidas[RR->index_resposta]->linhas; i++){
        RR->saidas[RR->index_resposta]->matriz[i][0] = delimitadorMinY[i] + (RR->saidas[RR->index_resposta]->matriz[i][0] * (delimitadorMaxY[i] - delimitadorMinY[i]));
        if(RR->saidas[RR->index_resposta]->matriz[i][0] < 0)
            RR->saidas[RR->index_resposta]->matriz[i][0] = 0;
    }

    Kp = RR->saidas[RR->index_resposta]->matriz[0][0];
    Ki = RR->saidas[RR->index_resposta]->matriz[1][0];
    Kd = RR->saidas[RR->index_resposta]->matriz[2][0];
    N = 1 / RR->saidas[RR->index_resposta]->matriz[3][0];
}



