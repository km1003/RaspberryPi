#include <sys/time.h>

#include "time.h"


uint64_t getuSecs()
{
  struct timeval tv;
  uint64_t t1;

  gettimeofday(&tv, NULL);
  t1 = (tv.tv_sec * 1000000 + tv.tv_usec);
  return t1;
}

