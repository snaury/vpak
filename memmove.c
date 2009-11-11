#include <stddef.h>

void* memmove(void* dst, const void* src, size_t size)
{
  char* dstp = dst;
  const char* srcp = src;
  if (srcp < dstp && dstp < (srcp + size)) {
    srcp += size - 1;
    dstp += size - 1;
    while (size) {
      *dstp-- = *srcp--;
      --size;
    }
  } else {
    while (size) {
      *dstp++ = *srcp++;
      --size;
    }
  }
  return dst;
}
