#include "akali.h"
#include "akali/akali_export.h"

#ifdef AKALI_WIN
#ifndef AKALI_STATIC
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID /*lpReserved*/) {
  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    ::DisableThreadLibraryCalls((HMODULE)hModule);
    break;
  }

  return TRUE;
}
#endif
#endif