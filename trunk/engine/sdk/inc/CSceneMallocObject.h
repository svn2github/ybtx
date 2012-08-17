#pragma once
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>

#ifdef _WIN32
#	include <hash_map>
#	include <hash_set>
#else
#	include <ext/hash_map>
#	include <ext/hash_set>
using namespace __gnu_cxx;
#endif


namespace sqr
{
#define USE_SCENE_POOL

#ifdef USE_SCENE_POOL
	DeclareAllocObject(CSceneMallocObject);

	DeclarePoolAllocator(CSceneMallocAllocator, CSceneMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class SMap :public map<  _Kty, _Ty, _Pr, CSceneMallocAllocator<pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class SVector :public vector<  _Ty , CSceneMallocAllocator<_Ty> >
	{
	public:
		typedef std::vector<  _Ty , CSceneMallocAllocator<_Ty> > Parent_t;
		SVector()
			: Parent_t(){}
		SVector(size_t _Count)
			: Parent_t(_Count){}
		SVector(size_t _Count, const _Ty& _Val)
			: Parent_t(_Count,_Val){}
	};

	template<class _Ty>
	class SList :public list<  _Ty, CSceneMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class SSet :public set<  _Kty, _Pr, CSceneMallocAllocator<_Kty> >
	{};

#ifdef _WIN32
	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class SHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, CSceneMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};
#else
	template<class _Key, class _Tp, class _HashFcn = hash<_Key>, class _EqualKey = equal_to<_Key> >
		class SHashMap : public __gnu_cxx::hash_map<_Key, _Tp, _HashFcn, _EqualKey, CSceneMallocAllocator< std::pair<const _Key, _Tp> > >
	{};
#endif

	typedef basic_string<char, std::char_traits<char>, CSceneMallocAllocator<char> >			SString;
	typedef basic_string<wchar_t, std::char_traits<wchar_t>,  CSceneMallocAllocator<wchar_t> >	SWString;
#else
	class CGrphicMallocObject 
	{};
	typedef map			SMap;
	typedef vector		SVector;
	typedef SList		SList;
	typedef set			SSet;
	typedef SString		SString;
	typedef SWString		SWString;
	typedef stdext::hash_map	SHashMap;
#endif
}

