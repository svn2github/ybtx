#include "stdafx.h"
#include "CAudioMallocObject.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

DefineAllocObject(CAudioMallocObject);

template class basic_string<char, char_traits<char>, CAudioMallocAllocator<char> >;
template class basic_string<wchar_t, char_traits<wchar_t>,	CAudioMallocAllocator<wchar_t> >;