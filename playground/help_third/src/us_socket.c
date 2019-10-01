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
#include "core.h"
#include "poll.h"
#include "us_internal.h"
#include <stdlib.h>

struct us_socket_context_t *us_socket_context(int ssl, struct us_socket_t *s) {
  return s->context;
}

void us_socket_timeout(int ssl, struct us_socket_t *s, unsigned int seconds) {
  if (seconds) {
    unsigned short timeout_sweeps = 0.5f + ((float) seconds) / ((float) LIBUS_TIMEOUT_GRANULARITY);
    s->timeout = timeout_sweeps ? timeout_sweeps : 1;
  } else {
    s->timeout = 0;
  }
}

void us_socket_flush(int ssl, struct us_socket_t *s) {
  if (!asock_socket_is_shutdown(0, s)) {
    asock_core_socket_flush(asock_poll_fd((struct us_poll_t *) s));
  }
}

/* Not shared with SSL */

int us_socket_write(int ssl, struct us_socket_t *s, const char *data, int length, int msg_more) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return us_internal_ssl_socket_write((struct us_internal_ssl_socket_t *) s, data, length, msg_more);
  }
#endif

  if (asock_socket_is_closed(ssl, s) || asock_socket_is_shutdown(ssl, s)) {
    return 0;
  }

  int written = asock_core_send(asock_poll_fd(&s->p), data, length, msg_more);
  if (written != length) {
    s->context->loop->data.last_write_failed = 1;
    asock_poll_change(&s->p, s->context->loop, LIBUS_SOCKET_READABLE | LIBUS_SOCKET_WRITABLE);
  }

  return written < 0 ? 0 : written;
}

void *us_socket_ext(int ssl, struct us_socket_t *s) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return us_internal_ssl_socket_ext((struct us_internal_ssl_socket_t *) s);
  }
#endif

  return s + 1;
}
