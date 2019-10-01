/*
 * Authored by Alex Hultman, 2018-2019.
 * Intellectual property of third-party.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "asock.h"
#include "us_internal.h"
#include "core.h"
#include <stdlib.h>


/* Note: This will set timeout to 0 */
struct us_socket_t *us_socket_context_adopt_socket(int ssl, struct us_socket_context_t *context, struct us_socket_t *s, int ext_size) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return (struct us_socket_t *) us_internal_ssl_socket_context_adopt_socket((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t *) s, ext_size);
  }
#endif

  /* Cannot adopt a closed socket */
  if (asock_socket_is_closed(ssl, s)) {
    return s;
  }

  /* This properly updates the iterator if in on_timeout */
  asock_context_unlink(s->context, s);

  struct us_socket_t *new_s = (struct us_socket_t *) asock_poll_resize(&s->p, s->context->loop, sizeof(struct us_socket_t) + ext_size);

  asock_context_link(context, new_s);

  return new_s;
}
