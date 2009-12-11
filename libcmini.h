#include <windows.h>

/*
 * part of C runtime, but redefine just in case
 * this way we can be sure signatures match
 */
extern void* memset(void*, int, size_t);
extern void* memcpy(void*, const void*, size_t);
extern void* memmove(void*, const void*, size_t);
extern void* malloc(size_t);
extern void free(void*);
extern size_t strlen(const char*);
extern size_t wcslen(const wchar_t*);
extern char* strdup(const char*);
extern wchar_t* wcsdup(const wchar_t*);

/*
 * not part of C runtime
 */
extern char* strndup(const char* s, size_t n);
extern wchar_t* wcsndup(const wchar_t* s, size_t n);
extern wchar_t* str2wcs(const char*);
extern char* wcs2str(const wchar_t*);
