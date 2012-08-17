#pragma once
#include "CCoreServerJob.inl"
#include "CCoreServerObjJob.h"
#include "ThreadTypes.h"
#include "TVariantJobAllocator.h"
#include "CVariantSyncStream.h"
#include "CVariantHolderDef.h"

namespace sqr
{
	template <class T>
	class TVariantJobAllocator;

	typedef basic_stringstream<char,std::char_traits<char>,TVariantJobAllocator<char> > CVariantJobStream;
	typedef basic_string<char,std::char_traits<char>,TVariantJobAllocator<char> > CVariantJobString;

	class CCoreObjVariantSyncStreamJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjVariantSyncStreamJob(uint32 uObjID, bool bForSync, ECodeTableIndex eCodeTableIndex);
		~CCoreObjVariantSyncStreamJob();
		//void Test();
		CVariantSyncStream	m_Stream;

		static	HTHREADID	ms_CreateJobThreadID;
	private:
		bool	m_bForSync;
		ECodeTableIndex m_eCodeTableIndex;
	};

}
