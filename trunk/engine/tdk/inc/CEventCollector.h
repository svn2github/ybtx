#pragma once
#include "CEventOwner.h"

namespace sqr
{
	class CEventState;
	class CEventCollector
	{
	public:
		CEventCollector( CEventOwner* pOwner=NULL );
		virtual ~CEventCollector();

		virtual void	Collect( const CMsg& msg );
		virtual	void	Collect( const MSG_SOURCE& fSource,const MSG_TYPE& fType,const MSG_INFO& fInfo );
		virtual void	SetOwner( CEventOwner* pOwner );
	protected:

		virtual bool _ProcMsg(const CMsg& msg );
				void _collect( const CMsg& msg );
	protected:
		CEventOwner*	m_pOwner;
	};
}