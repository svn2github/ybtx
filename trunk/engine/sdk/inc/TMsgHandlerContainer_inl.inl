#pragma once
#include "TMsgHandlerContainer.h"

template<typename ImpClass>
template<typename MsgClass>
void TMsgHandlerContainer<ImpClass>::SetHandler( void (ImpClass::*HandlerFun) (const MsgClass*,void*) )
{
	typedef TMsgHandler<ImpClass,MsgClass> Handler_t;

	Handler_t* pNewHandler = new Handler_t( HandlerFun );

	BaseHandler_t*& pHandler=m_vecHandler[ MsgClass::GetId() + m_nIdOffset ];
	delete pHandler;
	pHandler=pNewHandler;
}
