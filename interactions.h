#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "conveyor.h"

#define HUMAN_UNINFECTED_0  300.0
#define HUMAN_HOSTS_0       1.0
#define HUMAN_PIMMUNE_0      0.0
#define HUMAN_PIMMUNE_HOSTS_0 0.0
#define HUMAN_REMISSION_0   0.0
#define MOSQUITO_UNINFECTED_0 300.0
#define MOSQUITO_VECTORS_0    0.0
#define MOSQUITO_TAINTED_0  0.0

#define TIME_START  0.0
//#define TIME_MAX    365.0
#define TIME_MAX  1000000.0
#define TIME_D      0.01
//#define REPORT_INTERVAL 1.0
#define REPORT_INTERVAL 2000000.0


#define PIMMUNITY_RATE 0.01 // Rate of hosts becoming partially immune
#define PIMMUNE_HOST  0.99 // Chance that partially immune becomes symptomless carrier of the disease, vs losing immunity and becoming infected
#define RECOVERY_RATE 0.3
#define MALARIA_INDUCED_DEATH_RATE 0.0005
#define HUMAN_BIRTH_RATE  0.000049828884
#define HUMAN_DEATH_RATE  0.000022724162
#define RELAPSE_RATE  0.0009308693
#define REMISSION_RATE  0.15
#define MOSQUITO_BIRTH_RATE 0.01
#define MOSQUITO_DEATH_RATE 0.01
#define MOSQUITO_INFECTION_RATE 0.1
#define MOSQUITO_INCUBATION_TIME 10
#define PROB_BITE_HUMAN 0.3
#define HEALTHY_HUMAN_MOSQUITO_RATIO  0.04

struct human_population {
  double uninfected;
  double hosts;
  double pimmune;
  double pimmune_hosts;
  double remission;
};

struct mosquito_population {
  double uninfected;
  struct conveyor *tainted_conv;
  double tainted;
  double vectors;
};

extern struct human_population *h_pop;
extern struct mosquito_population *m_pop;

void prepare_round( double td );

double human_uninfected( double td );
double human_hosts( double td );
double human_pimmune( double td );
double human_pimmune_hosts( double td );
double human_remission( double td );

double mosquito_uninfected( double td );
void mosquito_tainted( double td );
double mosquito_vectors( double td );

#endif
