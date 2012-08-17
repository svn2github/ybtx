#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class CCoreObjectFollower;
	class CCoreObjectDirector;
	class ICoreSceneClientHandler  : public virtual CDynamicObject
	{
	public:
		virtual ~ICoreSceneClientHandler() {};
		virtual void OnObjectCreated(CCoreObjectFollower* pObject){};
		virtual void OnDirectorCreated(CCoreObjectDirector* pObject){};
	};
}