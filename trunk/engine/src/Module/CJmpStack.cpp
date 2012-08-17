#include "stdafx.h"
#include "CJmpStack.h"
#include "ThreadHelper.h"
#include "CCallStackInfo.h"
#include <stack>
#include <string>
#include "ExpHelper.h"
#include "TMallocAllocator.h"

CJmpBuf::CJmpBuf()
: m_bReset(false)
{
	CJmpStack::Inst().Push(this);
}

CJmpBuf::~CJmpBuf()
{
	if(!m_bReset)
		CJmpStack::Inst().Pop();
}

JmpBuf_t* CJmpBuf::GetBuf() 
{
	return &m_Buf;
}

void CJmpBuf::Reset()
{
	m_bReset = true;
	CJmpStack::Inst().Pop();
}

namespace sqr
{
	class CJmpStackImp
		: public CMallocObject
	{
	public:
		class JmpStack_t
			: public stack<CJmpBuf*, deque<CJmpBuf*, TMallocAllocator<CJmpBuf*> > >
			, public CMallocObject
		{
		};

		class CJmpStackError
			: public CMallocObject
		{
		public:
			CJmpStackError() : m_Error("","")
			{
				m_szErrType[0] = '\0';
				m_szErrMsg[0] = '\0';
				m_pStackInfo = new CCallStackInfo(false);
			}

			~CJmpStackError()
			{
				delete m_pStackInfo;
			}

			char	m_szErrType[1024];
			char	m_szErrMsg[1024];

			CCallStackInfo*	m_pStackInfo;
		
			CError		m_Error;
		};

		CJmpStackImp()
		{
			TLS_CreateKey(&m_key);
			TLS_CreateKey(&m_ErrorKey);
		}
		
		~CJmpStackImp()
		{
			TLS_DestroyKey(m_ErrorKey);
			TLS_DestroyKey(m_key);
		}

		HTLSKEY	m_key;
		HTLSKEY	m_ErrorKey;

		bool Empty()
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			if(!pStack)
				return true;

			return pStack->empty();
		}

		void Push(CJmpBuf* buf)
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			if(!pStack)
			{
				pStack = new JmpStack_t;
				TLS_SetValue(m_key, pStack);
			}
			pStack->push(buf);
		}

		void Clear()
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			Ast(pStack);
			while( !pStack->empty() )
				pStack->pop();
		}

		CJmpBuf* Top()
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			Ast(pStack);
			return pStack->top();
		}

		void Pop()
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			Ast(pStack);
			pStack->pop();
			if(pStack->empty())
			{
				delete pStack;
				TLS_SetValue(m_key, NULL);
			}
		}

		void* GetCurrentContextStack()
		{
			JmpStack_t* pStack = (JmpStack_t*)TLS_GetValue(m_key);
			return pStack;
		}

		void SetCurrentContextStack(void* pStack)
		{
			TLS_SetValue(m_key, pStack);
		}

		void SetError(CError& error)
		{
			CJmpStackError* pError= (CJmpStackError*)TLS_GetValue(m_ErrorKey);
			if(!pError)
			{
				pError = new CJmpStackError;
				TLS_SetValue(m_ErrorKey, pError);
			}
			pError->m_Error = error;
		}


		void SetError(const char* szErrorType, const char* szErrorMsg)
		{
			CJmpStackError* pError= (CJmpStackError*)TLS_GetValue(m_ErrorKey);
			if(!pError)
			{
				pError = new CJmpStackError;
				TLS_SetValue(m_ErrorKey, pError);
			}
			
			pError->m_pStackInfo->WalkStack();

			strcpy(pError->m_szErrType, szErrorType);
			strcpy(pError->m_szErrMsg, szErrorMsg);
		}

		CError& GetError()
		{
			CJmpStackError* pError= (CJmpStackError*)TLS_GetValue(m_ErrorKey);
			Ast(pError);

			if(pError->m_szErrType[0] !=  '\0')
			{
				
				pError->m_Error = CError(pError->m_szErrType, pError->m_pStackInfo->GetText());
				
				pError->m_Error.AppendMsg(pError->m_szErrMsg);

				pError->m_szErrType[0] = '\0';
				pError->m_szErrMsg[0] = '\0';
			}
					
			return pError->m_Error;
		}
	};
}

CJmpStack::CJmpStack()
{
	m_pData = new CJmpStackImp;
}

CJmpStack::~CJmpStack()
{
	delete m_pData;
}


CJmpStack& CJmpStack::Inst()
{
	static CJmpStack ls_Inst;
	return ls_Inst;
}                                


void* CJmpStack::GetCurrentContextStack()
{
	return m_pData->GetCurrentContextStack();
}

void CJmpStack::SetCurrentContextStack(void* pStack)
{
	return m_pData->SetCurrentContextStack(pStack);
}

void CJmpStack::Push(CJmpBuf* buf)
{
	return m_pData->Push(buf);
}                                                                                                                        

void CJmpStack::Pop()
{
	return m_pData->Pop();
}

void CJmpStack::Clear()
{
	return m_pData->Clear();
}

CJmpBuf* CJmpStack::Top()
{
	return m_pData->Top();
}

CError& CJmpStack::GetError() 
{
	return m_pData->GetError();
}

void CJmpStack::LongJmp(CError& exp)
{
	if(m_pData->Empty())
		abort();
	
	m_pData->SetError(exp);

	JmpBuf_t* buf = m_pData->Top()->GetBuf();
	
#ifdef _WIN32
	longjmp(*buf, 1);                               
#else
	siglongjmp(*buf, 1);                               
#endif
}

void CJmpStack::LongJmp(CError* pExp)
{
	if(m_pData->Empty())
		abort();
	
	m_pData->SetError(*pExp);

	delete pExp;

	JmpBuf_t* buf = m_pData->Top()->GetBuf();
	
#ifdef _WIN32
	longjmp(*buf, 1);                               
#else
	siglongjmp(*buf, 1);                               
#endif
}

void CJmpStack::LongJmp(const char* szErrorType, const char* szErrorMsg)
{
	if(m_pData->Empty())
		abort();
	
	m_pData->SetError(szErrorType, szErrorMsg);
	
	JmpBuf_t* buf = m_pData->Top()->GetBuf();
	
#ifdef _WIN32
	longjmp(*buf, 1);                               
#else
	siglongjmp(*buf, 1);                               
#endif
}               

/************************************************************************/
/*                                                                      */
/************************************************************************/

namespace sqr
{
	MODULE_API void  *GetCurrentContextStack()
	{
		return CJmpStack::Inst().GetCurrentContextStack();
	}

	MODULE_API void  SetCurrentContextStack(void* pStack)
	{
		CJmpStack::Inst().SetCurrentContextStack(pStack);
	}
}
