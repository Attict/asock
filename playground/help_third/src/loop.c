#include "loop.h"
#include "timer.h"
#include <stdlib.h>

/**
 * asock_loop_free
 *
 * @note
 */
void asock_loop_free(asock_loop_t *loop)
{
  asock_loop_data_free(loop);
  close(loop->fd);
  free(loop);
}

/**
 * asock_loop_data_free
 *
 */
void asock_loop_data_free(asock_loop_t *loop)
{
  free(loop->data.recv_buf);

  asock_timer_close((asock_timer_t *) loop->data.sweep_timer);
  asock_async_close(loop->data.wakeup_async);
}

