#include "stdafx.h"
#include "SQRGuiMallocObject.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

DefineAllocObject(SQRGuiMallocObject);
template class basic_string<char, char_traits<char>, SQRGuiMallocAllocator<char> >;
template class basic_string<wchar_t, char_traits<wchar_t>,	SQRGuiMallocAllocator<wchar_t> >;
