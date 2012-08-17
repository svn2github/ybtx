#include "stdafx.h"
#include "CSceneMallocObject.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"

DefineAllocObject(CSceneMallocObject);

template class basic_string<char, char_traits<char>, CSceneMallocAllocator<char> >;
template class basic_string<wchar_t, char_traits<wchar_t>,	CSceneMallocAllocator<wchar_t> >;
