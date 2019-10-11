#ifndef ASQL_CORE_H
#define ASQL_CORE_H

#include "../asql.h"

/**
 * asql_result_t
 *
 * @brief
 */
typedef struct asql_result_t
{
  int num_rows;
}
asql_result_t;

/**
 * asql_
 *
 * @brief
 */
int asql_open(const char *str);

/**
 * asql_
 *
 * @brief
 */
int asql_close();

/**
 * asql_
 *
 * @brief
 */
asql_result_t *asql_query(const char *str);

#endif // ASQL_CORE_H
