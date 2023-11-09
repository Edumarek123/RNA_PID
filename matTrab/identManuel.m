clear, format long, clc;

% Variaveis
moduloSinal=1;
tempoAmostragem= 1.859 / 1000;

% Aquisicao dos Dados
arquivo=num2str(moduloSinal);
arquivo='ensaio1'
caminho='C:\Users\eduma\Desktop\TCC\PIDController\matTrab\ensaios\';
diretorio=strcat(caminho, arquivo, '.txt');
DadosY=readmatrix(diretorio);
velocidadeMaxima=max(DadosY);
velocidadeMaxima = 10;

% Processando dados
numeroPontos=size(DadosY, 1);
DadosR=velocidadeMaxima*ones(numeroPontos, 1);
DadosU=ones(numeroPontos, 1)*moduloSinal;
DadosT=[0:tempoAmostragem:((numeroPontos-1)*tempoAmostragem)];

% Estimando Funcao de Transferência
mydata=iddata(DadosY, DadosU, tempoAmostragem); 
G=tfest(mydata, 7)

opt = pidtuneOptions('DesignFocus', 'reference-tracking');
Cc=pidtune(G, 'PIDF', opt);

% Verifica dados
respostaDegrau=step(G*moduloSinal, DadosT);
figure, hold on;
plot(DadosT, DadosY, 'g');
plot(DadosT, respostaDegrau, 'b');
plot(DadosT, DadosR, 'm');
legend('Saida', 'Referencia');
legend('Ensaio', 'Estimado', 'Referencia', 'gzinha');

