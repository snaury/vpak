#include <windows.h>

void free(void* p)
{
  if (p) {
    HeapFree(GetProcessHeap(), 0, p);
  }
}
