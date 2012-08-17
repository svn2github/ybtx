#pragma once


namespace sqr
{
	class CCoreMsgMgr;

	class CCoreMsg
	{
		friend class CCoreMsgMgr;
	public:
		void *operator new( size_t stSize, CCoreMsgMgr& MsgMgr)throw();
		inline void operator delete( void*, CCoreMsgMgr& ){};		
		inline void operator delete(void*){}
	protected:
		CCoreMsg(void);
		virtual ~CCoreMsg(void);
	};
}

