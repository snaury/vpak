#include <windows.h>

size_t wcslen(const wchar_t* s)
{
  return lstrlenW(s);
}
