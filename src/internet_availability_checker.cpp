#include "akali/internet_availability_checker.h"

#ifdef AKALI_WIN
#include <netlistmgr.h>
#include <atlbase.h>
#include <atlcom.h>
#include <assert.h>

namespace akali {
bool InternetAvailabilityChecker::InternetConnectionAvailable(bool& avaliable) {
  CComPtr<INetworkListManager> manager;
  HRESULT hr = manager.CoCreateInstance(CLSID_NetworkListManager);
  assert(("initialize COM first, suggest akali::ScopedCOMInitializer", SUCCEEDED(hr)));
  if (FAILED(hr)) {
    return false;
  }

  VARIANT_BOOL is_connected;
  hr = manager->get_IsConnectedToInternet(&is_connected);
  assert(("initialize COM first, suggest akali::ScopedCOMInitializer", SUCCEEDED(hr)));
  if (FAILED(hr)) {
    return false;
  }

  // Normally VARIANT_TRUE/VARIANT_FALSE are used with the type VARIANT_BOOL
  // but in this case the docs explicitly say to use FALSE.
  // https://docs.microsoft.com/en-us/windows/desktop/api/Netlistmgr/nf-netlistmgr-inetworklistmanager-get_isconnectedtointernet

  avaliable = (is_connected != FALSE);
  return true;
}
}  // namespace akali
#endif