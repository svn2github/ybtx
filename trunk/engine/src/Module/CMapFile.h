#pragma once

#include <vector>
#include <map>
#include "CMallocObject.h"
#include "TMallocAllocator.h"
#include "TMallocPoolAllocator.h"

namespace sqr
{
	class CAddrFunContainer;
	class CMallocPool;

	class CMapFile : public CMallocObject
	{
	public:
		CMapFile();
		~CMapFile();

		void HandleMapFile(const wchar_t* szFileName);
	
		const char* FindFunName(uint32 uFunAddr);

	private:
		typedef basic_string<wchar_t,std::char_traits<wchar_t>,
			TMallocAllocator<wchar_t> > String_t;
		
		set<String_t, less<String_t>, 
			TMallocAllocator<String_t> > m_setMapFile;
	
		typedef pair<uint32, char*>		PairAddr2Symbol_t;
		typedef vector<PairAddr2Symbol_t, TMallocAllocator<PairAddr2Symbol_t> >	VecAddr2Symbol_t;
		typedef vector<VecAddr2Symbol_t, TMallocAllocator<VecAddr2Symbol_t> > VecVecAddr2Symbol_t;

		static bool LessAddr(const PairAddr2Symbol_t& left, const PairAddr2Symbol_t& right);

		VecVecAddr2Symbol_t	m_AddrSymbol;

		CMallocPool*	m_pSymbolPool;
	};

}
