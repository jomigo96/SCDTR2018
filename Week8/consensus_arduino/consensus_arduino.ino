bool feasibility(float* d, float* k, int* e, float o, int L) {
  if (-k[0]*d[0]-k[1]*d[1] <= o - L and -e[0]*d[0]-e[1]*d[1] <= 0 and e[0]*d[0]+e[1]*d[1] <= 100) {
    return True
  }
  else {
    retur False 
  }
}

float cost_function(float rho, float* d, float* z) {
  return (1/2)*rho*(d[0]*d[0]+d[1]*d[1]) - d[0]*z[0]-d[1]*z[1] 
}

void setup() {
  // put your setup code here, to run once:
  i = 1 //nodo, 2 se for o outro
  k[2] = {k11, k12} //or {k21 k22}
  norm_k = pow(k11^2 + k12^2, 2)
  e[2] = {1, 0} //or {0, 1}
  float optimum_border_cost = 0
} 
  
void loop() {
  // put your main code here, to run repeatedly:
  
  d_av[2] = {d[0], d[1]};
  y[2] = {y[0] + rho(d[0] - d_av[0]), y[1] + rho(d[1] - d_av[1])};
  z[2] = {rho*d_av[0] - c[0] - y[0], rho*d_av[1] - c[1] - y[1]};
  d[2] = min(0.5*rho*(d[0]*d[0]+d[1]*d[1]) - d[0]*z[0] - d[1]*z[1]);
  cost_function = 0.5*rho*(d[0]*d[0]+d[1]*d[1]) - d[0]*z[0] - d[1]*z[1]; 
  
  // Solution in the interior, tem prioridade sobre as soluções na fronteira
  d_0[2] = {(1/rho)*z[0], (1/rho)*z[1]}
  
  if(feasibility(d, k, e, o, L == true)) {   //o -> external illuminance , L -> lower bound
    continue
  }
  else {  //unfeasible solution, check solution in the boundary
    w = -k[0]*z[0]-k[1]*z[1]
    d_set_1 = {(1/rho)*z[0] - k[0]*(o - L - (1/rho)*w)/norm_k, (1/rho)*z[1] - k[1]*(o - L - (1/rho)*w)/norm_k}
    d_set_2 = {(1/rho)z[0] - (1/rho)*e[0]*z[0], (1/rho)*z[1] - (1/rho)*e[1]*z[0]}
    d_set_3 = {(1/rho)*z[0] + e[0](100-(1/rho)*z[0]), (1/rho)*z[1] + e[1](100-(1/rho)*z[0])} //os zii são são z[1] no outro nodo
    d_set_4 = {(1/rho)*z[0] + (-k[0] + e[0]*k[0])/(norm_k - pow(k[0], 2))*(o - L) - (1/rho)*((-k[0]+e[0]*k[0])*(-k[0]*z[0]) - z[0]*(k[0]*k[0] - e[0]*norm_k))/(norm_k - pow(k[0], 2)), (1/rho)*z[1] + (-k[1] + e[0]*k[0])/(norm_k - pow(k[0], 2))*(o - L) - (1/rho)*((-k[1]+e[1]*k[0])*(-k[1]*z[1]) - z[0]*(k[1]*k[0] - e[1]*norm_k))/(norm_k - pow(k[0], 2))}
    d_set_5 = {(1/rho)*z[0] + ((-k[0]+e[0]*k[0])*(o - L)+100*(-k[0]*k[0]+e[0]*norm_k))/(norm_k -pow(k[0], 2)) - (1/rho)*((-k[0]+e[0]*k[0])*w + z[0]*(-k[0]*k[0]+e[0]*norm_k))/(norm_k-pow(k[0], 2), (1/rho)*z[1] + ((-k[1]+e[1]*k[0])*(o - L)+100*(-k[1]*k[0]+e[1]*norm_k))/(norm_k -pow(k[0], 2)) - (1/rho)*((-k[1]+e[1]*k[0])*w + z[0]*(-k[1]*k[0]+e[1]*norm_k))/(norm_k-pow(k[0], 2)}

    if(feasibility(d_set_1, k, e, o, L)==true) {
      optimum_border_cost = cost_function(rho, d_set_1, z)
    }
    else if(feasibility(d_set_2, k, e, o, L)==true) { 
      if (optimum_border_cost > cost_function(rho, d_set_2, z)) {
        optimum_border_cost = cost_function(rho, d_set_2, z)
      }
    }
    else if(feasibility(d_set_3, k, e, o, L)==true) { 
      if (optimum_border_cost > cost_function(rho, d_set_3, z)) {
        optimum_border_cost = cost_function(rho, d_set_3, z)
      }
    }
    else if(feasibility(d_set_4, k, e, o, L)==true) { 
      if (optimum_border_cost > cost_function(rho, d_set_4, z)) {
        optimum_border_cost = cost_function(rho, d_set_4, z)
      }
    }
    else if(feasibility(d_set_5, k, e, o, L)==true) { 
      if (optimum_border_cost > cost_function(rho, d_set_5, z)) {
        optimum_border_cost = cost_function(rho, d_set_5, z)
      }
    }
  }
}
