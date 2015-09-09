#include <stdio.h>
#include <stdlib.h>
#include "interactions.h"

struct human_population *h_pop;
struct mosquito_population *m_pop;

void timestep(double td)
{
  struct human_population h_new;
  struct mosquito_population m_new;

  h_new.uninfected = human_uninfected( td );
  h_new.hosts = human_hosts( td );
  h_new.immune = human_immune( td );

  m_new.uninfected = mosquito_uninfected( td );
  m_new.vectors = mosquito_vectors( td );

  *h_pop = h_new;
  *m_pop = m_new;
}

int main( void )
{
  double t;
 
  h_pop = malloc(sizeof(struct human_population));
  m_pop = malloc(sizeof(struct mosquito_population));

  if (!h_pop || !m_pop)
  {
    fprintf(stderr, "failed to allocate memory during init\n");
    exit(1);
  }

  /* Initial population */
  h_pop->uninfected = HUMAN_UNINFECTED_0;
  h_pop->hosts = HUMAN_HOSTS_0;
  h_pop->immune = HUMAN_IMMUNE_0;

  m_pop->uninfected = MOSQUITO_UNINFECTED_0;
  m_pop->vectors = MOSQUITO_VECTORS_0;

  for (t = TIME_START; t < TIME_MAX; t += TIME_D)
  {
    timestep(TIME_D);
  }

  printf("Human population\n");
  printf(" - Uninfected: %f\n", h_pop->uninfected);
  printf(" - Hosts:      %f\n", h_pop->hosts);
  printf(" - Immune:     %f\n", h_pop->immune);

  printf("\nMosquito population\n");
  printf(" - Uninfected: %f\n", m_pop->uninfected);
  printf(" - Vectors:    %f\n", m_pop->vectors);

  return 0;
}

