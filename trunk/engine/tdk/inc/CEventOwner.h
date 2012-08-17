#pragma once
#include <queue>
#include <set>
#include "CMsg.h"
#include "CMsgDefine.h"
#include "CEventDelegate.h"

namespace sqr
{
	class CEventOwner
	{
	public:
		CEventOwner();
		virtual ~CEventOwner();
	public:
		
		bool IsEmpty();
		void DoNext();

		void EventCollect( const CMsg& msg );
		bool Run(void);
		bool Pause(void);

		void RegisterDelegate( CEventDelegate* pDele );
		void UnRegisterDelegate( CEventDelegate* pDele );
		void ActiveDelegate( CEventDelegate* pDele );
	protected:
		virtual bool _ProcMsg( const CMsg& MSG );
				bool _IsRuning(void);
		virtual bool _PostMsg( const CMsg& MSG ); //Á¢¿ÌÖ´ÐÐ
	private:
		typedef std::queue<CMsg>			MsgQueue;
		typedef std::list<CEventDelegate*>	DelegatePool;
		MsgQueue			m_qMsg;
		DelegatePool		m_DelegatePool;
		CEventDelegate*		m_ActiveDelegate;
		bool				m_bIsRunning;
	};

	inline bool CEventOwner::_IsRuning(void) { return m_bIsRunning; }
}