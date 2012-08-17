#include "stdafx.h"
#include "CAsyAoiScene.h"
#include "CRectangle.h"
#include "CPos.h"

#include "ThreadHelper.h"
#include "CAoiThreadMgr.h"

#include "CAsyAoiViewObj.h"
#include "CAsyAoiSyncerObj.h"
#include "CAsyAoiSynceeObj.h"

#include "CAoiViewObjHandler.h"
#include "CAoiSyncerObjHandler.h"
#include "CAoiSynceeObjHandler.h"

#include "CAsyAoiDimScene.h"
#include "CAsyAoiDimObj.h"
#include "ErrLogHelper.h"

#include "CSynAoiScene.h"
#include "ExpHelper.h"

#include "CAoiSceneHandler.h"
#include "TSqrAllocator.inl"
#include "CAoiJob.inl"
#include "CDimensionInfo.h"

CAsyAoiScene::MapSynAoiScene_t CAsyAoiScene::ms_mapIdToSynScene;
CAsyAoiScene::MapAsyAoiScene_t CAsyAoiScene::ms_mapIdToAsyScene;

CAsyAoiScene::CAsyAoiScene(const char* szSyncAoiFileName,uint32 uScale,  
					 uint32 uPixelWidth, uint32 uPixelHeight, CAoiSceneHandler* pHandler, CDimensionInfo* DimInfo)
					 : m_uId(GetNextId())
					 , m_pHandler(pHandler)
					 , m_eState(eAASS_Running)
#ifndef _WIN32
					 , m_mapAoiDimScene(10)
#endif
{
	ms_mapIdToAsyScene.insert(make_pair<uint32, CAsyAoiScene*>(m_uId, this));


	CDimensionInfo::MapDimensionInfo::const_iterator iter = DimInfo->GetDimensionInfo().begin();
	CDimensionInfo::MapDimensionInfo::const_iterator iter_end = DimInfo->GetDimensionInfo().end();

	while(iter != iter_end)
	{
		uint32 uDimensionID = iter->first;
		uint32 uPrecision = iter->second;

		CAsyAoiDimScene* pDimScene = new CAsyAoiDimScene(this, uPrecision);

		m_mapAoiDimScene.insert(make_pair(uDimensionID, pDimScene));

		++iter;
	}

	(new CAoiCreateSceneJob(m_uId, szSyncAoiFileName, uScale,
		uPixelWidth, uPixelHeight, DimInfo))->Add();
}

CAsyAoiScene::~CAsyAoiScene()
{
	ms_mapIdToAsyScene.erase(m_uId);
}



CAsyAoiScene* CAsyAoiScene::GetAoiScene(uint32 uId)
{
	return ms_mapIdToAsyScene[uId];
}


void CAsyAoiScene::Release()
{
	MapAsyAoiDimScene_t::iterator iter = m_mapAoiDimScene.begin();
	while (iter != m_mapAoiDimScene.end())
	{
		delete iter->second;
		++iter;
	}

	delete this;
}


bool CAsyAoiScene::IsInSyncAoi(const CPos& GridPos)const
{
	return true;
}


CAsyAoiSyncerObj* CAsyAoiScene::CreateSyncerObj(const CFPos& pos,CAoiSyncerObjHandler *pHandler)
{
	CAsyAoiSyncerObj *pObj = new CAsyAoiSyncerObj(this,pos,pHandler);
	//创建同步aoi对象
	pObj->CreateAoiObj(pos, pObj->GetType());

	return pObj;
}

CAsyAoiSynceeObj* CAsyAoiScene::CreateSynceeObj(const CFPos& pos,CAoiSynceeObjHandler *pHandler)
{
	CAsyAoiSynceeObj *pObj = new CAsyAoiSynceeObj(this,pos,pHandler);
	//创建同步aoi对象
	pObj->CreateAoiObj(pos, pObj->GetType());
	
	return pObj;
}

CAsyAoiViewObj* CAsyAoiScene::CreateViewObj(const CFPos& pos,CAoiViewObjHandler *pHandler)
{
	CAsyAoiViewObj *pObj = new CAsyAoiViewObj(this,pos,pHandler);
	//创建同步aoi对象
	pObj->CreateAoiObj(pos, pObj->GetType());

	return pObj;
}

void CAsyAoiScene::DestroyObj(CAsyAoiViewObj* pObj)
{
	delete pObj;
}


uint32 CAsyAoiScene::GetSynSceneId() const
{
	return m_uId;
}

uint32 CAsyAoiScene::GetNextId()
{
	static uint32 uId = 0;
	uId++;
	return uId;
}


CSynAoiScene* CAsyAoiScene::GetSynAoiScene(uint32 uId)
{
	MapSynAoiScene_t::iterator it = ms_mapIdToSynScene.find(uId);
	if (it == ms_mapIdToSynScene.end())
	{
		return NULL;	
	}

	return it->second;
}

bool CAsyAoiScene::AddSynAoiScene(uint32 uGlobalId, CSynAoiScene* pScene)
{
	return ms_mapIdToSynScene.insert(make_pair(uGlobalId, pScene)).second;
}

bool CAsyAoiScene::DelSynAoiScene(uint32 uGlobalId)
{
	return ms_mapIdToSynScene.erase(uGlobalId) == 1;
}


bool CAsyAoiScene::Running()const
{
	return m_eState == eAASS_Running;
}

bool CAsyAoiScene::Halting()const
{
	return m_eState == eAASS_Halting;
}

bool CAsyAoiScene::Halted()const
{
	return m_eState == eAASS_Halted;
}

bool CAsyAoiScene::Halt()
{
	if ( !Running() )
		return false;

	m_eState = eAASS_Halting;

	(new CAoiDestroySceneJob(m_uId))->Add();
	return true;
}

void CAsyAoiScene::OnHalted()
{
	m_eState = eAASS_Halted;
	m_pHandler->OnHalted();
}

CAoiSceneHandler* CAsyAoiScene::GetHandler()
{
	return m_pHandler;
}

