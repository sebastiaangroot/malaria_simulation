#include "interactions.h"
#include "conveyor.h"

extern struct human_population_newborn *h_pop_newb;
extern struct human_population_toddler_normal *h_pop_tod_n;
extern struct human_population_toddler_sicklecell *h_pop_tod_s;
extern struct human_population_adult *h_pop_adult;
extern struct mosquito_population *m_pop;

/* Variables that are filled with precomputed values of round constants */
double human_population_c;
double mosquito_population_c;
double get_prob_bite_human_c;
double get_prob_bit_c;
double p_vector_c;
double p_host_c;

static double human_population( void )
{
  return h_pop_newb->uninfected + h_pop_tod_n->uninfected + h_pop_tod_n->tainted + h_pop_tod_n->hosts + h_pop_tod_n->pimmune + h_pop_tod_n->remission + h_pop_tod_s->uninfected + h_pop_tod_s->tainted + h_pop_tod_s->hosts + h_pop_tod_s->pimmune + h_pop_tod_s->remission + h_pop_adult->uninfected + h_pop_adult->tainted + h_pop_adult->hosts + h_pop_adult->pimmune + h_pop_adult->remission;
}

static double mosquito_population( void )
{
  return m_pop->uninfected + m_pop->tainted + m_pop->vectors;
}

static double get_prob_bite_human( void )
{
  double hsize = human_population();
  double msize = mosquito_population();

  if ((hsize / msize) >= HEALTHY_HUMAN_MOSQUITO_RATIO)
  {
    return PROB_BITE_HUMAN;
  }
  return (1.0 / HEALTHY_HUMAN_MOSQUITO_RATIO) * (hsize / msize) * PROB_BITE_HUMAN;
}

static double get_prob_bit( void )
{
  return (mosquito_population() / human_population()) * get_prob_bite_human();
}

/* Probability of a mosquito being a vector */
static double p_vector( void )
{
  return m_pop->vectors / mosquito_population();
}

/* Probability of a human being a host */
static double p_host( void )
{
  return (h_pop_tod_n->hosts + h_pop_tod_n->pimmune + h_pop_tod_s->hosts + h_pop_tod_s->pimmune + h_pop_adult->hosts + h_pop_adult->pimmune) / human_population();
}

void prepare_round( double td )
{
  /* Update conveyor timers */
  update_conveyor( h_pop_tod_n->tainted_conv, td );
  update_conveyor( h_pop_tod_s->tainted_conv, td );
  update_conveyor( h_pop_adult->tainted_conv, td );
  update_conveyor( m_pop->tainted_conv, td );

  /* Pre-compute population of conveyors */
  h_pop_tod_n->tainted = conveyor_get_population( h_pop_tod_n->tainted_conv );
  h_pop_tod_s->tainted = conveyor_get_population( h_pop_tod_s->tainted_conv );
  h_pop_adult->tainted = conveyor_get_population( h_pop_adult->tainted_conv );
  m_pop->tainted = conveyor_get_population( m_pop->tainted_conv );

  /* Pre-compute round constants */
  human_population_c = human_population();
  mosquito_population_c = mosquito_population();
  get_prob_bite_human_c = get_prob_bite_human();
  get_prob_bit_c = get_prob_bit();
  p_vector_c = p_vector();
  p_host_c = p_host();
}

/*
 * Population-change functions.
 * Each function calculates the population of a certain
 * group given the previous population state h_pop and
 * m_pop and time increment td.
 */

/* Newborn */
double human_newborn_uninfected( double td )
{
  //printf("newb(%f) -> %f | ", h_pop_newb->uninfected, h_pop_newb->uninfected * (FROM_NEWBORN_TO_TODDLER));
  return h_pop_newb->uninfected
          + (human_population_c * (HUMAN_BIRTH_RATE * td))
          - (h_pop_newb->uninfected * (HUMAN_DEATH_RATE * td))
          - (h_pop_newb->uninfected * (FROM_NEWBORN_TO_TODDLER * td));
}

