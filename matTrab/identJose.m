clear, format short, clc;

%VARIAVEIS
tempoAmostragem=1.7 / 1000; %[s]
yMaximo=6; %[mm]
diretorioSaidas='C:\Users\eduma\Desktop\TCC\PIDController\matTrab\saidas\'

%IMPORTANDO DADOS ENSAIO
arquivo='ensaio1';
caminho='C:\Users\eduma\Desktop\TCC\PIDController\matTrab\ensaios\';
diretorioEnsaio=strcat(caminho, arquivo, '.txt');
diretorioSimulacao= 'C:\Users\eduma\Desktop\TCC\PIDController\matTrab\simuidentJose1.slx';
DadosY=readmatrix(diretorioEnsaio);

%PROCESSANDO ENSAIO
numeroPontos=size(DadosY, 1);
DadosU=ones(numeroPontos, 1);
DadosT=[0:tempoAmostragem:((numeroPontos-1)*tempoAmostragem)];

%ESTIMANDO FUNCAO DE TRANSFERENCIA
mydata=iddata(DadosY, DadosU, tempoAmostragem); 
G=tfest(mydata, 7)

%DADOS SIMULACAOS
% X:
% Acao de controle (Uk[n]): 0, -1, -2
% Saida (Yk[n]): 0, -1
% Erro (Ek[n]): 0, -1, -2

% Y:
% Kp, Kd, Ki, N

% Para respostas a:
% Triangular, Seno, Quadrada

%GERA FUNCOES DE TRANSFERENCIA | REALIZA SIMULACOES | COLETAS DE DADOS
numeroDeFuncoes=1000;
coeficienteDispersao=0.3;
coeficienteErroMax = 0.25;
opt = pidtuneOptions('DesignFocus', 'reference-tracking');

saidas = 0;
iterador = 0;
warning('off', 'all');
while iterador < numeroDeFuncoes
    %Gerando planta
    vetorNumeradorG=rand(1, size(G.Numerator,2));
    vetorDenominadorG=rand(1, size(G.Denominator, 2));
    %Numerador
    for j=1:size(G.Numerator,2)
        vetorNumeradorG(j)=vetorNumeradorG(j) * G.Numerator(j) + (G.Numerator(j) * coeficienteDispersao);
    end
    %Denominador
    for j=1:size(G.Denominator,2)
        vetorDenominadorG(j)=vetorDenominadorG(j) * G.Denominator(j) + (G.Denominator(j) * coeficienteDispersao);
    end
    
    %Monta funcao TF
    Gg=tf(vetorNumeradorG, vetorDenominadorG);
    
    %Sintonizando um controlador
    [Cc, info] = pidtune(Gg, 'PIDf', opt);
    if Cc.Tf == 0
        Cc.tf = 0.00001;
    end
    
    %Verifica estabilidade
    if(info.Stable)
        info;
        %Simulacao
        simulouSemErro = 1;
        try
            SS=sim(diretorioSimulacao);
        catch e
            simulouSemErro = 0;
        end
        %Verifica se simulou ok
        if std(SS.erroRY.Data(:,1) - SS.erroRY.Data(:,2)) > (yMaximo * coeficienteErroMax)
            simulouSemErro = 0;
        end
        
        %Armazena resultados
        if simulouSemErro
            pause(0.5);
            iterador = iterador + 1
            Ic=[Cc.Kp Cc.Ki Cc.Kd Cc.Tf];
            Mc=zeros(size(SS.simulacoes.Data, 1), 4);
            for j=1:size(SS.simulacoes.Data, 1)
                Mc(j, :)=Ic;
            end
            if iterador == 1
                saidas=[SS.simulacoes.Data Mc]; %[ Ek | Uk | Yk | Kp | Ki | Kd | Tf ] 
            else
                saidas=[saidas; [SS.simulacoes.Data Mc]]; %armazena matriz da simulacao                         
            end    
        else
%             disp('controlador cagado');
        end
    else
%         disp('controlador cagado');
    end
    clc;
    [(iterador * 100 / numeroDeFuncoes) 100]
end
warning('on', 'all');
clear arquivo C Cc DadosR DadosT DadosU G i Ic iterador j Mc SS TaEstimado numeroDeFuncoes vetorDenominadorG vetorNumeradorG Gg DadosY numeroPontos pct valoresMaximos valoresMinimos ans simulouSemErro myData coeficienteDispersao;
clc;

