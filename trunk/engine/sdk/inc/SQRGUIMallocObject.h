#pragma once
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include <fstream>
#include <string>
#include <vector>
#include <hash_map>
#include <list>
#include <queue>
#include <map>
#include <set>

namespace sqr
{
#define USE_GUI_POOL

#ifdef USE_GUI_POOL
	DeclareAllocObject(SQRGuiMallocObject);

	DeclarePoolAllocator(SQRGuiMallocAllocator, SQRGuiMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class UIMap :public map<  _Kty, _Ty, _Pr, SQRGuiMallocAllocator<pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class UIVector :public vector<  _Ty , SQRGuiMallocAllocator<_Ty> >
	{
	public:
		UIVector(): vector(){}
		UIVector(size_type _Count)
			: vector(_Count){}
		UIVector(size_type _Count, const _Ty& _Val)
			: vector(_Count,_Val){}
	};

	template<class _Ty>
	class UIList :public list<  _Ty, SQRGuiMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class UISet :public set<  _Kty, _Pr, SQRGuiMallocAllocator<_Kty> >
	{};

	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class UIHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, SQRGuiMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};

	typedef basic_string<char, char_traits<char>, SQRGuiMallocAllocator<char> >			UIString;
	typedef basic_string<wchar_t, char_traits<wchar_t>,	SQRGuiMallocAllocator<wchar_t> >	UIWString;
#else
	class SQRGuiMallocObject 
	{};
	typedef map			UIMap;
	typedef vector		UIVector;
	typedef list		UIList;
	typedef set			UISet;
	typedef string		UIString;
	typedef wstring		UIWString;
	typedef stdext::hash_map	GHashMap;
#endif
}

