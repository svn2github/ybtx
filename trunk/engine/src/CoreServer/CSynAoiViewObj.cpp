#include "stdafx.h" 
#include "CSynAoiViewObj.h" 
#include "CSynAoiScene.h"
#include "CSynAoiViewObj.h"
#include "CSynAoiSynceeObj.h"
#include "CRectangle.h"
#include "CoreCommon.h"
#include "CoreCommon.inl"

#include "CSynAoiDimObj.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"

#include "CAoiThreadMgr.h"
#include "TSqrAllocator.inl"
#include "CAoiResult.inl"
#include "CLogicThreadResult.h"
#include "CLogicThreadResult.inl"

CSynAoiViewObj::CSynAoiViewObj(CSynAoiScene *pScene, const CFPos& PixelPos, uint32 uId, uint32 uCoreObjId)
: m_pScene(pScene)
, m_Pos(PixelPos)
, m_uId(uId)
, m_uCoreObjId(uCoreObjId)
#ifndef _WIN32
, m_mapAoiDimObj(10)
#endif 
{ 
	
} 

CSynAoiViewObj::~CSynAoiViewObj()
{
	MapDimObj_t::iterator iter = m_mapAoiDimObj.begin();
	while (iter != m_mapAoiDimObj.end())
	{
		iter->second->Release();
		++iter;
	}
}

bool CSynAoiViewObj::CanSee(CSynAoiViewObj* pObj, uint32 uDimension)
{
	return m_mapAoiDimObj[uDimension]->CanSee(pObj->m_mapAoiDimObj[uDimension]);
}

void CSynAoiViewObj::SetSize( float fSize, uint32 uDimension )
{
	Ast(GetScene()->DimensionExist(uDimension));

	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
	{
		pDimObj = new CSynAoiDimObj(GetScene()->m_mapDimScene[uDimension],
			this, m_Pos, uDimension);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	pDimObj->SetSize(fSize);

}

void CSynAoiViewObj::SetEyeSight( float fEyeSight, uint32 uDimension )
{
	Ast(GetScene()->DimensionExist(uDimension));

	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
	{
		pDimObj = new CSynAoiDimObj(GetScene()->m_mapDimScene[uDimension],
			this, m_Pos, uDimension);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	pDimObj->SetEyeSight(fEyeSight);
}

void CSynAoiViewObj::SetStealth( float fStealth, uint32 uDimension )
{
	//cout<<"Syn SetStealth "<<fStealth<<endl;
	Ast(GetScene()->DimensionExist(uDimension));

	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
	{
		pDimObj = new CSynAoiDimObj(GetScene()->m_mapDimScene[uDimension],
			this, m_Pos, uDimension);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	pDimObj->SetStealth(fStealth);
}

void CSynAoiViewObj::SetKeenness(float fKeenness, uint32 uDimension)
{
	Ast(GetScene()->DimensionExist(uDimension));

	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
	{
		pDimObj = new CSynAoiDimObj(GetScene()->m_mapDimScene[uDimension],
			this, m_Pos, uDimension);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	pDimObj->SetKeenness(fKeenness);
}

void CSynAoiViewObj::SetViewGroup(uint32 uViewGroup, uint32 uDimension)
{
	Ast(GetScene()->DimensionExist(uDimension));

	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
	{
		pDimObj = new CSynAoiDimObj(GetScene()->m_mapDimScene[uDimension],
			this, m_Pos, uDimension);
		m_mapAoiDimObj.insert(make_pair(uDimension, pDimObj));
	}

	pDimObj->SetViewGroup(uViewGroup);
}

float CSynAoiViewObj::GetSize(uint32 uDimension) const
{ 
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetSize();
} 

float CSynAoiViewObj::GetEyeSight(uint32 uDimension) const 
{ 
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetEyeSight();
} 

float CSynAoiViewObj::GetStealth(uint32 uDimension) const 
{ 
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetStealth();
} 

float CSynAoiViewObj::GetKeenness(uint32 uDimension) const
{ 
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetKeenness();
}

uint32 CSynAoiViewObj::GetViewGroup(uint32 uDimension) const 
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetViewGroup();
}

void CSynAoiViewObj::GetDimensions(vector<uint32>& vecDim) const
{
	MapDimObj_t::const_iterator iter = m_mapAoiDimObj.begin();
	while(iter != m_mapAoiDimObj.end())
	{
		vecDim.push_back(iter->first);
		++iter;
	}
}

const CFPos& CSynAoiViewObj::GetPos() const
{
	return m_Pos;
}

void CSynAoiViewObj::SetPos(const CFPos& PixelPos)
{
	m_Pos = PixelPos;

	MapDimObj_t::iterator iter = m_mapAoiDimObj.begin();
	while (iter != m_mapAoiDimObj.end())
	{
		iter->second->SetPos(PixelPos);
		++iter;
	}
}

void CSynAoiViewObj::ForceSetPos(const CFPos& PixelPos)
{
	m_Pos = PixelPos;

	MapDimObj_t::iterator iter = m_mapAoiDimObj.begin();
	while (iter != m_mapAoiDimObj.end())
	{
		iter->second->ForceSetPos(PixelPos);
		++iter;
	}
}

const CFPos& CSynAoiViewObj::GetPosition(uint32 uDimension) const
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	Ast(pDimObj);

	return pDimObj->GetPosition();
}

void CSynAoiViewObj::InsertToScene()
{
}

void CSynAoiViewObj::RemoveFrScene()
{
	MapDimObj_t::iterator iter = m_mapAoiDimObj.begin();
	while (iter != m_mapAoiDimObj.end())
	{
		iter->second->RemoveFrScene();
		++iter;
	}
}

CSynAoiDimObj* CSynAoiViewObj::GetAoiDimObj(uint32 uId) const
{
	MapDimObj_t::const_iterator iter = m_mapAoiDimObj.find(uId);
	if (iter != m_mapAoiDimObj.end())
	{
		return iter->second;
	}

	return NULL;
}


void CSynAoiViewObj::OnViewRelationChanged(CSynAoiViewObj* pViewObj,
					  const CFPos& MyGridPos,const CFPos& OtGridPos,bool bSee, uint32 uDimension)
{
	//在这里触发回调
	HandleViewRelationChanged(pViewObj, bSee, uDimension);
}


void CSynAoiViewObj::OnViewRelationKept(CSynAoiViewObj* pOtObj,
				   const CFPos& MyOldGridPos,const CFPos& MyGridPos,
				   const CFPos& OtOldGridPos,const CFPos& OtGridPos, uint32 uDimension)
{
}


EAoiObjType CSynAoiViewObj::GetType()const
{
	return eAOT_ViewObj;
}

const CSynAoiSyncerObj* CSynAoiViewObj::CastToSyncerObj()const
{
	return GetType()!=eAOT_ViewObj ? static_cast<const CSynAoiSyncerObj*>(this) : NULL;
}

CSynAoiSyncerObj* CSynAoiViewObj::CastToSyncerObj()
{
	return const_cast<CSynAoiSyncerObj*>
		(const_cast<const CSynAoiViewObj*>(this)->CastToSyncerObj());
}

const CSynAoiSynceeObj* CSynAoiViewObj::CastToSynceeObj()const
{
	return GetType()==eAOT_SynceeObj ? static_cast<const CSynAoiSynceeObj*>(this) : NULL;
}

CSynAoiSynceeObj* CSynAoiViewObj::CastToSynceeObj()
{
	return const_cast<CSynAoiSynceeObj*>
		(const_cast<const CSynAoiViewObj*>(this)->CastToSynceeObj());
}


bool CSynAoiViewObj::CanBeSync()const
{
	return CastToSyncerObj()!=NULL;
}


void CSynAoiViewObj::PostMsgToViewee(const void* pContext, uint32 uDimension)
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
		return;

	pDimObj->PostMsgToViewee(pContext);
}

void CSynAoiViewObj::PostMsgToViewer(const void* pContext, uint32 uDimension)
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(uDimension);
	if(!pDimObj)
		return;

	pDimObj->PostMsgToViewer(pContext);
}

