#include "stdafx.h"

#include "CRectangle.h"
#include "CoreCommon.h"
#include "CoreCommon.inl"
#include "ThreadHelper.h"
#include "CAoiThreadMgr.h"

#include "CAsyAoiViewObj.h"
#include "CAsyAoiScene.h"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiSynceeObj.h"
#include "CAoiViewObjHandler.h"
#include "CAsyAoiDimObj.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"

#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"
#include "CAoiJob.inl"
#include "PatternRef.inl"

CAsyAoiViewObj::IDMapAsyAoiObject_t CAsyAoiViewObj::ms_IDMapAsyObject;
CAsyAoiViewObj::IDVecSynAoiObject_t CAsyAoiViewObj::ms_vecSynObject;


CAsyAoiViewObj::CAsyAoiViewObj(CAsyAoiScene *pScene, const CFPos& PixelPos,CAoiViewObjHandler *pHandler)
: m_pScene(pScene)
, m_Pos(PixelPos)
, m_eState(eAAOS_Running)
#ifndef _WIN32
, m_mapAoiDimObj(10)
#endif
{
	m_RefsByCoreObj.SetHolder(this);
	m_Handler.SetHolder(this);
	if(pHandler)
		m_Handler.Attach(pHandler->m_RefsByAoiObj);
	m_uId = ms_IDMapAsyObject.Add( this );
}

CAsyAoiViewObj::~CAsyAoiViewObj()
{
	Ver( ms_IDMapAsyObject.Del( GetGlobalID() ) );

	m_Handler.Detach();
	
	MapDimObj_t::iterator iter = m_mapAoiDimObj.begin();
	while (iter != m_mapAoiDimObj.end())
	{
		iter->second->Release();
		++iter;
	}
}

void CAsyAoiViewObj::CreateAoiObj(const CFPos &pos, EAoiObjType type)
{
	Ast( Running() );

	//创建同步aoi对象
	(new CAoiCreateObjJob(m_uId, m_pScene->GetSynSceneId(),GetHandler()->GetCoreObjID(),pos, type))->Add();
}

uint32 CAsyAoiViewObj::GetGlobalID() const
{
	return m_uId;
}

CAsyAoiViewObj* CAsyAoiViewObj::GetAoiObject(uint32 id)
{
	CAsyAoiViewObj* pObj;
	return ms_IDMapAsyObject.Get( pObj , id ) ? pObj: NULL;
}

CSynAoiViewObj* CAsyAoiViewObj::GetSynAoiObject(uint32 uID)
{
	const uint32 uIndex = IDMapAsyAoiObject_t::GetIndexByID(uID);
	if( ms_vecSynObject.size() <= uIndex )
		return NULL;
	return ms_vecSynObject[ uIndex ];
}

void CAsyAoiViewObj::AddSynAoiObject(uint32 uID, CSynAoiViewObj* pObj)
{
	const uint32 uIndex = IDMapAsyAoiObject_t::GetIndexByID(uID);
	if( ms_vecSynObject.size() <= uIndex )
		ms_vecSynObject.resize( uIndex+1 );
	ms_vecSynObject[uIndex] = pObj;
}

void CAsyAoiViewObj::DelSynAoiObject(uint32 uID)
{
	const uint32 uIndex = IDMapAsyAoiObject_t::GetIndexByID(uID);
	ms_vecSynObject[uIndex] = NULL;
}

CAsyAoiDimObj* CAsyAoiViewObj::GetAoiDimObj(uint32 uDimensionId) const
{
	MapDimObj_t::const_iterator iter = m_mapAoiDimObj.find(uDimensionId);
	if (iter != m_mapAoiDimObj.end())
		return iter->second;

	return NULL;
}

CAoiViewObjHandler* CAsyAoiViewObj::GetHandler() const
{
	return m_Handler.Get();
}

