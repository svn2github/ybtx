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
#define USE_MATH_POOL

#ifdef USE_MATH_POOL
	DeclareAllocObject(CMathMallocObject);

	DeclarePoolAllocator(TMathMallocAllocator, CMathMallocObject);

	template<class _Kty,
	class _Ty,
	class _Pr = less<_Kty> >
	class MathMap : public std::map<  _Kty, _Ty, _Pr, TMathMallocAllocator<std::pair<const _Kty, _Ty> > >
	{};

	template<class _Ty>
	class MathVector : public std::vector<  _Ty , TMathMallocAllocator<_Ty> >
	{
	public:
		typedef std::vector<  _Ty , TMathMallocAllocator<_Ty> > Parent_t;
		MathVector()
			: Parent_t(){}
		MathVector(size_t _Count)
			: Parent_t(_Count){}
		MathVector(size_t _Count, const _Ty& _Val)
			: Parent_t(_Count,_Val){}
	};

	template<class _Ty>
	class MathList :public std::list<  _Ty, TMathMallocAllocator<_Ty> >
	{};

	template<class _Kty,
	class _Pr = less<_Kty> >
	class MathSet :public std::set<  _Kty, _Pr, TMathMallocAllocator<_Kty> >
	{};

#ifdef _WIN32
	template<class _Kty,
	class _Ty,
	class _Tr = stdext::hash_compare<_Kty, std::less<_Kty> > >
	class MathHashMap : public stdext::hash_map<  _Kty, _Ty, _Tr, TMathMallocAllocator< std::pair<const _Kty, _Ty> > >
	{};
#else
	template<class _Key, class _Tp, class _HashFcn = hash<_Key>, class _EqualKey = equal_to<_Key> >
		class MathHashMap : public __gnu_cxx::hash_map<_Key, _Tp, _HashFcn, _EqualKey, TMathMallocAllocator< std::pair<const _Key, _Tp> > >
	{};
#endif

	typedef std::basic_string<char, std::char_traits<char>, TMathMallocAllocator<char> >			MathString;
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, TMathMallocAllocator<wchar_t> >	MathWString;
#else
	class CMathMallocObject 
	{};
	typedef map			MathMap;
	typedef vector		MathVector;
	typedef list		MathList;
	typedef set			MathSet;
	typedef string		MathString;
	typedef wstring		MathWString;
	typedef stdext::hash_map	MathHashMap;
#endif
}
