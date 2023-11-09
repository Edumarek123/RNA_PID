//
// Created by Edumarek on 12/02/2022.
//

#include "PID_lib.hpp"

//------------------CONSTRUTORES&DESTRUTORES-------------------//
ControladorPID::ControladorPID(_ponto tA, _ponto min_uK, _ponto max_uK)
{
    Ta = tA;
    min_Uk = min_uK;
    max_Uk = max_uK;

    int hl[] = {5};
    RR = new RedeNeural(8, 4, hl, sizeof(hl) / sizeof(int), 1); // entradas, saidas, camadas ocultas, numero de camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(_ponto kP, _ponto kD, _ponto kI, _ponto n, _ponto tA, _ponto min_uK, _ponto max_uK)
{
    Kp = kP;
    Kd = kD;
    Ki = kI;
    Ta = tA;
    N = n;
    min_Uk = min_uK;
    max_Uk = max_uK;

    int hl[] = {5};
    RR = new RedeNeural(8, 4, hl, sizeof(hl) / sizeof(int), 1); // entradas, saidas, camadas ocultas, leraning_rate
}

ControladorPID::ControladorPID(const ControladorPID &c)
{
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

ControladorPID::~ControladorPID()
{
    delete RR;

    free(delimitadorMaxX);
    free(delimitadorMinY);
    free(delimitadorMaxY);
}

//---------------------------METODOS---------------------------//
void ControladorPID::Imprimir_Parametros()
{
    Serial.print("Kp: ");
    Serial.print(Kp, 4);
    Serial.print(" | Kd: ");
    Serial.print(Kd, 4);
    Serial.print(" | Ki: ");
    Serial.print(Ki, 4);
    Serial.print(" | N: ");
    Serial.print(N, 2);
    Serial.print(" | TA: ");
    Serial.print(Ta * 1000, 4);
    Serial.print(" | Uk: ");
    Serial.print(Uk[0], 2);
    Serial.print("\n");
}

void ControladorPID::CarregarDelimitadores(_ponto *p)
{
    delimitadorMinX = (_ponto *)malloc(RR->tam_layer_x * sizeof(_ponto));
    delimitadorMaxX = (_ponto *)malloc(RR->tam_layer_x * sizeof(_ponto));

    delimitadorMinY = (_ponto *)malloc(RR->tam_layer_y * sizeof(_ponto));
    delimitadorMaxY = (_ponto *)malloc(RR->tam_layer_y * sizeof(_ponto));

    int k = 0;
    for (int i = 0; i < RR->tam_layer_x; i++)
    {
        delimitadorMinX[i] = p[k];
        k++;
    }

    for (int i = 0; i < RR->tam_layer_y; i++)
    {
        delimitadorMinY[i] = p[k];
        k++;
    }

    for (int i = 0; i < RR->tam_layer_x; i++)
    {
        delimitadorMaxX[i] = p[k];
        k++;
    }

    for (int i = 0; i < RR->tam_layer_y; i++)
    {
        delimitadorMaxY[i] = p[k];
        k++;
    }
}

_ponto ControladorPID::Calcula_Acao_Controle(_ponto referencia, _ponto saida)
{
    unsigned long tInicio;
    _ponto a, b, c, d, e, f;

    if(calcularTempoExecucao)
        tInicio = micros();

    // Calcula erro
    Yk[0] = saida;
    Ek[0] = referencia - saida;

    // Calcula acao de controle
    a = (4 * Kp) + (4 * Kd * N) + (2 * Ki * Ta) + (2 * Kp * N * Ta) + (Ki * N * Ta * Ta);
    b = (2 * Ki * N * Ta * Ta) - (8 * Kd * N) - (8 * Kp);
    c = (4 * Kp) + (4 * Kd * N) - (2 * Ki * Ta) - (2 * Kp * N * Ta) + (Ki * N * Ta * Ta);
    d = -8;
    e = 4 - (2 * Ta * N);
    f = 4 + (2 * N * Ta);

    Uk[0] = ((Ek[0] * a) + (Ek[1] * b) + (Ek[2] * c) - (Uk[1] * d) - (Uk[2] * e)) / f;

    // Limitador acao controle
    if (Uk[0] > max_Uk)
        Uk[0] = max_Uk;
    if (Uk[0] <= min_Uk)
        Uk[0] = min_Uk;

    // Atualiza Saidas
    Yk[1] = Yk[0];

    // Atualiza erros
    Ek[2] = Ek[1];
    Ek[1] = Ek[0];

    // Atualiza acoes de controle
    Uk[1] = Uk[0];
    Uk[2] = Uk[1];

    // Atualizacao parametros
    iteracoesAcaoControle++;
    if (iteracoesAcaoControle >= limiteIteracoesAcaoControle && atualizarParametros)
    {
        iteracoesAcaoControle = 0;
        Atualiza_Parametros();
    }

    if (imprimirParametros)
        Imprimir_Parametros();

    if (calcularTempoExecucao)
        tExecucao = micros() - tInicio;

    return Uk[0];
}

void ControladorPID::Atualiza_Parametros()
{
    Matriz m(8, 1);

    m.matriz[0][0] = Uk[0];
    m.matriz[1][0] = Uk[1];
    m.matriz[2][0] = Uk[2];
    m.matriz[3][0] = Yk[0];
    m.matriz[4][0] = Yk[1];
    m.matriz[5][0] = Ek[0];
    m.matriz[6][0] = Ek[1];
    m.matriz[7][0] = Ek[2];

    for (int i = 0; i < m.linhas; i++)
        m.matriz[i][0] = (m.matriz[i][0] - delimitadorMinX[i]) / (delimitadorMaxX[i] - delimitadorMinX[i]);

    RR->FeedFoward(m);

    for (int i = 0; i < RR->saidas[RR->index_resposta]->linhas; i++)
    {
        RR->saidas[RR->index_resposta]->matriz[i][0] = delimitadorMinY[i] + (RR->saidas[RR->index_resposta]->matriz[i][0] * (delimitadorMaxY[i] - delimitadorMinY[i]));
        if (RR->saidas[RR->index_resposta]->matriz[i][0] < 0)
            RR->saidas[RR->index_resposta]->matriz[i][0] = 0;
    }

    Kp = RR->saidas[RR->index_resposta]->matriz[0][0];
    Ki = RR->saidas[RR->index_resposta]->matriz[1][0];
    Kd = RR->saidas[RR->index_resposta]->matriz[2][0];
    N = 1 / RR->saidas[RR->index_resposta]->matriz[3][0];
    
}
