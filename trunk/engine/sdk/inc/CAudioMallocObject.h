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
#define USE_AUDIO_POOL

#ifdef USE_AUDIO_POOL
	DeclareAllocObject(CAudioMallocObject);

	DeclarePoolAllocator(CAudioMallocAllocator, CAudioMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class AMap :public map<  _Kty, _Ty, _Pr, CAudioMallocAllocator<pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class AVector :public vector<  _Ty , CAudioMallocAllocator<_Ty> >
	{
	public:
		AVector(): vector(){}
		AVector(size_type _Count)
			: vector(_Count){}
		AVector(size_type _Count, const _Ty& _Val)
			: vector(_Count,_Val){}
	};

	template<class _Ty>
	class AList :public list<  _Ty, CAudioMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class ASet :public set<  _Kty, _Pr, CAudioMallocAllocator<_Kty> >
	{};

	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class AHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, CAudioMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};

	typedef basic_string<char, char_traits<char>, CAudioMallocAllocator<char> >			AString;
	typedef basic_string<wchar_t, char_traits<wchar_t>,	CAudioMallocAllocator<wchar_t> >	AWString;
#else
	class CAudioMallocObject 
	{};
	typedef map			AMap;
	typedef vector		AVector;
	typedef list		AList;
	typedef set			ASet;
	typedef string		AString;
	typedef wstring		AWString;
	typedef stdext::hash_map	AHashMap;
#endif
}

