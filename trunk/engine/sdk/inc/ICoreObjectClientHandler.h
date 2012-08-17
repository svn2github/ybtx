#pragma once
#include "CoreObjectDefs.h"
#include "CDynamicObject.h"

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<float>	CFPos;
	class ICoreObjectFollowerHandler;
	class ICoreObjectPerformerHandler;

	class ICoreObjectClientHandler
		: public virtual CDynamicObject
	{
	public:
		virtual void OnRefreshRenderObjPos(const CFPos& PixelPos) {}
		virtual void OnPosChanged() {}
		virtual void OnMoveBegan() {}
		virtual void OnMovePathChanged() {}
		virtual void OnMoveEnded(EMoveEndedType eMEType) {}
		virtual void OnMoveStepped() {}
		virtual void OnWayPointChanged() {}
		virtual void OnDestroy() {}

		virtual ICoreObjectFollowerHandler* CastToObjFolHandler()const
		{
			return NULL;
		}
		virtual ICoreObjectPerformerHandler* CastToObjPerHandler()const
		{
			return NULL;
		}
		virtual void* GetTag() const
		{
			return NULL;
		}
	};
}