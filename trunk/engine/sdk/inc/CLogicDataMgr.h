#pragma once
#include "CVector3.h"
#include "CElementManager.h"

namespace sqr
{
	class CLogicDataMgr : public CElementManager
	{
	public:
		CLogicDataMgr(){};

		CElementNode*		_CreateNode( URegionID ID );
	
		const CVector3f&	GetLogicNormal( uint32 uX, uint32 uY );
		float				GetLogicHight(	uint32 uX, uint32 uY );
		int8				GetMaterialIndex(uint32 uX, uint32 uY);
		float				GetWaterHeight(uint32 uX, uint32 uY);
	};
}