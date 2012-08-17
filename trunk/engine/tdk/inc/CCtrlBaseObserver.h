#pragma once

#include "CCtrlBase.h"
#include "CEditObserver.h"

namespace sqr
{
	class CRenderNode;
}

namespace sqr_tools
{
	class CEditObserver;

	class CCtrlBaseObserver 
		: public CCtrlBase
		, public CEditObserver
		, protected CEventDelegate 
	{
	public:
		CCtrlBaseObserver();
		~CCtrlBaseObserver();
		
	private: //Delegate
				void	Active(void);
				bool	MsgProc(const CMsg& msg);
				void	_Init(CEventOwner* pOwner);
	
	public:
		virtual DWORD	Filter(void);

	public: //CCtrlBase
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
	protected:
	};
}