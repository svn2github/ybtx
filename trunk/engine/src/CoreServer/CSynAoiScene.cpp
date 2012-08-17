#include "stdafx.h"
#include "CSynAoiScene.h"
#include "CRectangle.h"
#include "CAoiShape.h"
#include "CAoiShapeCache.h"

#include "CSynAoiViewObj.h"
#include "CSynAoiSyncerObj.h" 
#include "CSynAoiSynceeObj.h" 

#include "CSynAoiDimScene.h"
#include "CSynAoiDimObj.h"
#include "ErrLogHelper.h"
#include "CDimensionInfo.h"
#include "TSqrAllocator.inl"


CSynAoiScene::CSynAoiScene(const char* szSyncAoiFileName,uint32 uScale, 
						   uint32 uPixelWidth, uint32 uPixelHeight, CDimensionInfo* DimInfo)
						   : m_uPixelWidth(uPixelWidth)
						   , m_uPixelHeight(uPixelHeight)
						   , m_uScale(uScale)
#ifndef _WIN32
						   , m_mapDimScene(10)
#endif
{
	m_pAoiShape=CAoiShapeCache::Inst().GetShape(szSyncAoiFileName);
	
	CDimensionInfo::MapDimensionInfo::const_iterator iter = DimInfo->GetDimensionInfo().begin();
	CDimensionInfo::MapDimensionInfo::const_iterator iter_end = DimInfo->GetDimensionInfo().end();

	while(iter != iter_end)
	{
		uint32 uDimensionID = iter->first;
		uint32 uPrecision = iter->second;

		CSynAoiDimScene* pDimScene = new CSynAoiDimScene(this, 
			m_uPixelWidth, m_uPixelHeight, uPrecision);
		m_mapDimScene.insert(make_pair(uDimensionID, pDimScene));		

		++iter;
	}
}

CSynAoiScene::~CSynAoiScene()
{
	MapDimScene_t::iterator iter = m_mapDimScene.begin();
	while (iter != m_mapDimScene.end())
	{
		iter->second->Release();
		++iter;
	}
}

bool CSynAoiScene::DimensionExist(uint32 uDimensionId)
{
	MapDimScene_t::const_iterator iter = m_mapDimScene.find(uDimensionId);
	if (iter != m_mapDimScene.end())
	{
		return true;
	}

	return false;
}


bool CSynAoiScene::IsInSyncAoi(const CFPos& OffsetGridPos)const
{
	CPos Offpos;
	Offpos.x = (int32)(OffsetGridPos.x / m_uScale);
	Offpos.y = (int32)(OffsetGridPos.y / m_uScale);
	return IsInSyncAoi(Offpos);
}

inline bool CSynAoiScene::IsInSyncAoi(const CPos& GridPos)const
{   
	return m_pAoiShape->In(GridPos);
}


CSynAoiSyncerObj* CSynAoiScene::CreateSyncerObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId)
{
	CSynAoiSyncerObj *pObj = new CSynAoiSyncerObj(this,pos, uId, uCoreObjId);
	pObj->InsertToScene();

	return pObj;
}

CSynAoiSynceeObj* CSynAoiScene::CreateSynceeObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId)
{
	CSynAoiSynceeObj *pObj = new CSynAoiSynceeObj(this,pos, uId, uCoreObjId);
	pObj->InsertToScene();

	return pObj;
}

CSynAoiViewObj* CSynAoiScene::CreateViewObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId)
{
	CSynAoiViewObj *pObj = new CSynAoiViewObj(this,pos, uId, uCoreObjId);
	pObj->InsertToScene();

	return pObj;
}

void CSynAoiScene::DestroyObj(CSynAoiViewObj* pAoiObj)
{
	pAoiObj->RemoveFrScene();
	delete pAoiObj;
}

void CSynAoiScene::Release()
{
	delete this;
}

