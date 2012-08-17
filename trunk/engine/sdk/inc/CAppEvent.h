#pragma once

namespace sqr
{
	class CAppEvent
	{
		friend class CCoreMsgAppEvent;
	public:
		void *operator new( size_t stSize)throw();
		inline void operator delete( void* ){};	
	protected:
		CAppEvent();
		virtual ~CAppEvent();
	};
}
