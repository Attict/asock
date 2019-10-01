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





void *us_socket_ext(int ssl, struct us_socket_t *s) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return us_internal_ssl_socket_ext((struct us_internal_ssl_socket_t *) s);
  }
#endif

  return s + 1;
}
