#include <windows.h>

char* strndup(const char* s, size_t n)
{
  if (!s)
    return NULL;
  r = malloc(n + 1);
  p = r;
  while (n--)
    *p++ = *s++;
  *p = '\0';
  return r;
}
