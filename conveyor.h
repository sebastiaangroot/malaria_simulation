#ifndef CONVEYOR_H
#define CONVEYOR_H

struct conveyor_object {
  double value;
  double limit;
};

struct conveyor_queue {
  struct conveyor_object *queue;
  double total_value;
  int head;
  int tail;
  int n;
  int max;
};

struct conveyor {
  double time;
  double conv_time;
  double outflux_rate;
  struct conveyor_queue *queue;
};

void initialize_conveyor( struct conveyor *conv, double conv_time, double outflux_rate, int size );
void update_conveyor( struct conveyor *conv, double td );
double conveyor_get_outflux( struct conveyor *conv );
void conveyor_add_influx( double value, struct conveyor *conv );
double conveyor_get_population( struct conveyor *conv );

#endif