void CSynAoiViewObj::HandleViewRelationChanged(CSynAoiViewObj* pViewObj,bool bSee, uint32 uDimension)
{
	//第0层AOI消息投到引擎线程用于同步,其它层直接投到逻辑线程
	if (uDimension == 0)
	{
		(new CAoiViewChangedResult(m_uId, pViewObj->GetGlobalID(), bSee, uDimension))->Add();
	}
	else
	{
		(new CAoiViewChangedResultToLogic(GetCoreObjID(), pViewObj->GetCoreObjID(), bSee, uDimension))->Add();
	}
}


void CSynAoiViewObj::OnMsgFromViewer(const void* pContext)
{
	(new CAoiMsgFromViewerHandledResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgFromViewee(const void* pContext)
{
	(new CAoiMsgFromVieweeHandledResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgToVieweeHandled(const void* pContext)
{
	(new CAoiMsgToVieweeHandledEndedResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgToViewerHandled(const void* pContext)
{
	(new CAoiMsgToViewerHandledEndedResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::PostMsgToSelf(const void* pContext)
{
	(new CAoiMsgToSelfHandledResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgFromSyncer(const void* pContext)
{
	(new CAoiMsgFromSyncerHandledResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgToSyncerHandled(const void* pContext)
{
	(new CAoiMsgToSyncerHandledEndedResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgFromSyncee(const void* pContext)
{
	(new CAoiMsgFromSynceeHandledResult(m_uId, pContext))->Add();
}

void CSynAoiViewObj::OnMsgToSynceeHandled(const void* pContext)
{
	(new CAoiMsgToSynceeHandledEndedResult(m_uId, pContext))->Add();
}

