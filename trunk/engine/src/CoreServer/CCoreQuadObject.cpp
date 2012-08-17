#include "stdafx.h"
#include "CCoreQuadObject.h"
#include "CSyncCoreObjectServer.h"
#include "CCoreQuadScene.h"
#include "CCoreQuadNode.h"
#include "CoreCommon.h"
#include "QuadTreeFun.h"
#include "ExpHelper.h"

CCoreQuadObject::ListQuadObject_t CCoreQuadObject::ms_DummyList(1, NULL);

CCoreQuadObject::CCoreQuadObject(uint32 uObjID, const CFPos& pos)
: m_uObjectID(uObjID)
, m_Pos(pos)
, m_fSize(0)
, m_pObjectNode(NULL)
, m_pViewRectNode(NULL)
, m_pSizeRectNode(NULL)
{
	m_ItObject = ms_DummyList.begin();
	m_ItSizeRect = ms_DummyList.begin();
	m_ItViewRect = ms_DummyList.begin();
}

CCoreQuadObject::~CCoreQuadObject()
{

}

void CCoreQuadObject::SetPosition(const CFPos& pos)
{
	m_Pos = pos;
}

const CFPos& CCoreQuadObject::GetPosition() const
{
	return m_Pos;
}

void CCoreQuadObject::InsertToScene(CCoreQuadScene* pScene)
{
	if (m_fSize != 0)
	{
		SetSizeRect(m_Pos, pScene->GetSceneBox());
		pScene->InsertSizeRectToQuad(this);
	}
}

void CCoreQuadObject::RemoveFrScene(CCoreQuadScene* pScene)
{
	if (m_pSizeRectNode)
	{
		pScene->RemoveSizeRectFrQuad(this);
	}
}

void CCoreQuadObject::RawSetPos(const CFPos& pos)
{
	m_Pos = pos;
}

void CCoreQuadObject::SetPos(const CFPos& pos, CCoreQuadScene* pScene)
{
	uint32 uMinOff = pScene->GetPrecision();

	if(abs(m_Pos.x - pos.x) > uMinOff || 
		abs(m_Pos.y - pos.y) > uMinOff)
	{
		IntSetPos(pos, pScene);
	}
}

void CCoreQuadObject::ForceSetPos(const CFPos& pos, CCoreQuadScene* pScene)
{
	IntSetPos(pos, pScene);
}

void CCoreQuadObject::IntSetPos(const CFPos& pos, CCoreQuadScene* pScene)
{
	pScene->SetObjectPos(this, pos);
}

void CCoreQuadObject::RawSetSize(float fSize)
{
	m_fSize = fSize;
}

void CCoreQuadObject::SetSize(float fSize, CCoreQuadScene* pScene)
{
	if(fSize > 0)
	{
		if(m_pSizeRectNode)
		{
			m_fSize = fSize;
			pScene->ResetSizeRectInQuad(this, m_Pos);
		}
		else
		{
			//第一次size
			m_fSize = fSize;
			SetSizeRect(m_Pos, pScene->GetSceneBox());
			pScene->InsertSizeRectToQuad(this);
		}
	}
	else
	{
		//从四叉树中删除
		if(m_pSizeRectNode)
		{
			pScene->RemoveSizeRectFrQuad(this);
			m_fSize = 0;
		}
	}
}

void CCoreQuadObject::SetSizeRect(const CFPos& NewPos, const CFRect& rcMaxBox)
{
	GetRadiusRect(m_rcSizeRect, NewPos, m_fSize, rcMaxBox);
}

float CCoreQuadObject::GetSize() const
{
	return m_fSize;
}

void CCoreQuadObject::SetViewRect(const CFPos& NewPos, const CFRect& rcMaxBox)
{
	GenErr("CCoreQuadObject SetViewRect not allowed called");
}

const CFRect& CCoreQuadObject::GetViewRect() const
{
	GenErr("CCoreQuadObject GetViewRect not allowed called");

	static CFRect ls_rcRect;
	return ls_rcRect;
}

const CFRect& CCoreQuadObject::GetSizeRect() const
{
	return m_rcSizeRect;
}
