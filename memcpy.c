#include <stddef.h>

void* memcpy(void* dst, const void* src, size_t size)
{
  char* dstp = dst;
  const char* srcp = src;
  while (size) {
    *dstp++ = *srcp++;
    --size;
  }
  return dst;
}
