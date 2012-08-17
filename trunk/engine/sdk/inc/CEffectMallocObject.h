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
#define USE_EFFECT_POOL

#ifdef USE_EFFECT_POOL
	DeclareAllocObject(CEffectMallocObject);

	DeclarePoolAllocator(CEffectMallocAllocator, CEffectMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class EMap :public map<  _Kty, _Ty, _Pr, CEffectMallocAllocator<pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class EVector :public vector<  _Ty , CEffectMallocAllocator<_Ty> >
	{
	public:
		EVector(): vector(){}
		EVector(size_type _Count)
			: vector(_Count){}
		EVector(size_type _Count, const _Ty& _Val)
			: vector(_Count,_Val){}

		EVector(const _Myt& _Right)
			: vector(_Right){}

		template<class _Iter>
		EVector(_Iter _First, _Iter _Last)
			: vector(_First,_Last){}
	};

	template<class _Ty>
	class EList :public list<  _Ty, CEffectMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class ESet :public set<  _Kty, _Pr, CEffectMallocAllocator<_Kty> >
	{};

	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class EHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, CEffectMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};

	typedef basic_string<char, char_traits<char>, CEffectMallocAllocator<char> >			EString;
	typedef basic_string<wchar_t, char_traits<wchar_t>,	CEffectMallocAllocator<wchar_t> >	EWString;
#else
	class CEeffectMallocObject 
	{};
	typedef map			EMap;
	typedef vector		EVector;
	typedef list		EList;
	typedef set			ESet;
	typedef string		EString;
	typedef	wstring		EWString;
	typedef stdext::hash_map	EHashMap;
#endif
}

