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
c1 = 1; c2 = 1; 

%asymmetric costs
%c1 = 1; c2 = 3;


c = [c1 c2]; 
L = [L1;L2]; 
o = [o1;o2];

% SOLVE WITH CONSENSUS
rho = 0.07;
%node 1 initialization
node1.index = 1;
node1.d = [(L1-o1)/k11;0];
node1.d_av = [(L1-o1)/k11;0];
node1.y = [0;0];
node1.k = [k11;k12]; 
node1.n = norm(node1.k)^2;
node1.m = node1.n-k11^2;
node1.c = c1;
node1.o = o1;
node1.L = L1;

%node 2 initialization
node2.index = 2;
node2.d = [0;(L2-o2)/k22];
node2.d_av = [0;(L2-o2)/k22];
node2.y = [0;0];
node2.k = [k21;k22]; 
node2.n = norm(node2.k)^2;
node2.m = node2.n-k22^2;
node2.c = c2;
node2.o = o2;
node2.L = L2;

%Initial contition (iteration = 1)
d11(1) = node1.d(1);
d12(1) = node1.d(2);
d21(1) = node2.d(1);
d22(1) = node2.d(2);
av1(1) = (d11(1)+d21(1))/2;
av2(1) = (d12(1)+d22(1))/2;
%iterations
for i=2:50
   % node 1
   [d1, cost1] = primal_solve(node1, rho);
   node1.d = d1;
   
%    %DEBUG: check with matlab quadprog
%    Q_ = [rho, 0; 0 rho];
%    c_ = [c1+node1.y(1)-rho*node1.d_av(1),node1.y(2)-rho*node1.d_av(2)];
%    A_ = [-k11 -k12; -1 0; 1 0];
%    b_ = [o1-L1, 0, 100];
%    d1_ = quadprog(Q_,c_,A_,b_,[],[],[],[]);
  
   %node2
   [d2, cost2] = primal_solve(node2, rho);
   node2.d = d2;
   
%    %DEBUG: check with matlab quadprog
%    Q_ = [rho, 0; 0 rho];
%    c_ = [node2.y(1)-rho*node2.d_av(1),c2+node2.y(2)-rho*node2.d_av(2)];
%    A_ = [-k21 -k22;0 -1; 0 1];
%    b_ = [o2-L2, 0, 100];
%    d2_ = quadprog(Q_,c_,A_,b_,[],[],[],[]);
   
   % Compute average with available data
   %node 1
   node1.d_av = (node1.d+node2.d)/2;
   
   %node 2
   node2.d_av = (node1.d+node2.d)/2;
   
   % Update local lagrangians
   %node 1
   node1.y = node1.y + rho*(node1.d-node1.d_av);
   
   %node 2
   node2.y = node2.y + rho*(node2.d-node2.d_av);
   
   
   %save data for plots
   d11(i) = node1.d(1);
   d12(i) = node1.d(2);
   d21(i) = node2.d(1);
   d22(i) = node2.d(2);
   av1(i) = (d11(i)+d21(i))/2;
   av2(i) = (d12(i)+d22(i))/2;
end;

% % SOLVE WITH MATLAB QUADPROG
% Q_ = [0, 0; 0 0];
% A_ = -K; 
% c_ = c;
% b_ = [o1-L1; o2-L2];
% lb = [0;0]; ub = [100;100];
% disp('Matlab solutions')
% d_ = quadprog(Q_,c_,A_,b_,[],[],lb,ub)
% l_ = K*d_+o

disp('Consensus Solutions')
d = node1.d_av
l = K*d+o

%Plots
figure(10);
plot(1:50, av1, 1:50, av2, 1:50, d11, 1:50, d12, 1:50, d21, 1:50, d22);
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


function cost = evaluate_cost(node,d,rho)
    cost =  node.c*d(node.index) + node.y'*(d-node.d_av) + ...
            rho/2*norm(d-node.d_av)^2;
end

