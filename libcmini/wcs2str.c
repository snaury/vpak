#include <windows.h>

char* wcs2str(const wchar_t* s)
{
  int len;
  size_t wlen;
  char* r;
  if (!s)
    return NULL;
  wlen = wcslen(s);
  if (!wlen) {
    /* MBTWC will fail when len=0 */
    r = malloc(1);
    *r = '\0';
    return r;
  }
  len = WideCharToMultiByte(CP_ACP, 0, s, wlen, NULL, 0, NULL, NULL);
  r = malloc(len + 1);
  if (len)
    len = WideCharToMultiByte(CP_ACP, 0, s, wlen, r, len, NULL, NULL);
  r[len] = '\0';
  return r;
}
