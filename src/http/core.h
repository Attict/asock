#ifndef AHTTP_CORE_H
#define AHTTP_CORE_H

#include "../ahttp.h"

#define AHTTP_MAX_HEADERS 50
#define AHTTP_MAX_URL_SEGMENTS 100

/**
 * METHODS
 *
 * @brief
 */
#define AHTTP_METHOD_UNKNOWN        0x0001
#define AHTTP_METHOD_GET            0x0002
#define AHTTP_METHOD_HEAD           0x0004
#define AHTTP_METHOD_POST           0x0008
#define AHTTP_METHOD_PUT            0x0010
#define AHTTP_METHOD_DELETE         0x0020
#define AHTTP_METHOD_MKCOL          0x0040
#define AHTTP_METHOD_COPY           0x0080
#define AHTTP_METHOD_MOVE           0x0100
#define AHTTP_METHOD_OPTIONS        0x0200

/**
 * ahttp_socket_t
 *
 * @brief
 */
typedef struct ahttp_socket_t
{
  int offset;
}
ahttp_socket_t;

/**
 * ahttp_context_t
 *
 * @brief
 */
typedef struct ahttp_context_t
{
  char *response;
  int length;
}
ahttp_context_t;

/**
 * ahttp_header_t
 *
 * @brief
 */
typedef struct ahttp_header_t
{
  char* name;
  char* value;
}
ahttp_header_t;

/**
 * ahttp_request_t
 *
 * @brief
 */
typedef struct ahttp_request_t
{
  unsigned int method;

  //ahttp_header_t headers[AHTTP_MAX_HEADERS];
  //char *version;
  //char *uri;
  //char *body;
}
ahttp_request_t;

/**
 * ahttp_response_t
 *
 * @brief
 */
typedef struct ahttp_response_t
{
  ahttp_header_t headers[AHTTP_MAX_HEADERS];
}
ahttp_response_t;

/**
 * AHTTP_STATUS
 *
 * @brief
 */
enum
{
  AHTTP_STATUS_OK = 200,
  AHTTP_STATUS_CREATED = 201,
  AHTTP_STATUS_ACCEPTED = 202,
  AHTTP_STATUS_NO_CONTENT = 204,
  AHTTP_STATUS_MULTIPLE_CHOICES = 300,
  AHTTP_STATUS_MOVED_PERMANENTLY = 301,
  AHTTP_STATUS_MOVED_TEMPORARILY = 302,
  AHTTP_STATUS_NOT_MODIFIED = 304,
  AHTTP_STATUS_BAD_REQUEST = 400,
  AHTTP_STATUS_UNAUTHORIZED = 401,
  AHTTP_STATUS_FORBIDDEN = 403,
  AHTTP_STATUS_NOT_FOUND = 404,
  AHTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
  AHTTP_STATUS_NOT_IMPLEMENTED = 501,
  AHTTP_STATUS_BAD_GATEWAY = 502,
  AHTTP_STATUS_SERVICE_UNAVAILABLE = 503
};

/**
 * ahttp_core_parse
 *
 * @brief
 *
 * @param data
 * @return A newly formed (a)http request object.
 */
ahttp_request_t *ahttp_core_parse(char *data);

#endif // AHTTP_CORE_H
