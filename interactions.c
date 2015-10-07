#include "interactions.h"
#include "conveyor.h"

extern struct human_population *h_pop;
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
  return h_pop->uninfected + h_pop->tainted + h_pop->hosts + h_pop->pimmune + h_pop->remission;
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
  return (h_pop->hosts + h_pop->pimmune) / human_population();
}

void prepare_round( double td )
{
  /* Update conveyor timers */
  update_conveyor( h_pop->tainted_conv, td );
  update_conveyor( m_pop->tainted_conv, td );

  /* Pre-compute population of conveyors */
  h_pop->tainted = conveyor_get_population( h_pop->tainted_conv );
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

double human_uninfected( double td )
{
  return h_pop->uninfected
          - (h_pop->uninfected * (get_prob_bit_c * td) * p_vector_c)
          + (h_pop->hosts * (RECOVERY_RATE * td))
          + (human_population_c * (HUMAN_BIRTH_RATE * td))
          - (h_pop->uninfected * (HUMAN_DEATH_RATE * td));
}

void human_tainted( double td )
{
  conveyor_add_influx( (h_pop->uninfected + h_pop->remission) * (get_prob_bit_c * td) * p_vector_c, h_pop->tainted_conv );
}

double human_hosts( double td )
{
  return h_pop->hosts
          + conveyor_get_outflux( h_pop->tainted_conv )
          + (h_pop->remission * (RELAPSE_RATE * td))
          - (h_pop->hosts
            * (( PIMMUNITY_RATE + MALARIA_INDUCED_DEATH_RATE + RECOVERY_RATE + HUMAN_DEATH_RATE + REMISSION_RATE) * td));
}

double human_pimmune( double td )
{
  return h_pop->pimmune
          + (h_pop->hosts * (PIMMUNITY_RATE * td))
          - (h_pop->pimmune * (HUMAN_DEATH_RATE * td));
}

double human_remission( double td )
{
  return h_pop->remission
          + (h_pop->hosts * (REMISSION_RATE * td))
          - (h_pop->remission * ((RELAPSE_RATE + HUMAN_DEATH_RATE) * td))
          - (h_pop->remission * (get_prob_bit_c * td) * p_vector_c);
}

double mosquito_uninfected( double td )
{
  return m_pop->uninfected
          + ( mosquito_population_c * (MOSQUITO_BIRTH_RATE * td) )
          - ( m_pop->uninfected
                * ((MOSQUITO_DEATH_RATE * td) + ( (get_prob_bite_human_c * td) * p_host_c)) );
}

void mosquito_tainted( double td )
{
  conveyor_add_influx( m_pop->uninfected * (get_prob_bite_human_c * td) * p_host_c , m_pop->tainted_conv );
}

double mosquito_vectors( double td )
{
  return m_pop->vectors
          + conveyor_get_outflux( m_pop->tainted_conv )
          - (m_pop->vectors * (MOSQUITO_DEATH_RATE * td));
}

