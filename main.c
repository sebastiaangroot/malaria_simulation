#include <stdio.h>
#include <stdlib.h>
#include "interactions.h"
#include "conveyor.h"

struct human_population_newborn *h_pop_newb; /* Human newborns population */
struct human_population_toddler_normal *h_pop_tod_n; /*Human toddlers without sickle cell population */
struct human_population_toddler_sicklecell *h_pop_tod_s; /*Human toddler with sickle cell population */
struct human_population_adult *h_pop_adult; /* Human adult population */

struct mosquito_population *m_pop;  /* Mosquito population */

void timestep(double td)
{
  struct human_population_newborn h_new_newb;
  struct human_population_toddler_normal h_new_tod_n;
  struct human_population_toddler_normal h_new_tod_s;
  struct human_population_toddler_normal h_new_adult;
  struct mosquito_population m_new;

  /* Step 1: Round update pre-work */
  prepare_round( td );

  /* Step 2: Calculate new population */
  h_new_newb.uninfected = human_newborn_uninfected( td );

  h_new_tod_n.uninfected = human_toddler_n_uninfected( td );
  human_toddler_n_tainted( td );
  h_new_tod_n.hosts = human_toddler_n_hosts( td );
  h_new_tod_n.pimmune = human_toddler_n_pimmune( td );
  h_new_tod_n.remission = human_toddler_n_remission( td );

  h_new_tod_s.uninfected = human_toddler_s_uninfected( td );
  human_toddler_s_tainted( td );
  h_new_tod_s.hosts = human_toddler_s_hosts( td );
  h_new_tod_s.pimmune = human_toddler_s_pimmune( td );
  h_new_tod_s.remission = human_toddler_s_remission( td );

  h_new_adult.uninfected = human_adult_uninfected( td );
  human_adult_tainted( td );
  h_new_adult.hosts = human_adult_hosts( td );
  h_new_adult.pimmune = human_adult_pimmune( td );
  h_new_adult.remission = human_adult_remission( td );

  m_new.uninfected = mosquito_uninfected( td );
  mosquito_tainted( td );
  m_new.vectors = mosquito_vectors( td );

  /* Step 3: Update h_pop and m_pop */
  h_pop_newb->uninfected = h_new_newb.uninfected;

  h_pop_tod_n->uninfected = h_new_tod_n.uninfected;
  h_pop_tod_n->hosts = h_new_tod_n.hosts;
  h_pop_tod_n->pimmune = h_new_tod_n.pimmune;
  h_pop_tod_n->remission = h_new_tod_n.remission;

  h_pop_tod_s->uninfected = h_new_tod_s.uninfected;
  h_pop_tod_s->hosts = h_new_tod_s.hosts;
  h_pop_tod_s->pimmune = h_new_tod_s.pimmune;
  h_pop_tod_s->remission = h_new_tod_s.remission;

  h_pop_adult->uninfected = h_new_adult.uninfected;
  h_pop_adult->hosts = h_new_adult.hosts;
  h_pop_adult->pimmune = h_new_adult.pimmune;
  h_pop_adult->remission = h_new_adult.remission;

  m_pop->uninfected = m_new.uninfected;
  m_pop->vectors = m_new.vectors;
}