/* Normal toddler */
double human_toddler_n_uninfected( double td )
{
  //printf("todn(%f) -> %f | ", h_pop_tod_n->uninfected, h_pop_tod_n->uninfected * (FROM_TODDLER_TO_ADULT * td));
  return h_pop_tod_n->uninfected
    + (h_pop_newb->uninfected * (FROM_NEWBORN_TO_TODDLER * td) * (1 - SICKLE_CELL_RATE))
    + (h_pop_tod_n->hosts * (RECOVERY_RATE * td))
    - (h_pop_tod_n->uninfected * (HUMAN_DEATH_RATE * td))
    - (h_pop_tod_n->uninfected * (FROM_TODDLER_TO_ADULT * td))
    - (h_pop_tod_n->uninfected * ((get_prob_bit_c * td) * p_vector_c));
}

void human_toddler_n_tainted( double td )
{
  conveyor_add_influx( (h_pop_tod_n->uninfected + h_pop_tod_n->remission) * (get_prob_bit_c * td) * p_vector_c, h_pop_tod_n->tainted_conv );
}

double human_toddler_n_hosts( double td )
{
  return h_pop_tod_n->hosts
    + conveyor_get_outflux( h_pop_tod_n->tainted_conv )
    + (h_pop_tod_n->remission * (RELAPSE_RATE * td))
    - (h_pop_tod_n->hosts
         * ((PIMMUNITY_RATE + MALARIA_INDUCED_DEATH_RATE_TODDLER + RECOVERY_RATE + HUMAN_DEATH_RATE + REMISSION_RATE + FROM_TODDLER_TO_ADULT) * td));
}

double human_toddler_n_pimmune( double td )
{
  return h_pop_tod_n->pimmune
    + (h_pop_tod_n->hosts * (PIMMUNITY_RATE * td))
    - (h_pop_tod_n->pimmune * ((FROM_TODDLER_TO_ADULT + HUMAN_DEATH_RATE) * td));
}

double human_toddler_n_remission( double td)
{
  return h_pop_tod_n->remission
    + (h_pop_tod_n->hosts * (REMISSION_RATE * td))
    - (h_pop_tod_n->remission * ((RELAPSE_RATE + HUMAN_DEATH_RATE + FROM_TODDLER_TO_ADULT) * td))
    - (h_pop_tod_n->remission * (get_prob_bit_c * td) * p_vector_c);
}

/* Toddler with sickle cell anemia*/
double human_toddler_s_uninfected( double td )
{
  //printf("tods(%f) -> %f\n", h_pop_tod_s->uninfected, h_pop_tod_s->uninfected * (FROM_TODDLER_TO_ADULT * td));
  return h_pop_tod_s->uninfected
    + (h_pop_newb->uninfected * (FROM_NEWBORN_TO_TODDLER * td) * (SICKLE_CELL_RATE))
    + (h_pop_tod_s->hosts * (RECOVERY_RATE * td))
    - (h_pop_tod_s->uninfected * (HUMAN_DEATH_RATE * td))
    - (h_pop_tod_s->uninfected * (FROM_TODDLER_TO_ADULT * td))
    - (h_pop_tod_s->uninfected * ((get_prob_bit_c * td) * p_vector_c));
}

void human_toddler_s_tainted( double td )
{
  conveyor_add_influx( (h_pop_tod_s->uninfected + h_pop_tod_s->remission) * (get_prob_bit_c * td) * p_vector_c, h_pop_tod_s->tainted_conv );
}

double human_toddler_s_hosts( double td )
{
  return h_pop_tod_s->hosts
    + conveyor_get_outflux( h_pop_tod_s->tainted_conv )
    + (h_pop_tod_s->remission * (RELAPSE_RATE * td))
          - (h_pop_tod_s->hosts
          * ((PIMMUNITY_RATE + RECOVERY_RATE + HUMAN_DEATH_RATE + REMISSION_RATE + FROM_TODDLER_TO_ADULT) * td))
    - (h_pop_tod_s->hosts * (MALARIA_INDUCED_DEATH_RATE_TODDLER * (1.0 - SICKLE_CELL_MALARIA_SURVIVAL_RATE) * td));
}

