close all, clear all;
%The system % Typical values
k11 = .01*326.840; 
k12 = .01*92.72; 
k21 = .01*109.37; 
k22 = .01*411.81; 
K = [k11, k12 ; k21 , k22];

% case 1
%L1 = 150; o1 = 30; L2 = 80; o2 = 0;

% case 2
%L1 = 80; o1 = 50; L2 = 150; o2 = 50;

% case 3
L1 = 80; o1 = 4.88; L2 = 80; o2 = 6.82;

%symmetric costs
c1 = 1; c2 = 4; 

%asymmetric costs
%c1 = 1; c2 = 1;


c = [c1 c2]; 
L = [L1;L2]; 
o = [o1;o2];

% SOLVE WITH CONSENSUS
rho = 0.07;
%node 1 initialization
node1.d1 = 0;
node1.d2 = 0;
node1.d1_av = 0;
node1.d2_av = 0;
node1.y1 = 0;
node1.y2 = 0;
node1.k11 = k11; 
node1.k12 = k12; 
node1.c = c1;
node1.o = o1;
node1.L = L1;

%node 2 initialization
node2.d1 = 0;
node2.d2 = 0;
node2.d1_av = 0;
node2.d2_av = 0;
node2.y1 = 0;
node2.y2 = 0;
node2.k11 = k22; 
node2.k12 = k21; 
node2.c = c2;
node2.o = o2;
node2.L = L2;

%Initial contition (iteration = 1)
d11(1) = node1.d1;
d12(1) = node1.d2;
d21(1) = node2.d2;
d22(1) = node2.d1;
av1(1) = (d11(1)+d21(1))/2;
av2(1) = (d12(1)+d22(1))/2;
%iterations
N=200;
for i=2:N
    
   % node 1
   d = primal_solve(node1, rho);
   node1.d1= d(1);
   node1.d2= d(2);
   
   %node2
   d = primal_solve(node2, rho);
   node2.d1= d(1);
   node2.d2= d(2);

      
   % Compute average with available data
   %node 1
   node1.d1_av = (node1.d1+node2.d2)/2;
   node1.d2_av = (node1.d2+node2.d1)/2;
   
   %node 2
   node2.d1_av = (node2.d1+node1.d2)/2;
   node2.d2_av = (node2.d2+node1.d1)/2;
   
   % Update local lagrangians
   %node 1
   node1.y1 = node1.y1 + rho*(node1.d1-node1.d1_av);
   node1.y2 = node1.y2 + rho*(node1.d2-node1.d2_av);
   
   %node 2
   node2.y1 = node2.y1 + rho*(node2.d1-node2.d1_av);
   node2.y2 = node2.y2 + rho*(node2.d2-node2.d2_av);

   d11(i) = node1.d1;
   d12(i) = node1.d2;
   d21(i) = node2.d2;
   d22(i) = node2.d1;
   av1(i) = (d11(i)+d21(i))/2;
   av2(i) = (d12(i)+d22(i))/2;
   
end


disp('Consensus Solutions')
d = [node1.d1_av; node1.d2_av]
l = K*[node1.d1_av; node1.d2_av]+[o1; o2]

%Plots
figure(10);
plot(1:N, av1, 1:N, av2, 1:N, d11, 1:N, d12, 1:N, d21, 1:N, d22);
legend('av1','av2', 'd11', 'd12', 'd21', 'd22');
title('time convergence');
xlabel('iter');

figure(15);
t = 0:100;
constr1 = (L1-o1)/k12-(k11/k12)*t;
constr2 = (L2-o2)/k22-(k21/k22)*t;
[x,y]=meshgrid(t,t);
hold on;
z = c1*x+c2*y;
contour(x,y,z);
plot(t,constr1,t,constr2,'LineWidth',2);
plot(t,zeros(size(t)),'k','LineWidth',2);
plot(zeros(size(t)),t,'k','LineWidth',2);
plot(t,100*ones(size(t)),'k','LineWidth',2);
plot(100*ones(size(t)),t,'k','LineWidth',2);
plot(av1,av2,'--','LineWidth',2);
plot(av1,av2,'bo');
% plot(d11,d12,'m-.','LineWidth',1);
% plot(d11,d12,'mx');
% plot(d21,d22,'k-.','LineWidth',1);
% plot(d21,d22,'kx');
title('solution space convergence');
xlabel('d_1');
ylabel('d_2');
% plot(d_(1),d_(2),'r*')
axis([-10,110,-10,110]);
hold off;



function cost = evaluate_cost(node, d1, d2, rho)
    cost =  node.c*d1 + node.y1*(d1-node.d1_av) + node.y2*(d2-node.d2_av) + ...
		rho/2.0*((d1-node.d1_av)^2 + (d2-node.d2_av)^2);
end

function check = check_feasibility(node, d1, d2)
	tol = 0.01; %%tolerance for rounding errors
	
	if ((d1 < 0) || (d1 > 100))% || (d2 < 0) || (d2 > 100))
		check = 0;
		return
	end
	check = (d1*node.k11 + d2*node.k12 > node.L-node.o-tol);

end

function d = primal_solve(node, rho)

    d = [-1,-1];
    d1_best = -1;
    d2_best = -1;

    cost_best = 1000000;
    
    z1 = rho*node.d1_av-node.y1-node.c;
    z2 = rho*node.d2_av-node.y2;

    % Unconstrained
    d1_sol = 1/rho*z1;
    d2_sol = 1/rho*z2;
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d = [d1_sol, d2_sol];
            return;
        end	
    end

    % constrained to linear boundary
    n = node.k11^2+node.k12^2;
    d1_sol = 1/rho*z1 - node.k11/n * (node.o-node.L+(z1*node.k11+z2*node.k12)/rho);
    d2_sol = 1/rho*z2 - node.k12/n * (node.o-node.L+(z1*node.k11+z2*node.k12)/rho);
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d1_best = d1_sol;
            d2_best = d2_sol;
            cost_best = cost;
        end
    end
    

    % constrained to 0 boundary
    d1_sol = 0; 
    d2_sol = z2/rho;
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d1_best = d1_sol;
            d2_best = d2_sol;
            cost_best = cost;
        end
    end

    % constrained to 100 boundary
    d1_sol = 100;
    d2_sol = z2/rho;
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d1_best = d1_sol;
            d2_best = d2_sol;
            cost_best = cost;
        end
    end

    % constrained to linear and 0 boundary
    d1_sol = 0;
    d2_sol = -(node.o-node.L)/node.k12;
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d1_best = d1_sol;
            d2_best = d2_sol;
            cost_best = cost;
        end
    end

    % constrained to linear and 100 boundary
    d1_sol = 100;
    d2_sol = (node.o-node.L+100*node.k11);
    if check_feasibility(node, d1_sol, d2_sol)
        cost = evaluate_cost(node, d1_sol, d2_sol, rho);
        if(cost < cost_best)
            d1_best = d1_sol;
            d2_best = d2_sol;
            cost_best = cost;
        end
    end

    d = [d1_best, d2_best];
end
