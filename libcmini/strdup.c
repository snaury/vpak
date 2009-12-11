#include <windows.h>

char* strdup(const char* s)
{
  size_t len;
  char* r;
  char* p;
  if (!s)
    return NULL;
  len = strlen(s);
  r = malloc(len + 1);
  p = r;
  while (*s)
    *p++ = *s++;
  *p = '\0';
  return r;
}
