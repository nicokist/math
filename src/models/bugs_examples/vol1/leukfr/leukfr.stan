# BUGS example vol 1: LeukFr 
# http://mathstat.helsinki.fi/openbugs/Examples/Leukfr.html
#  http://www.openbugs.info/Examples/Leukfr.html

# The result for sigma is a bit different from those in the 
# webpage. 

# But the result for beta on 
# http://www.mrc-bsu.cam.ac.uk/bugs/winbugs/Vol1.pdf
# might not be correct. 



data {
  int(0,) N;
  int(0,) NT;
  int(0,) obs_t[N]; 
  int(0,) t[NT + 1]; 
  int(0,) fail[N]; 
  int(0,) Npair; 
  int(0,) pair[N];
  real Z[N]; 
}

transformed data {
  int Y[N, NT];
  int dN[N, NT]; 
  real c;
  real r; 

  for(i in 1:N) {
    for(j in 1:NT) {
      Y[i, j] <- int_step(obs_t[i] - t[j] + .000000001);
      dN[i, j] <- Y[i, j] * fail[i] * int_step(t[j + 1] - obs_t[i] - .000000001);
    }
  }
  c <- 0.001; 
  r <- 0.1; 
}

parameters {
  real beta; 
  real(0,) tau;
  real(0,) dL0[NT]; 
  real b[Npair]; 
} 

transformed parameters {
  real(0,) sigma; 
  sigma <- 1 / sqrt(tau); 
} 

model {
  beta ~ normal(0, 1000);
  tau ~ gamma(.001, .001); 
  // for (k in 1:Npair)  b[k] ~ normal(0, sigma); 
  b ~ normal(0, sigma); 
  for(j in 1:NT) {
    dL0[j] ~ gamma(r * (t[j + 1] - t[j]) * c, c);
    for(i in 1:N) {
      // dN[i, j] ~ poisson(Y[i, j] * exp(beta * Z[i] + b[pair[i]]) * dL0[j]); 
      lp__ <- lp__ + if_else(Y[i, j], poisson_log(dN[i, j], Y[i, j] * exp(beta * Z[i] + b[pair[i]]) * dL0[j]), 0); 

    }     
  }
}
