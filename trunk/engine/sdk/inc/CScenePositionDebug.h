#pragma once
#include "CSceneDebugHelper.h"

namespace sqr
{

	class CScenePositionDebugNode:public CSceneDebugNode
	{
	public:
		CScenePositionDebugNode(){}
		~CScenePositionDebugNode(){}
		void RenderDebug();
	};
}