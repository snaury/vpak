#include <windows.h>

void* malloc(size_t size)
{
  return HeapAlloc(GetProcessHeap(), 0, size);
}