% PROCESSAMENTO PARA EXPORTACAO DADOS
%[ Ek(0) | Ek(-1) | Ek(-2) | Uk(0) | Uk(-1) | Uk(-2) | Yk(0) | Yk(-1) | Kp | Ki | Kd | Tf ]
paraArquivo=zeros(size(saidas, 1), size(saidas, 2) + 5);
for i=1:size(saidas, 1)
    pct=100 * i / size(saidas, 1);
    disp([pct 100])

    if i==1
        paraArquivo(i, :)=[saidas(i, 1) 0 0 saidas(i, 2) 0 0 saidas(i, 3) 0 saidas(i, 4) saidas(i, 5) saidas(i, 6) saidas(i, 7)];
    elseif saidas(i, 4)~=saidas(i-1, 4) && i > 2
        paraArquivo(i, :)=[saidas(i, 1) 0 0 saidas(i, 2) 0 0 saidas(i, 3) 0 saidas(i, 4) saidas(i, 5) saidas(i, 6) saidas(i, 7)];
    elseif i==2 || (saidas(i-1, 4)~=saidas(i-2, 4))
        paraArquivo(i, :)=[saidas(i, 1) saidas(i-1, 1) 0 saidas(i, 2) saidas(i-1, 2) 0 saidas(i, 3) saidas(i-1, 3) saidas(i, 4) saidas(i, 5) saidas(i, 6) saidas(i, 7)];
    else
        paraArquivo(i, :)=[saidas(i, 1) saidas(i-1, 1) saidas(i-2, 1) saidas(i, 2) saidas(i-1, 2) saidas(i-2, 2) saidas(i, 3) saidas(i-1, 3) saidas(i, 4) saidas(i, 5) saidas(i, 6) saidas(i, 7)];
    end
end
clc;

%AJUSTA LIMITES 0-1
valoresMaximos=max(paraArquivo);
valoresMinimos=min(paraArquivo);
for i=1:size(paraArquivo, 1)
    pct = 100 * i / size(paraArquivo, 1);
    disp([pct 100])

    for j=1:size(paraArquivo, 2)
        if valoresMaximos(j) ~= valoresMinimos(j)
            paraArquivo(i, j)=(paraArquivo(i, j)-valoresMinimos(j))/(valoresMaximos(j)-valoresMinimos(j));
        end
    end
end
delimitadores=[valoresMinimos valoresMaximos];
clc;

%RANDOMIZA LINHAS
paraArquivo=paraArquivo(randperm(size(paraArquivo, 1)), :);

%VERIFICA ARQUIVOS CONFLITANTES
entradas=paraArquivo(:, 1:8);
saidas_esperadas=paraArquivo(:, 9:12);

if isfile(strcat(diretorioSaidas, 'delimitadores.hpp'))
    delete (strcat(diretorioSaidas, 'delimitadores.hpp'));
end
if isfile(strcat(diretorioSaidas, 'entradas.txt'))
    delete (strcat(diretorioSaidas, 'entradas.txt'));
end
if isfile(strcat(diretorioSaidas, 'saidasEsperadas.txt'))
    delete (strcat(diretorioSaidas, 'saidasEsperadas.txt'));
end

%EXPORTA DELIMITADORES
arquivosDelimitadores = fopen(strcat(diretorioSaidas, 'delimitadores.hpp'), 'w');
fprintf(arquivosDelimitadores, '#ifndef DELIMITADORES_SALVOS_HPP\n');
fprintf(arquivosDelimitadores, '#define DELIMITADORES_SALVOS_HPP\n\n');
fprintf(arquivosDelimitadores, '#include "src/matriz_lib/matriz_lib.hpp"\n\n');
fprintf(arquivosDelimitadores, '_ponto DELIMITADORES_SALVOS[] = {');
for i=1:size(delimitadores, 2)
    if(i < size(delimitadores, 2))
        fprintf(arquivosDelimitadores, '%f, ', delimitadores(i));
    else
        fprintf(arquivosDelimitadores, '%f};', delimitadores(i));
    end
end
fprintf(arquivosDelimitadores, '\n\n#endif //DELIMITADORES_SALVOS_HPP');
fclose(arquivosDelimitadores);
clc;

%EXPORTA ENTRADAS
arquivoEntradas = fopen(strcat(diretorioSaidas, 'entradas.txt'), 'w');
for i=1:size(entradas, 1)
    pct = 100 * i / size(entradas, 1);
    disp([pct 100])

    for j=1:size(entradas, 2)
        fprintf(arquivoEntradas, '%f ', entradas(i, j));
    end
    fprintf(arquivoEntradas, '\n');
end
fclose(arquivoEntradas);
clc;

%EXPORTA SAIDAS
arquivoSaidasEsperadas = fopen(strcat(diretorioSaidas, 'saidasEsperadas.txt'), 'w');
for i=1:size(saidas_esperadas, 1)
    pct = 100 * i / size(saidas_esperadas, 1);
    disp([pct 100])
   
    for j=1:size(saidas_esperadas, 2)
        fprintf(arquivoSaidasEsperadas, '%f ', saidas_esperadas(i, j));
    end
    fprintf(arquivoSaidasEsperadas, '\n');
end
fclose(arquivoSaidasEsperadas);
clc;

disp("Finalizado!")