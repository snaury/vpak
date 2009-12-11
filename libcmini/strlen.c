#include <windows.h>

size_t strlen(const char* s)
{
  return lstrlenA(s);
}
