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
#define USE_GRAPHIC_POOL

#ifdef USE_GRAPHIC_POOL
	DeclareAllocObject(CGraphicMallocObject);

	DeclarePoolAllocator(CGraphicMallocAllocator, CGraphicMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class GMap :public map<  _Kty, _Ty, _Pr, CGraphicMallocAllocator<pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class GVector :public vector<  _Ty , CGraphicMallocAllocator<_Ty> >
	{
	public:
		GVector(): vector(){}
		GVector(size_type _Count)
			: vector(_Count){}
		GVector(size_type _Count, const _Ty& _Val)
			: vector(_Count,_Val){}
	};

	template<class _Ty>
	class GList :public list<  _Ty, CGraphicMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class GSet :public set<  _Kty, _Pr, CGraphicMallocAllocator<_Kty> >
	{};

	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class GHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, CGraphicMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};

	typedef basic_string<char, char_traits<char>, CGraphicMallocAllocator<char> >			GString;
	typedef basic_string<wchar_t, char_traits<wchar_t>,	CGraphicMallocAllocator<wchar_t> >	GWString;
#else
	class CGrphicMallocObject 
	{};
	typedef map			GMap;
	typedef vector		GVector;
	typedef list		GList;
	typedef set			GSet;
	typedef string		GString;
	typedef wstring		GWString;
	typedef stdext::hash_map	GHashMap;
#endif
}

