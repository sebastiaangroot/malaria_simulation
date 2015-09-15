#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#define HUMAN_UNINFECTED_0  300.0
#define HUMAN_HOSTS_0       1.0
#define HUMAN_IMMUNE_0      0.0
#define MOSQUITO_UNINFECTED_0 300.0
#define MOSQUITO_VECTORS_0    0.0
#define MOSQUITO_INCUBATED_0  0.0

#define TIME_START  0.0
#define TIME_MAX    200.0
#define TIME_D      0.01
#define REPORT_INTERVAL 10.0

#define IMMUNITY_RATE 0.01
#define RECOVERY_RATE 0.3
#define MALARIA_INDUCED_DEATH_RATE 0.005
#define PROB_BIT 0.3
#define MOSQUITO_BIRTH_RATE 0.01
#define MOSQUITO_DEATH_RATE 0.01
#define MOSQUITO_INFECTION_RATE 0.05
#define PROB_BITE_HUMAN 0.3

struct human_population {
  double uninfected;
  double hosts;
  double immune;
};

struct mosquito_population {
  double uninfected;
  double incubated;
  double vectors;
};

extern struct human_population *h_pop;
extern struct mosquito_population *m_pop;

double human_uninfected( double td );
double human_hosts( double td );
double human_immune( double td );

double mosquito_uninfected( double td );
double mosquito_incubated( double td );
double mosquito_vectors( double td );

#endif
