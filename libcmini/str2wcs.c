#include <windows.h>

wchar_t* str2wcs(const char* s)
{
  int wlen;
  size_t len;
  wchar_t* r;
  if (!s)
    return NULL;
  len = strlen(s);
  if (!len) {
    /* MBTWC will fail when len=0 */
    r = malloc(2);
    *r = L'\0';
    return r;
  }
  wlen = MultiByteToWideChar(CP_ACP, 0, s, len, NULL, 0);
  r = malloc(wlen * 2 + 2);
  if (wlen)
    wlen = MultiByteToWideChar(CP_ACP, 0, s, len, r, wlen);
  r[wlen] = L'\0';
  return r;
}
