#pragma once

#include "ModuleDefs.h"
#include "CMallocObject.h"

namespace sqr
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4275)	//dll linkage
#pragma warning(disable:4251)	//dll linkage
#endif

	//现在这个类只用于GenErr的时候保存堆栈，不跟内存管理库有任何关系了
	class MODULE_API CCallStackInfo
		:public CMallocObject
	{
	public:
		const char* GetText() const;
		
		void WalkStack();

		//stBase表明从那一层堆栈开始记录，一般是从第0层开始，stCount表明记录的层数
		CCallStackInfo(bool bWalk = true, size_t stBase = 0, size_t stCount = 30);
		CCallStackInfo(void* buffer, size_t stBase = 0, size_t stCount = 30);
#ifdef _WIN32
		CCallStackInfo(PCONTEXT pContext, size_t stBase = 0, size_t stCount = 30);
#endif
		~CCallStackInfo(void);
	private:
		void GetStack( size_t uBegin, size_t uEnd );
#ifdef _WIN32
		void GetStack( PCONTEXT pContext, uint32 uBegin, uint32 uEnd );
		void FetchSymbol(void* pAddr);
#endif

		void CopySymbol(const char* szSymbol);

		size_t m_stNextSymbolPos;

		size_t m_stBegin;
		size_t m_stEnd;
		
		char*		m_ayStack;
		bool		m_bNewStack;
	};

#ifdef _WIN32
#pragma warning(pop)
#endif
}
