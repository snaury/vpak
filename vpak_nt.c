#include <windows.h>
#include <ddk/ntddk.h>
#include <stdio.h>

#ifndef HEAP_CREATE_ENABLE_EXECUTE
// http://msdn.microsoft.com/en-us/library/aa366599(VS.85).aspx
#define HEAP_CREATE_ENABLE_EXECUTE 0x00040000
#endif

HANDLE vpak_exec_heap = 0;
inline HANDLE vpak_get_exec_heap() {
  if (!vpak_exec_heap)
    vpak_exec_heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE | HEAP_GENERATE_EXCEPTIONS, 0, 0);
  return vpak_exec_heap;
}

//////////////////////////////////////////////////////////////////////////////

#define VPAK_MODULE(module) \
  static HMODULE hModule_##module = NULL;

#define VPAK_HOOK(retval, name, args) \
  typedef retval (*type_##name) args; \
  static type_##name orig_##name = NULL; \
  static retval vpak_##name args

void dump_object_attributes(LPCWSTR Header, POBJECT_ATTRIBUTES ObjectAttributes) {
  if (ObjectAttributes && ObjectAttributes->ObjectName) {
    wprintf(L"%s: dir[%p]->'", Header, ObjectAttributes->RootDirectory);
    unsigned i;
    for (i = 0; i < ObjectAttributes->ObjectName->Length/2; ++i)
      putwchar(ObjectAttributes->ObjectName->Buffer[i]);
    wprintf(L"'\n");
  } else if (ObjectAttributes) {
    wprintf(L"%s: dir[%p]->[no-name]\n", Header, ObjectAttributes->RootDirectory);
  } else {
    wprintf(L"%s: <no-object-attributes>\n", Header);
  }
}

VPAK_HOOK(NTSTATUS DDKAPI, NtClose, (HANDLE Handle))
{
  wprintf(L"NtClose: %p\n", Handle);
  return orig_NtClose(Handle);
}

VPAK_HOOK(NTSTATUS DDKAPI, NtOpenFile, (PHANDLE FileHandle,
                                        ACCESS_MASK DesiredAccess,
                                        POBJECT_ATTRIBUTES ObjectAttributes,
                                        PIO_STATUS_BLOCK IoStatusBlock,
                                        ULONG ShareAccess,
                                        ULONG OpenOptions))
{
  dump_object_attributes(L"NtOpenFile", ObjectAttributes);
  return orig_NtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

VPAK_HOOK(NTSTATUS DDKAPI, NtCreateFile, (PHANDLE FileHandle,
                                          ACCESS_MASK DesiredAccess,
                                          POBJECT_ATTRIBUTES ObjectAttributes,
                                          PIO_STATUS_BLOCK IoStatusBlock,
                                          PLARGE_INTEGER AllocationSize,
                                          ULONG FileAttributes,
                                          ULONG ShareAccess,
                                          ULONG CreateDisposition,
                                          ULONG CreateOptions,
                                          PVOID EaBuffer,
                                          ULONG EaLength))
{
  dump_object_attributes(L"NtCreateFile", ObjectAttributes);
  return orig_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

VPAK_HOOK(NTSTATUS DDKAPI, NtQueryAttributesFile, (POBJECT_ATTRIBUTES ObjectAttributes,
                                                   PFILE_BASIC_INFORMATION FileInformation))
{
  dump_object_attributes(L"NtQueryAttributesFile", ObjectAttributes);
  return orig_NtQueryAttributesFile(ObjectAttributes, FileInformation);
}

VPAK_HOOK(NTSTATUS DDKAPI, NtQueryFullAttributesFile, (POBJECT_ATTRIBUTES ObjectAttributes,
                                                       PFILE_NETWORK_OPEN_INFORMATION FileInformation))
{
  dump_object_attributes(L"NtQueryFullAttributesFile", ObjectAttributes);
  return orig_NtQueryFullAttributesFile(ObjectAttributes, FileInformation);
}

VPAK_HOOK(NTSTATUS DDKAPI, NtOpenSection, (PHANDLE SectionHandle,
                                           ACCESS_MASK DesiredAccess,
                                           POBJECT_ATTRIBUTES ObjectAttributes))
{
  wprintf(L"NtOpenSection: SectionHandle=[%p] DesiredAccess=%08X ObjectAttributes=%p\n", SectionHandle, DesiredAccess, ObjectAttributes);
  if (ObjectAttributes)
    dump_object_attributes(L"NtOpenSection", ObjectAttributes);
  NTSTATUS status = orig_NtOpenSection(SectionHandle, DesiredAccess, ObjectAttributes);
  wprintf(L"NtOpenSection: opened section [%p]=%p\n", SectionHandle, SectionHandle ? *SectionHandle : NULL);
  return status;
}

VPAK_HOOK(NTSTATUS DDKAPI, NtCreateSection, (PHANDLE SectionHandle,
                                             ACCESS_MASK DesiredAccess,
                                             POBJECT_ATTRIBUTES ObjectAttributes,
                                             PLARGE_INTEGER MaximumSize,
                                             ULONG SectionPageProtection,
                                             ULONG AllocationAttributes,
                                             HANDLE FileHandle))
{
  wprintf(L"NtCreateSection: SectionHandle=[%p] DesiredAccess=%08X ObjectAttributes=%p MaximumSize=%p%p SectionPageProtection=%p AllocationAttributes=%p FileHandle=%p\n", SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize ? MaximumSize->HighPart : 0, MaximumSize ? MaximumSize->LowPart : 0, SectionPageProtection, AllocationAttributes, FileHandle);
  if (ObjectAttributes)
    dump_object_attributes(L"NtCreateSection", ObjectAttributes);
  NTSTATUS result = orig_NtCreateSection(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
  wprintf(L"NtCreateSection: created section [%p]=%p\n", SectionHandle, SectionHandle ? *SectionHandle : NULL);
  return result;
}

VPAK_HOOK(NTSTATUS DDKAPI, NtMapViewOfSection, (HANDLE SectionHandle,
                                                HANDLE ProcessHandle,
                                                PVOID *BaseAddress,
                                                ULONG_PTR ZeroBits,
                                                SIZE_T CommitSize,
                                                PLARGE_INTEGER SectionOffset,
                                                PSIZE_T ViewSize,
                                                SECTION_INHERIT InheritDisposition,
                                                ULONG AllocationType,
                                                ULONG Win32Protect))
{
  wprintf(L"NtMapViewOfSection: Section=%p Process=%p BaseAddress=[%p]=%p ZeroBits=%d CommitSize=%08x SectionOffset=[%p]=%p%p ViewSize=[%p]=%p InheritDisposition=%d AllocationType=%p Win32Protect=%p\n", SectionHandle, ProcessHandle, BaseAddress, BaseAddress ? (*BaseAddress) : NULL, ZeroBits, CommitSize, SectionOffset, SectionOffset ? SectionOffset->HighPart : 0, SectionOffset ? SectionOffset->LowPart : 0, ViewSize, ViewSize ? *ViewSize : 0, InheritDisposition, AllocationType, Win32Protect);
  NTSTATUS result = orig_NtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Win32Protect);
  wprintf(L"NtMapViewOfSection: mapped section [%p]=%p +%p%p size=%08x\n", BaseAddress, BaseAddress ? *BaseAddress : NULL, SectionOffset ? SectionOffset->HighPart : 0, SectionOffset ? SectionOffset->LowPart : 0, ViewSize ? (*ViewSize) : 0);
  return result;
}

VPAK_HOOK(NTSTATUS DDKAPI, NtUnmapViewOfSection, (HANDLE ProcessHandle,
                                                  PVOID BaseAddress))
{
  wprintf(L"NtUnmapViewOfSection: Process=%p BaseAddress=%p\n", ProcessHandle, BaseAddress);
  return orig_NtUnmapViewOfSection(ProcessHandle, BaseAddress);
}

//////////////////////////////////////////////////////////////////////////////

int vpak_nt_hook(const char* dllname, const char* apiname, void* vpak_func, void** orig_func)
{
  HMODULE Module = GetModuleHandle(dllname);
  if (!Module)
    return 0;
  PBYTE Target = (PBYTE)GetProcAddress(Module, apiname);
  if (!Target)
    return 0;
  PBYTE Original;
  switch (*Target) {
    case 0xE9: // jmp another_hook [someone hooked it before us]
      Original = HeapAlloc(vpak_get_exec_heap(), 0, 5);
      Original[0] = 0xE9; // jmp another_hook
      *(PLONG)(Original+1) = (Target+5) + *(PLONG)(Target+1) - (Original+5);
      break;
    case 0xB8: // mov eax, service_index [ntdll kernel jump prelude]
      Original = HeapAlloc(vpak_get_exec_heap(), 0, 10);
      Original[0] = 0xB8; // mov eax, service_index
      *(PLONG)(Original+1) = *(PLONG)(Target+1);
      Original[5] = 0xE9; // jmp target+5
      *(PLONG)(Original+6) = (Target+5) - (Original+10);
      break;
    default:
      // TODO: [wine] use some LDE engine to hook properly
      return 0;
  }
  DWORD dwOldProtect;
  if (!VirtualProtect(Target, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    return 0;
  *orig_func = (void*)Original;
  Target[0] = 0xE9; // jmp vpak_func
  *(PLONG)(Target+1) = (PBYTE)vpak_func - (Target+5);
  VirtualProtect(Target, 5, dwOldProtect, &dwOldProtect);
  return 1;
}

#define VPAK_NT_HOOK(name) \
  if (!vpak_nt_hook("ntdll", #name, (void*)&vpak_##name, (void**)&orig_##name)) do { \
    printf("Failed to hook ntdll.%s\n", #name); \
  } while(0)

//////////////////////////////////////////////////////////////////////////////

void vpak_initialize()
{
  VPAK_NT_HOOK(NtClose);
  VPAK_NT_HOOK(NtOpenFile);
  VPAK_NT_HOOK(NtCreateFile);
  VPAK_NT_HOOK(NtQueryAttributesFile);
  VPAK_NT_HOOK(NtQueryFullAttributesFile);
  VPAK_NT_HOOK(NtOpenSection);
  VPAK_NT_HOOK(NtCreateSection);
  VPAK_NT_HOOK(NtMapViewOfSection);
  VPAK_NT_HOOK(NtUnmapViewOfSection);
}

int main(int argc, char** argv) {
  vpak_initialize();
  LoadLibrary("msvcrt-ruby18.dll");
  FILE* f = fopen("test.txt", "wb");
  if (f) {
    fprintf(f, "Hello world!\n");
    fclose(f);
  }
  LPVOID addr = (LPVOID)0xccc000;
  VirtualAlloc(&addr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  return 0;
}
