#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conveyor.h"

static struct conveyor_object *dequeue( struct conveyor_queue *q )
{
  struct conveyor_object *obj;

  if (q->n == 0)
    return NULL;

  obj = &q->queue[q->tail];
  q->total_value -= obj->value;
  q->n--;
  q->tail = (q->tail + 1) % q->max;

  return obj;
}

static void enqueue( struct conveyor_queue *q, double value, double limit)
{
  if (q->n == q->max)
  {
    fprintf(stderr, "conveyor enqueue: attempting to enqueue (max %d) on a full queue!\n", q->max);
    exit(1);
  }

  q->total_value += value;
  q->queue[q->head].value = value;
  q->queue[q->head].limit = limit;
  q->n++;
  q->head = (q->head + 1) % q->max;
}

static struct conveyor_object *queue_peek( struct conveyor_queue *q )
{
  if ( q->n == 0 )
    return NULL;
  return &q->queue[q->tail];
}

/* Initialize the conveyor. Make sure that size allows for enough space.
   At least ((1 / td) * conv_time) * add_influx_per_td */
void initialize_conveyor( struct conveyor *conv, double conv_time, double outflux_rate, int size)
{
  /* Initialize struct conveyor */
  conv->time = 0.0;
  conv->conv_time = conv_time;
  conv->outflux_rate = outflux_rate;
  conv->queue = malloc( sizeof(struct conveyor_queue) );
  if (!conv->queue)
  {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }

  /* Initialize struct conveyor_queue */
  conv->queue->total_value = 0.0;
  conv->queue->head = 0;
  conv->queue->tail = 0;
  conv->queue->n = 0;
  conv->queue->max = size;
  conv->queue->queue = malloc(sizeof(struct conveyor_object) * size);
  if (!conv->queue->queue)
  {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }
}

void update_conveyor( struct conveyor *conv, double td )
{
  int i, n;
  struct conveyor_object *obj;
  struct conveyor_queue *q;
  double outflux;

  /* Update outflux for each item */
  q = conv->queue;
  i = q->tail;
  n = q->n;
  while (n)
  {
    obj = &q->queue[i];
    outflux = obj->value * (conv->outflux_rate * td);
    obj->value = obj->value - outflux;
    q->total_value -= outflux;
    n--;
    i = (i + 1) % q->max;
  }

  /* Update conveyor time */
  conv->time += td;
}

double conveyor_get_outflux( struct conveyor *conv )
{
  struct conveyor_object *obj;
  double ret = 0.0;
  
  while ((obj = queue_peek(conv->queue)) != NULL)
  {
    /* Oldest object is not yet done */
    if (obj->limit > conv->time)
      break;

    /* Dequeue and add the value to the return value */
    obj = dequeue(conv->queue);
    ret += obj->value;
  }

  /* Return object value plus any other conveyor objects ready to be released */
  return ret;
}

void conveyor_add_influx( double value, struct conveyor *conv )
{
  enqueue(conv->queue, value, conv->time + conv->conv_time);
}

double conveyor_get_population( struct conveyor *conv )
{
  return conv->queue->total_value;
}

