#include <windows.h>

wchar_t* wcsndup(const wchar_t* s, size_t n)
{
  wchar_t* r;
  wchar_t* p;
  if (!s)
    return NULL;
  r = malloc(n * 2 + 2);
  p = r;
  while (n--)
    *p++ = *s++;
  *p = L'\0';
  return r;
}
