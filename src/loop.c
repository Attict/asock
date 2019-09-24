#include "loop.h"
#include <stdlib.h>

/**
 * Creates a loop.
 *
 * @brief: TODO
 */
asock_loop_t* asock_create_loop(
  void* hint,
  void (*wakup_cb)(asock_loop_t *loop),
  void (*pre_cb)(asock_loop_t *loop),
  void (*post_cb)(asock_loop_t *loop),
  unsigned int ext_size
)
{
  asock_loop_t* loop = (asock_loop_t*) malloc(sizeof(asock_loop_t) + ext_size);
  loop->num_polls = 0;


  return loop;
}
