#include "interactions.h"

extern struct human_population *h_pop;
extern struct mosquito_population *m_pop;

static double human_population( void )
{
  return h_pop->uninfected + h_pop->hosts + h_pop->immune;
}


static double mosquito_population( void )
{
  return m_pop->uninfected + m_pop->tainted + m_pop->vectors;
}

/* Probability of a mosquito being a vector */
static double p_vector( void )
{
  return m_pop->vectors / mosquito_population();
}

/* Probability of a human being a host */
static double p_host( void )
{
  return h_pop->hosts / human_population();
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
          - (h_pop->uninfected * (PROB_BIT * td) * p_vector())
          + (h_pop->hosts * (RECOVERY_RATE * td))
          + (human_population() * (HUMAN_BIRTH_RATE * td))
          - (h_pop->uninfected * (HUMAN_DEATH_RATE * td));
}

double human_hosts( double td )
{
  return h_pop->hosts
          + (h_pop->uninfected * (PROB_BIT * td) * p_vector())
          - (h_pop->hosts
            * (( IMMUNITY_RATE + MALARIA_INDUCED_DEATH_RATE + RECOVERY_RATE + HUMAN_DEATH_RATE) * td));
}

double human_immune( double td )
{
  return h_pop->immune
          + (h_pop->hosts * (IMMUNITY_RATE * td))
          - (h_pop->immune * (HUMAN_DEATH_RATE * td));
}

double mosquito_uninfected( double td )
{
  return m_pop->uninfected
          + ( mosquito_population() * (MOSQUITO_BIRTH_RATE * td) )
          - ( m_pop->uninfected
                * ((MOSQUITO_DEATH_RATE * td) + ( (PROB_BITE_HUMAN * td) * p_host())) );
}

double mosquito_tainted( double td )
{
  return m_pop->tainted
          + ( m_pop->uninfected * (PROB_BITE_HUMAN * td) * p_host())
          - ( m_pop->tainted 
                * ((MOSQUITO_DEATH_RATE * td) + (MOSQUITO_INFECTION_RATE * td)) );
}

double mosquito_vectors( double td )
{
  return m_pop->vectors
          + (m_pop->tainted * (MOSQUITO_INFECTION_RATE * td))
          - (m_pop->vectors * (MOSQUITO_DEATH_RATE * td));
}