function check = check_feasibility(node, d)
   tol = 0.001; %%tolerance for rounding errors
   if (d(node.index) < 0-tol), check = 0; return; end;
   if (d(node.index) > 100+tol), check = 0; return; end;
   if (d'*node.k < node.L-node.o-tol), check = 0; return; end;
   check = 1;
end

function [d, cost] = primal_solve(node, rho)
    d_best = [-1,-1]';
    cost_best = 1000000; %large number
    sol_unconstrained = 1;
    sol_boundary_linear = 1;
    sol_boundary_0 = 1;
    sol_boundary_100 = 1;
    sol_linear_0 = 1;
    sol_linear_100 = 1;
    z = rho*node.d_av - node.y;
    z(node.index) = z(node.index) - node.c;
    %unconstrained minimum
    d_u = (1/rho)*z;
    sol_unconstrained = check_feasibility(node,d_u);
    if sol_unconstrained
        cost_unconstrained = evaluate_cost(node, d_u,rho);
        if cost_unconstrained < cost_best
           d = d_u;
           cost = cost_unconstrained;
           return  %IF UNCONSTRAINED SOLUTION EXISTS, THEN IT IS OPTIMAL
                   %NO NEED TO COMPUTE THE OTHER
        end;
    end;
    %compute minimum constrained to linear boundary   
    d_bl = (1/rho)*z - node.k/node.n*(node.o-node.L+(1/rho)*z'*node.k);
    %check feasibility of minimum constrained to linear boundary
    sol_boundary_linear = check_feasibility(node, d_bl);
    % compute cost and if best store new optimum
    if sol_boundary_linear 
        cost_boundary_linear = evaluate_cost(node, d_bl,rho);
        if cost_boundary_linear < cost_best
           d_best = d_bl;
           cost_best = cost_boundary_linear;
        end;
    end;
    %compute minimum constrained to 0 boundary
    d_b0 = (1/rho)*z;
    d_b0(node.index) = 0;
    %check feasibility of minimum constrained to 0 boundary
    sol_boundary_0 = check_feasibility(node, d_b0);
    % compute cost and if best store new optimum
    if sol_boundary_0 
        cost_boundary_0 = evaluate_cost(node, d_b0,rho);
        if cost_boundary_0 < cost_best
           d_best = d_b0;
           cost_best = cost_boundary_0;
        end;
    end;
    %compute minimum constrained to 100 boundary
    d_b1 = (1/rho)*z;
    d_b1(node.index) = 100;
    %check feasibility of minimum constrained to 100 boundary
    sol_boundary_100 = check_feasibility(node, d_b1);
    % compute cost and if best store new optimum
    if sol_boundary_100 
        cost_boundary_100 = evaluate_cost(node, d_b1,rho);
        if cost_boundary_100 < cost_best
           d_best = d_b1;
           cost_best = cost_boundary_100;
        end;
    end;
    % compute minimum constrained to linear and 0 boundary
    d_l0 = (1/rho)*z - ...
          (1/node.m)*node.k*(node.o-node.L) + ...
          (1/rho/node.m)*node.k*(node.k(node.index)*z(node.index)-z'*node.k);
    d_l0(node.index) = 0;
    %check feasibility of minimum constrained to linear and 0 boundary
    sol_linear_0 = check_feasibility(node, d_l0);
    % compute cost and if best store new optimum
    if sol_linear_0 
        cost_linear_0 = evaluate_cost(node, d_l0,rho);
        if cost_linear_0 < cost_best
           d_best = d_l0;
           cost_best = cost_linear_0;
        end;
    end;
    % compute minimum constrained to linear and 100 boundary
    d_l1 = (1/rho)*z - ...
          (1/node.m)*node.k*(node.o-node.L+100*node.k(node.index)) + ...
          (1/rho/node.m)*node.k*(node.k(node.index)*z(node.index)-z'*node.k);
    d_l1(node.index) = 100;
    %check feasibility of minimum constrained to linear and 0 boundary
    sol_linear_1 = check_feasibility(node, d_l1);
    % compute cost and if best store new optimum
    if sol_linear_1 
        cost_linear_1 = evaluate_cost(node, d_l1,rho);
        if cost_linear_1 < cost_best
           d_best = d_l1;
           cost_best = cost_linear_1;
        end;
    end;
    d = d_best;
    cost = cost_best;
end
    
