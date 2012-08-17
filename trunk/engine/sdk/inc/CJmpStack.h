#pragma once 
#include "ModuleDefs.h"
#include "CError.h"
#include <setjmp.h>
#include <memory>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

namespace sqr
{
	//windows下面用jmp_buf，linux下面用sigjmp_buf
#ifdef  _WIN32
	typedef jmp_buf		JmpBuf_t;
#else	
	typedef sigjmp_buf	JmpBuf_t;
#endif

	class MODULE_API CJmpBuf 
	{
	public:
		CJmpBuf();
		~CJmpBuf();
		void Reset();

		JmpBuf_t* GetBuf();
	private:
		JmpBuf_t	m_Buf;
		bool		m_bReset;
	};

	class CJmpStackImp;

	class MODULE_API CJmpStack
	{
	public:
		static CJmpStack& Inst();

		void Push(CJmpBuf* buf);
		void Pop();
		void Clear();

		CJmpBuf* Top();

		void LongJmp(const char* szErrorType, const char* szErrorMsg = "");
		void LongJmp(CError& exp);
		void LongJmp(CError* pExp);

		CError& GetError();

		void* GetCurrentContextStack();
		void SetCurrentContextStack(void* pStack);

	private:
		CJmpStack();
		~CJmpStack();

	private:
		CJmpStackImp*		m_pData;
	};
}

#ifdef _WIN32
#pragma warning (pop)
#endif
