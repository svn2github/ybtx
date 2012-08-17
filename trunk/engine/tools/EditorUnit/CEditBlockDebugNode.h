#pragma once
#include "CSceneBlockDebug.h"

namespace sqr_tools
{
	class CEditBlockDebugNode : public CSceneBlockDebugNode
	{
	public:
		CEditBlockDebugNode(ERenderObjStyle style , CRenderScene* pScene);
		~CEditBlockDebugNode();

		EBarrierType	GetBarrierType(CPos pos);
	};

}
