#pragma once
#include "CElementManager.h"

namespace sqr_tools
{
	class CMapWaterCoder : public CElementCoder
	{
	public:
		bool AutoDeCode(void){return true;}

		bool CElementDeCode( CElementNode* pNode, CDataChunk* pDataChunk, CRegionReadData* pReadData, bool bSync );
	};
}