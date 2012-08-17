#pragma once
//$id$

#include "ICoreObjectClientHandler.h"

namespace sqr
{
	class ICoreObjectDirectorHandler;

	class ICoreObjectFollowerHandler
		:public ICoreObjectClientHandler
	{
	public:
		virtual void OnPositionSet() {}

		//当Follower或者出于被动态的Director才可能触发这个回调
		virtual void OnSpeedChanged() {}

		virtual void OnEnteredSyncAoi(){}
		virtual void OnLeftedSyncAoi(){}
		virtual ICoreObjectFollowerHandler* CastToObjFolHandler()const
		{
			return const_cast<ICoreObjectFollowerHandler*>(this);
		}
		virtual ICoreObjectDirectorHandler* CastToObjDirHandler()const
		{
			return NULL;
		}
	};
}