int main( void )
{
  double t;
  double report_interval = REPORT_INTERVAL;
 
  h_pop_newb = malloc(sizeof(struct human_population_newborn));
  h_pop_tod_n = malloc(sizeof(struct human_population_toddler_normal));
  h_pop_tod_s = malloc(sizeof(struct human_population_toddler_sicklecell));
  h_pop_adult = malloc(sizeof(struct human_population_adult));
  m_pop = malloc(sizeof(struct mosquito_population));

  if ( !h_pop_newb || !h_pop_tod_n || !h_pop_tod_s || !h_pop_adult || !m_pop )
  {
    fprintf(stderr, "failed to allocate memory during init\n");
    exit(1);
  }

  m_pop->tainted_conv = malloc(sizeof(struct conveyor));
  h_pop_tod_n->tainted_conv = malloc(sizeof(struct conveyor));
  h_pop_tod_s->tainted_conv = malloc(sizeof(struct conveyor));
  h_pop_adult->tainted_conv = malloc(sizeof(struct conveyor));

  if ( !m_pop->tainted_conv || !h_pop_adult->tainted_conv || !h_pop_tod_n->tainted_conv || !h_pop_tod_s->tainted_conv )
  {
    fprintf(stderr, "failed to allocate memory during init\n");
    exit(1);
  }

  /* Initial population */
  h_pop_newb->uninfected = HUMAN_NEWBORN_UNINFECTED_0;

  h_pop_tod_n->uninfected = HUMAN_TOD_N_UNINFECTED_0;
  h_pop_tod_n->tainted = HUMAN_TOD_N_TAINTED_0;
  h_pop_tod_n->hosts = HUMAN_TOD_N_HOSTS_0;
  h_pop_tod_n->pimmune = HUMAN_TOD_N_PIMMUNE_0;
  h_pop_tod_n->remission = HUMAN_TOD_N_REMISSION_0;

  h_pop_tod_s->uninfected = HUMAN_TOD_S_UNINFECTED_0;
  h_pop_tod_s->tainted = HUMAN_TOD_S_TAINTED_0;
  h_pop_tod_s->hosts = HUMAN_TOD_S_HOSTS_0;
  h_pop_tod_s->pimmune = HUMAN_TOD_S_PIMMUNE_0;
  h_pop_tod_s->remission = HUMAN_TOD_S_REMISSION_0;
  
  h_pop_adult->uninfected = HUMAN_ADULT_UNINFECTED_0;
  h_pop_adult->tainted = HUMAN_ADULT_TAINTED_0;
  h_pop_adult->hosts = HUMAN_ADULT_HOSTS_0;
  h_pop_adult->pimmune = HUMAN_ADULT_PIMMUNE_0;
  h_pop_adult->remission = HUMAN_ADULT_REMISSION_0;

  m_pop->uninfected = MOSQUITO_UNINFECTED_0;
  m_pop->tainted = MOSQUITO_TAINTED_0;
  m_pop->vectors = MOSQUITO_VECTORS_0;

  initialize_conveyor(
    h_pop_adult->tainted_conv,
    HUMAN_INCUBATION_TIME,
    HUMAN_DEATH_RATE,
    (int)(((1.0 / TIME_D) * HUMAN_INCUBATION_TIME) + 2.0)
  );

  initialize_conveyor(
    h_pop_tod_n->tainted_conv,
    HUMAN_INCUBATION_TIME,
    HUMAN_DEATH_RATE,
    (int)(((1.0 / TIME_D) * HUMAN_INCUBATION_TIME) + 2.0)
  );

  initialize_conveyor(
    h_pop_tod_s->tainted_conv,
    HUMAN_INCUBATION_TIME,
    HUMAN_DEATH_RATE,
    (int)(((1.0 / TIME_D) * HUMAN_INCUBATION_TIME) + 2.0)
  );

  initialize_conveyor( 
    m_pop->tainted_conv,      /* struct conveyor * */
    MOSQUITO_INCUBATION_TIME, /* time spent in conveyor */
    MOSQUITO_DEATH_RATE,      /* outflux rate while in the conveyor */
    (int)(((1.0 / TIME_D) * MOSQUITO_INCUBATION_TIME) + 2.0) /* size of conveyor queue */
  );

  /* Print CSV header */
  printf("time,"
        "human_newborn_uninfected,"
        "human_toddler_n_uninfected,human_toddler_n_tainted,human_toddler_n_hosts,human_toddler_n_pimmune,human_toddler_n_remission,"
        "human_toddler_s_uninfected,human_toddler_s_tainted,human_toddler_s_hosts,human_toddler_s_pimmune,human_toddler_s_remission,"
        "human_adult_uninfected,human_adult_tainted,human_adult_hosts,human_adult_pimmune,human_adult_remission,"
          "mosquito_uninfected,mosquito_tainted,mosquito_vectors\n");

  /* Main simulation loop */
  for (t = TIME_START; t <= (TIME_MAX + (TIME_D / 10.0)); t += TIME_D)
  {
    timestep(TIME_D);

    /* Every REPORT_INTERVAL timesteps, print simulation progression */
    if (t > report_interval - (TIME_D / 10.0)
        && t < report_interval + (TIME_D / 10.0))
    {
      printf("%f,%f,%f%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",t,
        h_pop_newb->uninfected,
        h_pop_tod_n->uninfected,conveyor_get_population(h_pop_tod_n->tainted_conv),h_pop_tod_n->hosts,h_pop_tod_n->pimmune,h_pop_tod_n->remission,
        h_pop_tod_s->uninfected,conveyor_get_population(h_pop_tod_s->tainted_conv),h_pop_tod_s->hosts,h_pop_tod_s->pimmune,h_pop_tod_s->remission,
        h_pop_adult->uninfected,conveyor_get_population(h_pop_adult->tainted_conv),h_pop_adult->hosts,h_pop_adult->pimmune,h_pop_adult->remission,
        m_pop->uninfected,conveyor_get_population(m_pop->tainted_conv),m_pop->vectors);
      report_interval += REPORT_INTERVAL;
    }
  }

  /* Final reporting */
  fprintf(stderr, "Time:                      %f\n", t - TIME_D);
  fprintf(stderr, "Human NEWBORN population\n");
  fprintf(stderr, " - Uninfected:             %f\n", h_pop_newb->uninfected);

  fprintf(stderr, "Human TODDLER NORMAL population\n");
  fprintf(stderr, " - Uninfected:             %f\n", h_pop_tod_n->uninfected);
  fprintf(stderr, " - Tainted:                %f\n", conveyor_get_population(h_pop_tod_n->tainted_conv));
  fprintf(stderr, " - Hosts:                  %f\n", h_pop_tod_n->hosts);
  fprintf(stderr, " - Partially immune:       %f\n", h_pop_tod_n->pimmune);
  fprintf(stderr, " - Remission:              %f\n", h_pop_tod_n->remission);

  fprintf(stderr, "Human TODDLER SICKLECELL population\n");
  fprintf(stderr, " - Uninfected:             %f\n", h_pop_tod_s->uninfected);
  fprintf(stderr, " - Tainted:                %f\n", conveyor_get_population(h_pop_tod_s->tainted_conv));
  fprintf(stderr, " - Hosts:                  %f\n", h_pop_tod_s->hosts);
  fprintf(stderr, " - Partially immune:       %f\n", h_pop_tod_s->pimmune);
  fprintf(stderr, " - Remission:              %f\n", h_pop_tod_s->remission);

  fprintf(stderr, "Human ADULT population\n");
  fprintf(stderr, " - Uninfected:             %f\n", h_pop_adult->uninfected);
  fprintf(stderr, " - Tainted:                %f\n", conveyor_get_population(h_pop_adult->tainted_conv));
  fprintf(stderr, " - Hosts:                  %f\n", h_pop_adult->hosts);
  fprintf(stderr, " - Partially immune:       %f\n", h_pop_adult->pimmune);
  fprintf(stderr, " - Remission:              %f\n", h_pop_adult->remission);

  fprintf(stderr, "\nMosquito population\n");
  fprintf(stderr, " - Uninfected:             %f\n", m_pop->uninfected);
  fprintf(stderr, " - Tainted:                %f\n", conveyor_get_population(m_pop->tainted_conv));
  fprintf(stderr, " - Vectors:                %f\n", m_pop->vectors);

  return 0;
}

