#include "asock.h"
#include <stdlib.h>


void asock_loop_data_init(asock_loop_t* loop,
  void (*wakeup_cb)(asock_loop_t* loop), void (*pre_cb)(asock_loop_t* loop),
  void (*post_cb)(asock_loop_t* loop))
{
  //loop->data.sweep_timer = asock_create_timer(loop, 1, 0);
  //loop->data.
}

/**
 * Creates a loop.
 *
 * @brief: TODO
 */
asock_loop_t* asock_create_loop(void* hint,
  void (*wakeup_cb)(asock_loop_t* loop), void (*pre_cb)(asock_loop_t* loop),
  void (*post_cb)(asock_loop_t* loop), unsigned int ext_size)
{
  asock_loop_t* loop = (asock_loop_t*) malloc(sizeof(asock_loop_t) + ext_size);
  loop->num_polls = 0;
  loop->fd = kqueue();
  asock_loop_data_init(loop, wakeup_cb, pre_cb, post_cb);
  return loop;
}
