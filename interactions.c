#include "interactions.h"
#include "conveyor.h"

extern struct human_population *h_pop;
extern struct mosquito_population *m_pop;

static double human_population( void )
{
  return h_pop->uninfected + h_pop->hosts + h_pop->pimmune + h_pop->pimmune_hosts + h_pop->remission;
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
  return (h_pop->hosts + h_pop->pimmune_hosts) / human_population();
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
          - (h_pop->uninfected * (get_prob_bit() * td) * p_vector())
          + (h_pop->hosts * (RECOVERY_RATE * td))
          + (human_population() * (HUMAN_BIRTH_RATE * td))
          - (h_pop->uninfected * (HUMAN_DEATH_RATE * td));
}

double human_hosts( double td )
{
  return h_pop->hosts
          + ((h_pop->uninfected + h_pop->remission) * (get_prob_bit() * td) * p_vector())
          + (h_pop->remission * (RELAPSE_RATE * td))
          + (h_pop->pimmune * ((get_prob_bit() * td) * p_vector() * (1.0 - PIMMUNE_HOST)))
          - (h_pop->hosts
            * (( PIMMUNITY_RATE + MALARIA_INDUCED_DEATH_RATE + RECOVERY_RATE + HUMAN_DEATH_RATE + REMISSION_RATE) * td));
}

double human_pimmune( double td )
{
  return h_pop->pimmune
          + (h_pop->hosts * (PIMMUNITY_RATE * td))
          - (h_pop->pimmune * (get_prob_bit() * td) * p_vector())
          - (h_pop->pimmune * (HUMAN_DEATH_RATE * td));
}

double human_pimmune_hosts( double td )
{
  return h_pop->pimmune_hosts
          + (h_pop->pimmune * ((get_prob_bit() * td) * p_vector() * PIMMUNE_HOST))
          - (h_pop->pimmune_hosts * (RECOVERY_RATE * td))
          - (h_pop->pimmune_hosts * (HUMAN_DEATH_RATE * td));
}

double human_remission( double td )
{
  return h_pop->remission
          + (h_pop->hosts * (REMISSION_RATE * td))
          - (h_pop->remission * ((RELAPSE_RATE + HUMAN_DEATH_RATE) * td))
          - (h_pop->remission * (get_prob_bit() * td) * p_vector());
}

double mosquito_uninfected( double td )
{
  return m_pop->uninfected
          + ( mosquito_population() * (MOSQUITO_BIRTH_RATE * td) )
          - ( m_pop->uninfected
                * ((MOSQUITO_DEATH_RATE * td) + ( (get_prob_bite_human() * td) * p_host())) );
}

void mosquito_tainted( double td )
{
  conveyor_add_influx( m_pop->uninfected * (get_prob_bite_human() * td) * p_host() , m_pop->tainted_conv );
}

double mosquito_vectors( double td )
{
  return m_pop->vectors
          + conveyor_get_outflux( m_pop->tainted_conv )
          - (m_pop->vectors * (MOSQUITO_DEATH_RATE * td));
}