double human_toddler_s_pimmune( double td )
{
  return h_pop_tod_s->pimmune
          + (h_pop_tod_s->hosts * (PIMMUNITY_RATE * td))
          - (h_pop_tod_s->pimmune * ((FROM_TODDLER_TO_ADULT + HUMAN_DEATH_RATE) * td));
}

double human_toddler_s_remission( double td)
{
  return h_pop_tod_s->remission
          + (h_pop_tod_s->hosts * (REMISSION_RATE * td))
          - (h_pop_tod_s->remission * ((RELAPSE_RATE + HUMAN_DEATH_RATE + FROM_TODDLER_TO_ADULT) * td))
          - (h_pop_tod_s->remission * (get_prob_bit_c * td) * p_vector_c);
}

/* Adults */
double human_adult_uninfected( double td)
{
  return h_pop_adult->uninfected
    + ((h_pop_tod_n->uninfected + h_pop_tod_s->uninfected) * (FROM_TODDLER_TO_ADULT * td))
    + (h_pop_adult->hosts * (RECOVERY_RATE * td))
    - (h_pop_adult->uninfected * (HUMAN_DEATH_RATE * td))
    - (h_pop_adult->uninfected * ((get_prob_bit_c * td) * p_vector_c));
}

void human_adult_tainted( double td )
{
  conveyor_add_influx( (h_pop_adult->uninfected + h_pop_adult->remission) * (get_prob_bit_c * td) * p_vector_c, h_pop_adult->tainted_conv );
}

double human_adult_hosts( double td )
{
  return h_pop_adult->hosts
    + ((h_pop_tod_n->hosts + h_pop_tod_s->hosts) * (FROM_TODDLER_TO_ADULT * td))
    + conveyor_get_outflux( h_pop_adult->tainted_conv )
    + (h_pop_adult->remission * (RELAPSE_RATE * td))
    - (h_pop_adult->hosts
      * (( PIMMUNITY_RATE + MALARIA_INDUCED_DEATH_RATE + RECOVERY_RATE + HUMAN_DEATH_RATE + REMISSION_RATE) * td));
}

double human_adult_pimmune( double td )
{
  return h_pop_adult->pimmune
    + ((h_pop_tod_n->pimmune + h_pop_tod_s->pimmune) * (FROM_TODDLER_TO_ADULT * td))
          + (h_pop_adult->hosts * (PIMMUNITY_RATE * td))
          - (h_pop_adult->pimmune * (HUMAN_DEATH_RATE * td));
}


double human_adult_remission( double td )
{
  return h_pop_adult->remission
    + ((h_pop_tod_n->remission + h_pop_tod_s->remission) * (FROM_TODDLER_TO_ADULT * td))
          + (h_pop_adult->hosts * (REMISSION_RATE * td))
          - (h_pop_adult->remission * ((RELAPSE_RATE + HUMAN_DEATH_RATE) * td))
          - (h_pop_adult->remission * (get_prob_bit_c * td) * p_vector_c);
}

double mosquito_uninfected( double td )
{
  return m_pop->uninfected
          + ( mosquito_population_c * (MOSQUITO_BIRTH_RATE * td) )
          - ( m_pop->uninfected * (MOSQUITO_DEATH_RATE * td))
          - ( m_pop->uninfected * ((get_prob_bite_human_c * td) * p_host_c));
}

void mosquito_tainted( double td )
{
  conveyor_add_influx( m_pop->uninfected * ((get_prob_bite_human_c * td) * p_host_c) , m_pop->tainted_conv );
}

double mosquito_vectors( double td )
{
  return m_pop->vectors
          + conveyor_get_outflux( m_pop->tainted_conv )
          - (m_pop->vectors * (MOSQUITO_DEATH_RATE * td));
}

