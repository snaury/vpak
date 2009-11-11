#include <stddef.h>

void* memset(void* dst, int val, size_t size)
{
  char* dstp = dst;
  while (size) {
    *dstp++ = val;
    --size;
  }
  return dst;
}
