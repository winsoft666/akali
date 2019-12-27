#include "akali/scoped_variant.h"

#if (defined _WIN32 || defined WIN32)
#include "akali/assert.h"

namespace akali {
// Global, const instance of an empty variant.
const VARIANT ScopedVariant::kEmptyVariant = {VT_EMPTY};

ScopedVariant::~ScopedVariant() { ::VariantClear(&var_); }

ScopedVariant::ScopedVariant(const wchar_t *str) {
  var_.vt = VT_EMPTY;
  Set(str);
}

ScopedVariant::ScopedVariant(const wchar_t *str, UINT length) {
  var_.vt = VT_BSTR;
  var_.bstrVal = ::SysAllocStringLen(str, length);
}

ScopedVariant::ScopedVariant(int value, VARTYPE vt) {
  var_.vt = vt;
  var_.lVal = value;
}

ScopedVariant::ScopedVariant(double value, VARTYPE vt) {
  AKALI_ASSERT(vt == VT_R8 || vt == VT_DATE);
  var_.vt = vt;
  var_.dblVal = value;
}

ScopedVariant::ScopedVariant(IDispatch *dispatch) {
  var_.vt = VT_EMPTY;
  Set(dispatch);
}

ScopedVariant::ScopedVariant(IUnknown *unknown) {
  var_.vt = VT_EMPTY;
  Set(unknown);
}

ScopedVariant::ScopedVariant(SAFEARRAY *safearray) {
  var_.vt = VT_EMPTY;
  Set(safearray);
}

ScopedVariant::ScopedVariant(const VARIANT &var) {
  var_.vt = VT_EMPTY;
  Set(var);
}

void ScopedVariant::Reset(const VARIANT &var) {
  if (&var != &var_) {
    ::VariantClear(&var_);
    var_ = var;
  }
}

VARIANT ScopedVariant::Release() {
  VARIANT var = var_;
  var_.vt = VT_EMPTY;
  return var;
}

void ScopedVariant::Swap(ScopedVariant &var) {
  VARIANT tmp = var_;
  var_ = var.var_;
  var.var_ = tmp;
}

VARIANT *ScopedVariant::Receive() {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "variant leak.type: %d", var_.vt);
  return &var_;
}

VARIANT ScopedVariant::Copy() const {
  VARIANT ret = {VT_EMPTY};
  ::VariantCopy(&ret, &var_);
  return ret;
}

int ScopedVariant::Compare(const VARIANT &var, bool ignore_case) const {
  ULONG flags = ignore_case ? NORM_IGNORECASE : 0;
  HRESULT hr = ::VarCmp(const_cast<VARIANT *>(&var_), const_cast<VARIANT *>(&var),
                        LOCALE_USER_DEFAULT, flags);
  int ret = 0;

  switch (hr) {
  case VARCMP_LT:
    ret = -1;
    break;

  case VARCMP_GT:
  case VARCMP_NULL:
    ret = 1;
    break;

  default:
    // Equal.
    break;
  }

  return ret;
}

void ScopedVariant::Set(const wchar_t *str) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_BSTR;
  var_.bstrVal = ::SysAllocString(str);
}

void ScopedVariant::Set(int8_t i8) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_I1;
  var_.cVal = i8;
}

void ScopedVariant::Set(uint8_t ui8) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_UI1;
  var_.bVal = ui8;
}

void ScopedVariant::Set(int16_t i16) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_I2;
  var_.iVal = i16;
}

void ScopedVariant::Set(uint16_t ui16) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_UI2;
  var_.uiVal = ui16;
}

void ScopedVariant::Set(int32_t i32) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_I4;
  var_.lVal = i32;
}

void ScopedVariant::Set(uint32_t ui32) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_UI4;
  var_.ulVal = ui32;
}

void ScopedVariant::Set(int64_t i64) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_I8;
  var_.llVal = i64;
}

void ScopedVariant::Set(uint64_t ui64) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_UI8;
  var_.ullVal = ui64;
}

void ScopedVariant::Set(float r32) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_R4;
  var_.fltVal = r32;
}

void ScopedVariant::Set(double r64) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_R8;
  var_.dblVal = r64;
}

void ScopedVariant::SetDate(DATE date) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_DATE;
  var_.date = date;
}

void ScopedVariant::Set(IDispatch *disp) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_DISPATCH;
  var_.pdispVal = disp;
  if (disp)
    disp->AddRef();
}

void ScopedVariant::Set(bool b) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_BOOL;
  var_.boolVal = b ? VARIANT_TRUE : VARIANT_FALSE;
}

void ScopedVariant::Set(IUnknown *unk) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  var_.vt = VT_UNKNOWN;
  var_.punkVal = unk;
  if (unk)
    unk->AddRef();
}

void ScopedVariant::Set(SAFEARRAY *array) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  if (SUCCEEDED(::SafeArrayGetVartype(array, &var_.vt))) {
    var_.vt |= VT_ARRAY;
    var_.parray = array;
  }
  else {
    AKALI_ASSERT(!array, "Unable to determine safearray vartype");
    var_.vt = VT_EMPTY;
  }
}

void ScopedVariant::Set(const VARIANT &var) {
  AKALI_ASSERT(!IsLeakableVarType(var_.vt), "leaking variant: %d", var_.vt);
  if (FAILED(::VariantCopy(&var_, &var))) {
    var_.vt = VT_EMPTY;
  }
}

ScopedVariant &ScopedVariant::operator=(const VARIANT &var) {
  if (&var != &var_) {
    VariantClear(&var_);
    Set(var);
  }
  return *this;
}

bool ScopedVariant::IsLeakableVarType(VARTYPE vt) {
  bool leakable = false;
  switch (vt & VT_TYPEMASK) {
  case VT_BSTR:
  case VT_DISPATCH:
  // we treat VT_VARIANT as leakable to err on the safe side.
  case VT_VARIANT:
  case VT_UNKNOWN:
  case VT_SAFEARRAY:

  // very rarely used stuff (if ever):
  case VT_VOID:
  case VT_PTR:
  case VT_CARRAY:
  case VT_USERDEFINED:
  case VT_LPSTR:
  case VT_LPWSTR:
  case VT_RECORD:
  case VT_INT_PTR:
  case VT_UINT_PTR:
  case VT_FILETIME:
  case VT_BLOB:
  case VT_STREAM:
  case VT_STORAGE:
  case VT_STREAMED_OBJECT:
  case VT_STORED_OBJECT:
  case VT_BLOB_OBJECT:
  case VT_VERSIONED_STREAM:
  case VT_BSTR_BLOB:
    leakable = true;
    break;
  }

  if (!leakable && (vt & VT_ARRAY) != 0) {
    leakable = true;
  }

  return leakable;
}
} // namespace akali
#endif