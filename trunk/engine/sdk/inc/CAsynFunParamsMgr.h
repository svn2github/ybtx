#pragma once
#include "CSceneMallocObject.h"

namespace sqr
{
	//
	class FunCommand : public CSceneMallocObject
	{
		friend class CAsynFunParamsMgr;
	public:
		virtual ~FunCommand(){};
		virtual DWORD	GetType(void) { return 0; }	
		virtual bool	Compare(const void* param) { return true; }
	protected:
		virtual bool Do(void) = 0;
	};

	class CAsynFunParamsMgr : public CSceneMallocObject
	{
	public:
		CAsynFunParamsMgr() : m_bLock(false),m_ActiveCmd(NULL) {};
		~CAsynFunParamsMgr() 
		{ Clear(); };
		void	Insert( FunCommand* pFunCom );
		bool	Delete( DWORD type ,const void* param );
		void	Do(DWORD type = 0);
		size_t	Size(void);
		void	Clear(void);
		bool	IsActive(void);

	protected:
		typedef SList<FunCommand*> FunPool;
		FunPool	m_lFunPool;
		bool	m_bLock;
		FunCommand* m_ActiveCmd;
	};
}