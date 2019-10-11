#include "ssl.h"
#include <string.h>

int asock_passphrase_cb(char *buf, int size, int rwflag, void *u)
{
  const char *passphrase = (const char *) u;
  int passphrase_length = strlen(passphrase);
  memcpy(buf, passphrase, passphrase_length);
  return passphrase_length;
}

