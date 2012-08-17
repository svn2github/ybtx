#include "stdafx.h"
#include "CLogicData.h"
#include "CMetaRegion.h"
#include "CLogicDataCoder.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

BEGIN_VERSION_MAP(CLogicDataCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()

CLogicDataCoder::CLogicDataCoder(CSceneManagerClient* pScene)
: m_pScene(pScene)
{
	m_eSpri = SPRI_High;
}

bool CLogicDataCoder::CElementDeCode( CElementNode* pNode, bool bSync  )
{
	if(bSync)
	{
		pNode->SetCoder(this);
		CDataChunk tempChunk;
		uint16		ver = 0;
		//m_pScene	= (CSceneManagerClient*)((CClientReadData*)pReadData)->pScene;
		tempChunk.SetVersion(ver);

		SQR_TRY	
		{
			DeCode(tempChunk,pNode);
		}
		SQR_CATCH(exp)
		{
			exp.AppendMsg("CLogicDataCoder::资源文件加载失败");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
	else
		CElementCoderEx::CElementDeCode(pNode,false);
	return true;
}

bool CLogicDataCoder::_CElementDeCode( const DeCodeParams& Params )
{
	CDataChunk tempChunk;
	uint16		ver = 0;
	tempChunk.SetVersion(ver);
	return DeCode(tempChunk,Params.m_pNode);	
}


bool CLogicDataCoder::DeCode_Ver_0( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CLogicData* pLogicData = (CLogicData*)handle;
	//CMetaGridMgr& GridMgr = *m_pScene->m_pGridMgr;
	uint32 regionId;
	uint32 vecId;

	pLogicData->m_aryLogicHight.resize(eSRegionSpan*eSRegionSpan);
	pLogicData->m_aryWaterHight.resize(eSRegionSpan*eSRegionSpan,0.0f);
	pLogicData->m_aryLogicNormal.resize(eSRegionSpan*eSRegionSpan);
	pLogicData->m_MaterialIndex.resize(eSRegionSpan*eSRegionSpan);
	
	int32 w = pLogicData->GetID().X;// % m_pScene->GetWidth();
	int32 h = pLogicData->GetID().Z;// / m_pScene->GetWidth();

	w *= 4;
	h *= 4;
	int wn = 0;
	for( int x = 0; x<4 ; ++x )
	{
		for( int y = 0; y<4; ++y)
		{
			uint32 deltaWidth = w + x;
			uint32 deltaHeight = y + h;
			regionId = deltaWidth + deltaHeight*m_pScene->GetWidthInRegion();

			CMetaRegion* region = m_pScene->GetRegion(regionId);
			RegionWater* waterRegion = region->GetWaterInfo();
			vecId = x * eRegionSpan + y * eRegionSpan * eSRegionSpan;
	
			if(waterRegion)
				++wn;
			for(int ty = 0; ty< eRegionSpan; ++ty)
			{
				for(int tx =0; tx<eRegionSpan; ++tx)
				{
					if(waterRegion)
						pLogicData->m_aryWaterHight[vecId]	= waterRegion->m_nWaterHeight[ty*eRegionSpan + tx];
					pLogicData->m_aryLogicHight[vecId]	= float(region->GetOffsetHeight(tx,ty) + region->GetTerranHeight(tx,ty));
					pLogicData->m_aryLogicNormal[vecId] = region->GetTerranLogicNomal(tx,ty);
					pLogicData->m_MaterialIndex[vecId] = region->GetTerrainMaterialIndex(tx,ty);
					++vecId;
				}
				vecId += eSRegionSpan - eRegionSpan;
			}
		}
	}
	if(0==wn)
		pLogicData->m_aryWaterHight.clear();
	return true;
}