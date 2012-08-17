#pragma once
#include "ICoreObjectDictatorHandler.h"
NAMESPACE_BEGIN(sqr);
class ICoreObjHandler : public ICoreObjectDictatorHandler
{
public:
	ICoreObjHandler(){}
	virtual ~ICoreObjHandler(){}

	virtual void OnCaughtSyncSightOf(uint32 uObjGlobalId){}
	virtual void OnLostSyncSightOf(uint32 uObjGlobalId){}

	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId){}

	virtual ICoreObjHandler* CastToObjHandler()const
	{return const_cast<ICoreObjHandler*>(this);}
};
NAMESPACE_END;