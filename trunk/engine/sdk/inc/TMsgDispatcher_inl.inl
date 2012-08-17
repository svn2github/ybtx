#pragma once
#include "TMsgDispatcher.h"
#include "TMsgHandler.h"
#include "TMsgHandlerContainer_inl.inl"


template<typename ImpClass,typename IdType,uint32 uId>
template<typename MsgClass>
void TBaseMsgDispatcher<ImpClass,IdType,uId>::SetHandler(  void (ImpClass::*HandlerFun) (const MsgClass*,void*) )
{
	GetHandlers().SetHandler(HandlerFun);
}
