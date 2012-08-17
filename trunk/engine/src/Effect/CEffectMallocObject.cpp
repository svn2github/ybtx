#include "stdafx.h"
#include "CEffectMallocObject.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

DefineAllocObject(CEffectMallocObject);
template class basic_string<char, char_traits<char>, CEffectMallocAllocator<char> >;
template class basic_string<wchar_t, char_traits<wchar_t>,	CEffectMallocAllocator<wchar_t> >;
