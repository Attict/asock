#ifndef ASOCK_LOOP_H
#define ASOCK_LOOP_H

#include "asock.h"

void asock_loop_integrate(asock_loop_t *loop)
{
  asock_timer_set(loop->data.sweep_timer,
      (void (*)(asock_timer_t *)) sweep_timer_cb, A
}

#endif // ASOCK_LOOP_H
