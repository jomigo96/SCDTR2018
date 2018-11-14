% Dados obtidos para cálculo
% data deleted, import from csv
%data4 = [mv, micros, led1];

% Obter o ganho dos dados sobre o escalão na coluna três
gain = [data(:,1), data(:,1)./data(:,3)];
plot(1:size(data,1), data(:,2), 1:size(data,1), data(:,3), 1:size(data,1), gain(:,2));
plot(1:size(data,1), gain(:,2));

% Filtrar a matriz utilizando a média dos últimos windowSize valores
windowSize = 5;
b = (1/windowSize) * ones(1,windowSize);
a = 1;
datafiltered = [data(:,1) filter(b,a,data(:,2))];
plot(1:size(data,1), data(:,2), 1:size(data,1), datafiltered(:,2));

% Retirar os últimos valores arredondados de cada escalão da matriz filtrada
avgs = round([0; datafiltered(2001,2); datafiltered(3001,2); datafiltered(4001,2); datafiltered(5001,2); 0]);

% Criar uma matriz utilizando ditos valores para comparar a plot
dataestimated(:,1) = data(:,1);
dataestimated(1:1001,2) = avgs(1);
dataestimated(1002:2001,2) = avgs(2);
dataestimated(2002:3001,2) = avgs(3);
dataestimated(3002:4001,2) = avgs(4);
dataestimated(4002:5001,2) = avgs(5);
dataestimated(5001:size(data,1),2) = avgs(1);
%plot(1:size(data,1), data(:,2), 1:size(data,1), dataestimated(:,2));

% Encontrar a constante de tempo
Tc = zeros(4,2);
Tc(1,1) = 1001 + max(find(abs(data(1002:2001,2) - avgs(2)) >= (1 - 0.63) * (avgs(2) - data(1002,2))));
Tc(2,1) = 2001 + max(find(abs(data(2002:3001,2) - avgs(3)) >= (1 - 0.63) * (avgs(3) - data(2002,2))));
Tc(3,1) = 3001 + max(find(abs(data(3002:4001,2) - avgs(4)) >= (1 - 0.63) * (avgs(4) - data(3002,2))));
Tc(4,1) = 4001 + max(find(abs(data(4002:5001,2) - avgs(5)) >= (1 - 0.63) * (avgs(5) - data(4002,2))));
Tc(:,2) = [data(Tc(1),1) - data(1002,1); data(Tc(2),1) - data(2002,1); data(Tc(3),1) - data(3002,1); data(Tc(4),1) - data(4002,1)];
disp("Constantes de tempo em segundos");
disp(Tc(:,2) * 10.^(-6));
% Resultados obtidos: 0.022376, 0.011336, 0.005672, 0.005664
