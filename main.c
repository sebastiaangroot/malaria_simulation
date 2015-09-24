#include <stdio.h>
#include <stdlib.h>
#include "interactions.h"
#include "conveyor.h"

struct human_population *h_pop;     /* Human population */
struct mosquito_population *m_pop;  /* Mosquito population */

void timestep(double td)
{
  struct human_population h_new;
  struct mosquito_population m_new;

  /* Step 1: Get current population in conveyors */
  m_pop->tainted = conveyor_get_population( m_pop->tainted_conv );

  /* Step 2: Update conveyor timers and outflux */
  update_conveyor( m_pop->tainted_conv, td );

  /* Step 3: Calculate new population */
  h_new.uninfected = human_uninfected( td );
  h_new.hosts = human_hosts( td );
  h_new.pimmune = human_pimmune( td );
  h_new.pimmune_hosts = human_pimmune_hosts( td );
  h_new.remission = human_remission( td );

  m_new.uninfected = mosquito_uninfected( td );
  mosquito_tainted( td );
  m_new.vectors = mosquito_vectors( td );


  /* Step 4: Update h_pop and m_pop */
  *h_pop = h_new;
  m_pop->uninfected = m_new.uninfected;
  m_pop->vectors = m_new.vectors;
}

int main( void )
{
  double t;
  double report_interval = REPORT_INTERVAL;
 
  h_pop = malloc(sizeof(struct human_population));
  m_pop = malloc(sizeof(struct mosquito_population));

  if (!h_pop || !m_pop )
  {
    fprintf(stderr, "failed to allocate memory during init\n");
    exit(1);
  }

  m_pop->tainted_conv = malloc(sizeof(struct conveyor));

  if (!m_pop->tainted_conv)
  {
    fprintf(stderr, "failed to allocate memory during init\n");
    exit(1);
  }

  /* Initial population */
  h_pop->uninfected = HUMAN_UNINFECTED_0;
  h_pop->hosts = HUMAN_HOSTS_0;
  h_pop->pimmune = HUMAN_PIMMUNE_0;
  h_pop->pimmune_hosts = HUMAN_PIMMUNE_HOSTS_0;
  h_pop->remission = HUMAN_REMISSION_0;

  m_pop->uninfected = MOSQUITO_UNINFECTED_0;
  m_pop->tainted = MOSQUITO_TAINTED_0;
  m_pop->vectors = MOSQUITO_VECTORS_0;

  initialize_conveyor( 
    m_pop->tainted_conv,      /* struct conveyor * */
    MOSQUITO_INCUBATION_TIME, /* time spent in conveyor */
    MOSQUITO_DEATH_RATE,      /* outflux rate while in the conveyor */
    (int)(((1.0 / TIME_D) * MOSQUITO_INCUBATION_TIME) + 2.0) /* size of conveyor queue */
  );

  /* Print CSV header */
  printf("time,human_uninfected,human_hosts,human_pimmune,human_pimmune_hosts,human_remission,"
          "mosquito_uninfected,mosquito_tainted,mosquito_vectors\n");

  /* Main simulation loop */
  for (t = TIME_START; t <= (TIME_MAX + (TIME_D / 10.0)); t += TIME_D)
  {
    timestep(TIME_D);

    /* Every REPORT_INTERVAL timesteps, print simulation progression */
    if (t > report_interval - (TIME_D / 10.0)
        && t < report_interval + (TIME_D / 10.0))
    {
      printf("%f,%f,%f,%f,%f,%f,%f,%f,%f\n",t,h_pop->uninfected,h_pop->hosts,
              h_pop->pimmune,h_pop->pimmune_hosts,h_pop->remission,m_pop->uninfected,conveyor_get_population(m_pop->tainted_conv),m_pop->vectors);
      report_interval += REPORT_INTERVAL;
    }
  }

  /* Final reporting */
  fprintf(stderr, "Time:                      %f\n", t - TIME_D);
  fprintf(stderr, "Human population\n");
  fprintf(stderr, " - Uninfected:             %f\n", h_pop->uninfected);
  fprintf(stderr, " - Hosts:                  %f\n", h_pop->hosts);
  fprintf(stderr, " - Partially immune:       %f\n", h_pop->pimmune);
  fprintf(stderr, " - Partially immune hosts: %f\n", h_pop->pimmune_hosts);
  fprintf(stderr, " - Remission:              %f\n", h_pop->remission);

  fprintf(stderr, "\nMosquito population\n");
  fprintf(stderr, " - Uninfected:             %f\n", m_pop->uninfected);
  fprintf(stderr, " - Tainted  :              %f\n", conveyor_get_population(m_pop->tainted_conv));
  fprintf(stderr, " - Vectors:                %f\n", m_pop->vectors);

  return 0;
}

