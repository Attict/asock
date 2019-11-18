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
#define AHTTP_METHOD_PROPFIND       0x0400
#define AHTTP_METHOD_PROPPATCH      0x0800
#define AHTTP_METHOD_LOCK           0x1000
#define AHTTP_METHOD_UNLOCK         0x2000
#define AHTTP_METHOD_PATCH          0x4000
#define AHTTP_METHOD_TRACE          0x8000

/**
 * Compare
 *
 * @brief
 */
#define ahttp_compare_str3(m, c0, c1, c2)                           \
    *(uint32_t *) m == ((c2 << 16) | (c1 << 8) | c0)

#define ahttp_compare_str4(m, c0, c1, c2, c3)                       \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)

#define ahttp_compare_str5(m, c0, c1, c2, c3, c4)                   \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) && m[4] == c4


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
  char *url;
  unsigned int major_version;
  unsigned int minor_version;

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
 * @param request The pointer to be assigned
 * @param data
 * @return
 */
int ahttp_core_parse(ahttp_request_t *request, char *data);

/**
 * ahttp_core_parse_method
 *
 * @brief
 *
 * @param request A pointer to the request object.
 * @param data The incoming data.
 * @param len The length of the method from the data.
 * @return
 */
int ahttp_core_parse_method(int *method, const char *data, int len);

/**
 * ahttp_core_parse_url
 *
 * @brief
 *
 * @param request A pointer to the request object.
 * @param data The incoming data.
 * @param len The length of the method from the data.
 * @return
 */
int ahttp_core_parse_url(char *url, const char *data, int start, int len);

/**
 * ahttp_core_parse_version
 *
 * @brief
 *
 * @param major
 * @param minor
 * @param data
 * @param len
 * @return
 */
int ahttp_core_parse_version(int *major, int *minor, const char *data, int len);

#endif // AHTTP_CORE_H
