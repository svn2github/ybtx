#include "stdafx.h"
#include "CGraphicMallocObject.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

DefineAllocObject(CGraphicMallocObject);

template class basic_string<char, char_traits<char>, CGraphicMallocAllocator<char> >;
template class basic_string<wchar_t, char_traits<wchar_t>,	CGraphicMallocAllocator<wchar_t> >;