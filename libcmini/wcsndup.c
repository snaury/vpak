#include <windows.h>

wchar_t* wcsndup(const wchar_t* s, size_t n)
{
  if (!s)
    return NULL;
  r = malloc(n * 2 + 2);
  p = r;
  while (n--)
    *p++ = *s++;
  *p = L'\0';
  return r;
}
