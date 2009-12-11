#include <windows.h>

wchar_t* wcsdup(const wchar_t* s)
{
  size_t len;
  wchar_t* r;
  wchar_t* p;
  if (!s)
    return NULL;
  len = wcslen(s);
  r = malloc(len * 2 + 2);
  p = r;
  while (*s)
    *p++ = *s++;
  *p = L'\0';
  return r;
}
