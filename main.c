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
  m_new.incubated = mosquito_incubated( td );

  *h_pop = h_new;
  *m_pop = m_new;
}

int main( void )
{
  double t;
  double report_interval = REPORT_INTERVAL;
 
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
  m_pop->incubated = MOSQUITO_INCUBATED_0;
  m_pop->vectors = MOSQUITO_VECTORS_0;

  /* Print CSV header */
  printf("time,human_uninfected,human_hosts,human_immune,"
          "mosquito_uninfected,mosquito_incubated,mosquito_vectors\n");

  /* Main simulation loop */
  for (t = TIME_START; t < TIME_MAX; t += TIME_D)
  {
    timestep(TIME_D);

    /* Every REPORT_INTERVAL timesteps, print simulation progression */
    if (t > report_interval - (TIME_D / 10.0)
        && t < report_interval + (TIME_D / 10.0))
    {
      printf("%f,%f,%f,%f,%f,%f,%f\n",t,h_pop->uninfected,h_pop->hosts,
              h_pop->immune,m_pop->uninfected,m_pop->incubated,m_pop->vectors);
      report_interval += REPORT_INTERVAL;
    }
  }

  /* Final reporting */
  fprintf(stderr, "Time:          %f\n", t - TIME_D);
  fprintf(stderr, "Human population\n");
  fprintf(stderr, " - Uninfected: %f\n", h_pop->uninfected);
  fprintf(stderr, " - Hosts:      %f\n", h_pop->hosts);
  fprintf(stderr, " - Immune:     %f\n", h_pop->immune);

  fprintf(stderr, "\nMosquito population\n");
  fprintf(stderr, " - Uninfected: %f\n", m_pop->uninfected);
  fprintf(stderr, " - Incubated:  %f\n", m_pop->incubated);
  fprintf(stderr, " - Vectors:    %f\n", m_pop->vectors);

  return 0;
}

