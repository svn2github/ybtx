#include "stdafx.h"
#include "CLogicData.h"
#include "CSceneBase.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CLogicData::CLogicData(CElementManager* pParent , URegionID id)
: CElementNode(pParent,id) 
{

}

const CVector3f&	CLogicData::GetLogicNormal(uint16 x,uint16 y)
{
	if(m_aryLogicNormal.size()==0)
	{
		if(m_pCoder!=NULL)
		{
			m_pCoder->SyncCode(this);
		}

		if( m_aryLogicNormal.size()==0 )
			return CVector3f::ZERO;
	}

	return m_aryLogicNormal[y*eSRegionSpan + x];
}

float	CLogicData::GetLogicHight( uint16 x, uint16 y )
{
	if(m_aryLogicHight.size()==0)
	{
		if(m_pCoder!=NULL)
		{
			m_pCoder->SyncCode(this);
		}

		if(m_aryLogicHight.size()==0)
			return 0.0f;
	}

	return m_aryLogicHight[y*eSRegionSpan + x];
}

float CLogicData::GetWaterHeight( uint16 x, uint16 y )
{	
	if(m_aryWaterHight.size()==0)
	{
		if(m_pCoder!=NULL)
		{
			m_pCoder->SyncCode(this);
		}
		if(m_aryWaterHight.size()==0)
			return 0.0f;
	}

	return m_aryWaterHight[y*eSRegionSpan + x];
}

int8 CLogicData::GetMaterialIndex(uint16 x, uint16 y)
{
	if(m_MaterialIndex.size()==0)
	{
		if(m_pCoder!=NULL)
		{
			m_pCoder->SyncCode(this);
		}
		if(m_MaterialIndex.size()==0)
			return 0;
	}

	return m_MaterialIndex[y*eSRegionSpan + x];
}
