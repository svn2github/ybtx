#pragma once

#include "Base.h"
#include "TMsgAllocator.h"

namespace sqr
{
	template<typename ImpClass>
	class TMsgHandlerContainer
		:public CMsgMallocObject
	{
	public:
		typedef TBaseMsgHandler<ImpClass>	BaseHandler_t;

		TMsgHandlerContainer();

		~TMsgHandlerContainer();

		uint32 GetSize()const;

		int32 GetIdOffset()const;

		bool Resize(uint32 uSize,int32 nIdOffset);

		BaseHandler_t* GetHandler( uint32 uIndex )const;

		template<typename MsgClass>
		void SetHandler( void (ImpClass::*HandlerFun) (const MsgClass*,void*) );


	private:
		int32					m_nIdOffset;
		vector<BaseHandler_t*, TMsgAllocator<BaseHandler_t*> >	m_vecHandler;
	};
}