void CAsyAoiViewObj::SetSize( float fSize, uint32 uDimension )
{
	if (!GetAoiDimObj(uDimension))
	{
		CAsyAoiDimObj* pDimObj = new CAsyAoiDimObj(m_pScene->m_mapAoiDimScene[uDimension],
			this, m_Pos);

		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	CAsyAoiDimObj* pDimObj = m_mapAoiDimObj[uDimension];

	if (pDimObj->GetSize() == fSize)
		return;

	pDimObj->SetSize(fSize);

	if ( !Running() )
		return;

	(new CAoiSetSizeJob(m_uId, fSize, uDimension))->Add();
}

void CAsyAoiViewObj::SetEyeSight( float fEyeSight, uint32 uDimension )
{
	if (!GetAoiDimObj(uDimension))
	{
		CAsyAoiDimObj* pDimObj = new CAsyAoiDimObj(m_pScene->m_mapAoiDimScene[uDimension],
			this, m_Pos);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	CAsyAoiDimObj* pDimObj = m_mapAoiDimObj[uDimension];

	if (pDimObj->GetEyeSight() == fEyeSight)
		return;

	pDimObj->SetEyeSight(fEyeSight);


	if ( !Running() )
		return;

	(new CAoiSetEyeSightJob(m_uId, fEyeSight, uDimension))->Add();
}

void CAsyAoiViewObj::SetStealth( float fStealth, uint32 uDimension )
{
	if (!GetAoiDimObj(uDimension))
	{
		CAsyAoiDimObj* pDimObj = new CAsyAoiDimObj(m_pScene->m_mapAoiDimScene[uDimension],
			this, m_Pos);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	CAsyAoiDimObj* pDimObj = m_mapAoiDimObj[uDimension];

	if (pDimObj->GetStealth() == fStealth)
		return;

	pDimObj->SetStealth(fStealth);

	if ( !Running() )
		return;

	(new CAoiSetStealthJob(m_uId, fStealth, uDimension))->Add();
}

void CAsyAoiViewObj::SetKeenness(float fKeenness, uint32 uDimension)
{
	if (!GetAoiDimObj(uDimension))
	{
		CAsyAoiDimObj* pDimObj = new CAsyAoiDimObj(m_pScene->m_mapAoiDimScene[uDimension],
			this, m_Pos);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	CAsyAoiDimObj* pDimObj = m_mapAoiDimObj[uDimension];

	if (pDimObj->GetKeenness() == fKeenness)
		return;

	pDimObj->SetKeenness(fKeenness);

	if ( !Running() )
		return;

	(new CAoiSetKeennessJob(m_uId, fKeenness, uDimension))->Add();
}

void CAsyAoiViewObj::SetViewGroup(uint32 uViewGroup, uint32 uDimension)
{
	if (!GetAoiDimObj(uDimension))
	{
		CAsyAoiDimObj* pDimObj = new CAsyAoiDimObj(m_pScene->m_mapAoiDimScene[uDimension],
			this, m_Pos);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	CAsyAoiDimObj* pDimObj = m_mapAoiDimObj[uDimension];

	if (pDimObj->GetViewGroup() == uViewGroup)
		return;

	pDimObj->SetViewGroup(uViewGroup);

	if ( !Running() )
		return;

	(new CAoiSetViewGroupJob(m_uId, uViewGroup, uDimension))->Add();
}

const CFPos& CAsyAoiViewObj::GetPos() const
{
	return m_Pos;
}

void CAsyAoiViewObj::SetPos(const CFPos& PixelPos)
{
	if ( m_Pos == PixelPos)
		return;
	
	m_Pos = PixelPos;

	if ( !Running() )
		return;

	(new CAoiSetPosJob(m_uId, PixelPos))->Add();
}

void CAsyAoiViewObj::ForceSetPos(const CFPos& PixelPos)
{
	//可能之前也设置了同样的坐标，但是在SyncAoi对象的pos由于精度的关系不一定真的修改了，所以这个job一定要发过去
	m_Pos = PixelPos;

	if ( !Running() )
		return;

	(new CAoiForceSetPosJob(m_uId, PixelPos))->Add();
}

float CAsyAoiViewObj::GetSize(uint32 uDimension) const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetSize();

}

float CAsyAoiViewObj::GetEyeSight(uint32 uDimension) const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetEyeSight();
}

float CAsyAoiViewObj::GetStealth(uint32 uDimension) const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetStealth();
}

float CAsyAoiViewObj::GetKeenness(uint32 uDimension) const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetKeenness();
}

uint32 CAsyAoiViewObj::GetViewGroup(uint32 uDimension) const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetViewGroup();
}

float CAsyAoiViewObj::GetDim0Size() const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(0);
	if(!pDimObj)
		return 0.0f;

	return pDimObj->GetSize();
}

