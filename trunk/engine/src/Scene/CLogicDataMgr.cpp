#include "stdafx.h"
#include "CLogicData.h"
#include "CLogicDataMgr.h"
#include "CLogicData.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

const CVector3f& CLogicDataMgr::GetLogicNormal(uint32 uX, uint32 uY)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CLogicData* pData = (CLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		return pData->GetLogicNormal(x,y);
	}
	else
		return CVector3f::ZERO;
}

CElementNode*	CLogicDataMgr::_CreateNode( URegionID ID )
{
	return new CLogicData(this,ID);
}


float CLogicDataMgr::GetLogicHight(	uint32 uX, uint32 uY )
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CLogicData* pData = (CLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		return pData->GetLogicHight(x,y);
	}
	else
		return 0.0f;
}

int8 CLogicDataMgr::GetMaterialIndex(uint32 uX, uint32 uY)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CLogicData* pData = (CLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		return pData->GetMaterialIndex(x,y);
	}
	else
		return 0;
}

float CLogicDataMgr::GetWaterHeight(uint32 uX, uint32 uY)
{
	int16 GridX = uX/eSRegionSpan;
	int16 GridY = uY/eSRegionSpan;
	CLogicData* pData = (CLogicData*)GetNode(URegionID(GridX,GridY));
	if(pData)
	{
		uint16 x = uX - GridX * eSRegionSpan;
		uint16 y = uY - GridY * eSRegionSpan;
		return pData->GetWaterHeight(x,y);
	}
	else
		return 0.0f;
}