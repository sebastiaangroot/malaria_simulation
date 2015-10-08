#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "conveyor.h"

#define TIME_START  0.0
#define TIME_MAX    2000.0
#define TIME_D      0.1
#define REPORT_INTERVAL 5.0

#define FROM_NEWBORN_TO_TODDLER 0.016666667
#define FROM_TODDLER_TO_ADULT 0.000547945205

#define SICKLE_CELL_RATE 0.125
#define SICKLE_CELL_MALARIA_SURVIVAL_RATE 0.60

#define HUMAN_NEWBORN_UNINFECTED_0 10.0

#define HUMAN_TOD_N_UNINFECTED_0  15.0
#define HUMAN_TOD_N_TAINTED_0  0.0
#define HUMAN_TOD_N_HOSTS_0       0.0
#define HUMAN_TOD_N_PIMMUNE_0      0.0
#define HUMAN_TOD_N_REMISSION_0   0.0

#define HUMAN_TOD_S_UNINFECTED_0  15.0
#define HUMAN_TOD_S_TAINTED_0  0.0
#define HUMAN_TOD_S_HOSTS_0       0.0
#define HUMAN_TOD_S_PIMMUNE_0      0.0
#define HUMAN_TOD_S_REMISSION_0   0.0

#define HUMAN_ADULT_UNINFECTED_0  300.0
#define HUMAN_ADULT_TAINTED_0     0.0
#define HUMAN_ADULT_HOSTS_0       1.0
#define HUMAN_ADULT_PIMMUNE_0      0.0
#define HUMAN_ADULT_REMISSION_0   0.0

#define MOSQUITO_UNINFECTED_0 300.0
#define MOSQUITO_VECTORS_0    0.0
#define MOSQUITO_TAINTED_0  0.0

#define PIMMUNITY_RATE 0.01 // Rate of hosts becoming partially immune
#define RECOVERY_RATE 0.3
#define MALARIA_INDUCED_DEATH_RATE 0.0000035826733157
#define MALARIA_INDUCED_DEATH_RATE_TODDLER 0.0002266288952
#define HUMAN_BIRTH_RATE  0.000049828884
#define HUMAN_DEATH_RATE  0.000022724162
#define RELAPSE_RATE  0.0009308693
#define HUMAN_INCUBATION_TIME 18
#define REMISSION_RATE  0.15
#define MOSQUITO_BIRTH_RATE 0.075
#define MOSQUITO_DEATH_RATE 0.075
#define MOSQUITO_INFECTION_RATE 0.1
#define MOSQUITO_INCUBATION_TIME 2.0
#define PROB_BITE_HUMAN 1.0
#define HEALTHY_HUMAN_MOSQUITO_RATIO  0.04

struct human_population_newborn {
  double uninfected;
};

struct human_population_toddler_normal {
  double uninfected;
  struct conveyor *tainted_conv;
  double tainted;
  double hosts;
  double pimmune;
  double remission;
};

struct human_population_toddler_sicklecell {
  double uninfected;
  struct conveyor *tainted_conv;
  double tainted;
  double hosts;
  double pimmune;
  double remission;
};

struct human_population_adult {
  double uninfected;
  struct conveyor *tainted_conv;
  double tainted;
  double hosts;
  double pimmune;
  double remission;
};

struct mosquito_population {
  double uninfected;
  struct conveyor *tainted_conv;
  double tainted;
  double vectors;
};

extern struct human_population_newborn *h_pop_newb;
extern struct human_population_toddler_normal *h_pop_tod_n;
extern struct human_population_toddler_sicklecell *h_pop_tod_s;
extern struct human_population_adult *h_pop_adult;
extern struct mosquito_population *m_pop;

void prepare_round( double td );

double human_newborn_uninfected( double td );

double human_toddler_n_uninfected( double td );
void human_toddler_n_tainted( double td );
double human_toddler_n_hosts( double td );
double human_toddler_n_pimmune( double td );
double human_toddler_n_remission( double td );

double human_toddler_s_uninfected( double td );
void human_toddler_s_tainted( double td );
double human_toddler_s_hosts( double td );
double human_toddler_s_pimmune( double td );
double human_toddler_s_remission( double td );

double human_adult_uninfected( double td );
void human_adult_tainted( double td );
double human_adult_hosts( double td );
double human_adult_pimmune( double td );
double human_adult_remission( double td );

double mosquito_uninfected( double td );
void mosquito_tainted( double td );
double mosquito_vectors( double td );

#endif