float CAsyAoiViewObj::GetDim0Stealth() const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(0);
	if(!pDimObj)
		return 0.0f;

	return pDimObj->GetStealth();
}

uint32 CAsyAoiViewObj::GetDim0ViewGroup() const
{
	CAsyAoiDimObj* pDimObj = GetAoiDimObj(0);
	if(!pDimObj)
		return 0;

	return pDimObj->GetViewGroup();
}


void CAsyAoiViewObj::GetDimensions(vector<uint32>& vecDim) const
{
	MapDimObj_t::const_iterator iter = m_mapAoiDimObj.begin();
	while(iter != m_mapAoiDimObj.end())
	{
		vecDim.push_back(iter->first);
		++iter;
	}
}

const CAsyAoiSyncerObj* CAsyAoiViewObj::CastToSyncerObj()const
{
	return GetType()!=eAOT_ViewObj ? static_cast<const CAsyAoiSyncerObj*>(this) : NULL;
}

CAsyAoiSyncerObj* CAsyAoiViewObj::CastToSyncerObj()
{
	return const_cast<CAsyAoiSyncerObj*>
		(const_cast<const CAsyAoiViewObj*>(this)->CastToSyncerObj());
}

const CAsyAoiSynceeObj* CAsyAoiViewObj::CastToSynceeObj()const
{
	return GetType()==eAOT_SynceeObj ? static_cast<const CAsyAoiSynceeObj*>(this) : NULL;
}

CAsyAoiSynceeObj* CAsyAoiViewObj::CastToSynceeObj()
{
	return const_cast<CAsyAoiSynceeObj*>
		(const_cast<const CAsyAoiViewObj*>(this)->CastToSynceeObj());
}

bool CAsyAoiViewObj::CanBeSync()const
{
	return CastToSyncerObj()!=NULL;
}


void CAsyAoiViewObj::OnViewRelationChanged(CAsyAoiViewObj* pObj, bool bSee, uint32 uDimension)
{
	GetHandler()->OnViewRelationChanged(pObj,bSee, uDimension);
}

bool CAsyAoiViewObj::Running()const
{
	return m_eState == eAAOS_Running;
}

bool CAsyAoiViewObj::Halting()const
{
	return m_eState == eAAOS_Halting;
}

bool CAsyAoiViewObj::Halted()const
{
	return m_eState == eAAOS_Halted;
}

bool CAsyAoiViewObj::Halt()
{
	if ( !Running() )
		return false;

	m_eState = eAAOS_Halting;

	(new CAoiDestroyObjJob(m_uId, m_pScene->GetSynSceneId()))->Add();
	return true;
}

void CAsyAoiViewObj::OnHalted()
{
	m_eState = eAAOS_Halted;
	GetHandler()->OnHalted();
}


void CAsyAoiViewObj::PostMsgToViewee(const void* pContext, uint32 uDimension)
{
	if ( !Running() )
		return;

	if (!GetAoiDimObj(uDimension))
		return;

	(new CAoiPostMsgToVieweeJob(m_uId, pContext, uDimension))->Add();
}

void CAsyAoiViewObj::PostMsgToViewer(const void* pContext, uint32 uDimension)
{
	if (!Running() )
		return;

	if (!GetAoiDimObj(uDimension))
		return;

	(new CAoiPostMsgToViewerJob(m_uId, pContext, uDimension))->Add();
}

void CAsyAoiViewObj::PostMsgToSelf(const void* pContext)
{
	if (!Running() )
		return;

	(new CAoiPostMsgToSelfJob(m_uId, pContext))->Add();
}

void CAsyAoiViewObj::OnMsgFromViewer(const void* pContext)
{
	GetHandler()->OnMsgFromViewer(pContext);
}

void CAsyAoiViewObj::OnMsgFromViewee(const void* pContext)
{
	GetHandler()->OnMsgFromViewee(pContext);
}

void CAsyAoiViewObj::OnMsgToVieweeHandled(const void* pContext)
{
	GetHandler()->OnMsgToVieweeHandled(pContext);
}

void CAsyAoiViewObj::OnMsgToViewerHandled(const void* pContext)
{
	GetHandler()->OnMsgToViewerHandled(pContext);
}

void CAsyAoiViewObj::OnMsgFromSelf(const void* pContext)
{
	GetHandler()->OnMsgFromSelf(pContext);
}

