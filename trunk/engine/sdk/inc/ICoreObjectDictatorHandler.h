#pragma once
//$id$

#include "ICoreObjectServerHandler.h"


namespace sqr
{
	class ICoreObjectMediatorHandler;

	class ICoreObjectDictatorHandler
		:public ICoreObjectServerHandler
	{
	public:
		//严重注意，SyncAoi系列的回掉和ViewAoi系列的回掉各自的回掉顺序是严格保证的，但是他们之间的时序是不保证的

		virtual void OnCaughtSyncSightOf(uint32 uObjGlobalId, uint32 uDimension) {}
		virtual ICoreObjectDictatorHandler* CastToObjDicHandler()const
		{
			return const_cast<ICoreObjectDictatorHandler*>(this);
		}

		virtual ICoreObjectMediatorHandler* CastToObjMedHandler()const
		{
			return NULL;
		}
	};
}
