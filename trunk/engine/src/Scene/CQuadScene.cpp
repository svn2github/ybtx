#include "stdafx.h"
#include "CQuadScene.h"
#include "BaseHelper.h"
#include "CRenderNode.h"
#include "CGraphic.h"
#include "CGraphicExp.h"
#include "CRenderModel.h"
//#include "CRenderDebugFun.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------
//------------------------------------------------------------------------------
namespace sqr
{
const uint32	stMaxQuadNode = 10;
const float		stMaxQuadSize = 64.0f * 8.0f;


CElementNode*	CQuadTreeScene::_CreateNode( URegionID ID )
{
	float X = ID.X;// % m_pSceneClient->GetWidth();
	float Y = ID.Z;// / m_pSceneClient->GetWidth();

	float Width = float(eSRegionSpan * eSGridSpan);
	X *= Width;
	Y *= Width;

	return new CQuadTreeSpace(this,ID,NULL,X,Y,X + Width, Y + Width );
}

#pragma region CQuadNode

//------------------------------------------------------------------------------
CQuadNode::CQuadNode(CElementManager* pParent , URegionID id, CQuadTreeSpace* pQuadParent
					 , float MinX, float MinY ,float MaxX,float MaxY)
: CSceneTreeNode(pParent,id)
, m_pQuadParent(pQuadParent)
, MX(MaxX), MY(MaxY), mx(MinX), my(MinY)
, m_eClipStatus(ClipStatus::Invalid)
{
	miX = (MaxX + MinX) / 2;
	miY = (MaxY + MinY) / 2;
}

//------------------------------------------------------------------------------
void CQuadNode::Render( uint32 CurTime )
{
	Ast(ClipStatus::Invalid != m_eClipStatus);

	if (ClipStatus::Outside != m_eClipStatus)
		this->_Render(CurTime);
	else
		this->_unRender();
}

//------------------------------------------------------------------------------
void CQuadNode::Update(CCamera* pCam)
{
	this->_UpdateClipStatus(ClipStatus::Intersect,pCam);
}

#pragma endregion CQuadNode
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
#pragma region CQuadTreeSpace
//------------------------------------------------------------------------------
CQuadTreeSpace::CQuadTreeSpace(CElementManager* pParent , URegionID id, CQuadTreeSpace* pQuadParent
							 , float MinX, float MinY ,float MaxX,float MaxY)
: CQuadNode(pParent,id,pQuadParent,MinX,MinY,MaxX,MaxY)
{
	m_IsRendering = false;
	m_childTree[0][0] = new CQuadTreeNode(pParent,id,this,MinX,	MinY,	miX,	miY	);
	m_childTree[0][1] = new CQuadTreeNode(pParent,id,this,MinX,	miY,	miX,	MaxY);
	m_childTree[1][0] = new CQuadTreeNode(pParent,id,this,miX,	MinY,	MaxX,	miY	);
	m_childTree[1][1] = new CQuadTreeNode(pParent,id,this,miX,	miY,	MaxX,	MaxY);
}

//------------------------------------------------------------------------------
CQuadTreeSpace::~CQuadTreeSpace()
{
	GfkSafeDelete(m_childTree[0][0]);
	GfkSafeDelete(m_childTree[0][1]);
	GfkSafeDelete(m_childTree[1][0]);
	GfkSafeDelete(m_childTree[1][1]);
}

//------------------------------------------------------------------------------
void	CQuadTreeSpace::InsertNode(CRenderNode* pObj)
{
	m_AABB.mergeBox(pObj->GetAABB());
	m_childTree[(pObj->GetPosition().x * 0.5f/*/2*/)>miX][(pObj->GetPosition().z * 0.5f)>miY]->InsertNode(pObj);
}

bool	CQuadTreeSpace::DeleteNode(CRenderNode* pObj)
{
	if( m_childTree[(pObj->GetPosition().x * 0.5f/*/2*/)>miX][(pObj->GetPosition().z * 0.5f)>miY]->DeleteNode(pObj)
		|| m_childTree[0][0]->DeleteNode(pObj) || m_childTree[0][1]->DeleteNode(pObj)
		|| m_childTree[1][0]->DeleteNode(pObj) || m_childTree[1][1]->DeleteNode(pObj) )
	{
		m_AABB.setNull();
		m_AABB.mergeBox(m_childTree[0][0]->GetAABB());
		m_AABB.mergeBox(m_childTree[0][1]->GetAABB());
		m_AABB.mergeBox(m_childTree[1][0]->GetAABB());
		m_AABB.mergeBox(m_childTree[1][1]->GetAABB());
		return true;
	}
	return false;
}

void	CQuadTreeSpace::DoCtrl( CtrlFunHandle* pFun,void* param )
{
	m_childTree[0][0]->DoCtrl(pFun,param);
	m_childTree[0][1]->DoCtrl(pFun,param);
	m_childTree[1][0]->DoCtrl(pFun,param);
	m_childTree[1][1]->DoCtrl(pFun,param);
}

//------------------------------------------------------------------------------
void	CQuadTreeSpace::_Render(uint32 CurTime)
{
	m_IsRendering = true;
	m_childTree[0][0]->Render(CurTime);
	m_childTree[0][1]->Render(CurTime);
	m_childTree[1][0]->Render(CurTime);
	m_childTree[1][1]->Render(CurTime);
}

//------------------------------------------------------------------------------
void	CQuadTreeSpace::_unRender()
{
	if ( m_IsRendering )
	{
		m_IsRendering = false;
		m_childTree[0][0]->_unRender();
		m_childTree[0][1]->_unRender();
		m_childTree[1][0]->_unRender();
		m_childTree[1][1]->_unRender();
	}
}

bool CQuadTreeSpace::Intersects( const CVector3f& vec )
{
	if( m_AABB.intersects(vec) )
	{
		if( m_childTree[0][0]->Intersects(vec) ||
			m_childTree[0][1]->Intersects(vec) ||
			m_childTree[1][0]->Intersects(vec) ||
			m_childTree[1][1]->Intersects(vec) )
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void	CQuadTreeSpace::_UpdateClipStatus( ClipStatus::Type status,CCamera* pCam )
{
	Ast(ClipStatus::Outside != status && ClipStatus::Invalid != status);

	if(ClipStatus::Inside == status)
	{
		m_eClipStatus = status;
	}
	else
	{
		m_eClipStatus = pCam->cull(m_AABB);
		if (ClipStatus::Outside == m_eClipStatus)
			return;
	}
	
	m_childTree[0][0]->_UpdateClipStatus(m_eClipStatus,pCam);
	m_childTree[0][1]->_UpdateClipStatus(m_eClipStatus,pCam);
	m_childTree[1][0]->_UpdateClipStatus(m_eClipStatus,pCam);
	m_childTree[1][1]->_UpdateClipStatus(m_eClipStatus,pCam);
}
#pragma endregion CQuadTreeSpace
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
#pragma region CQuadTreeNode
//------------------------------------------------------------------------------
CQuadTreeNode::CQuadTreeNode(CElementManager* pParent , URegionID id, CQuadTreeSpace* pQuadParent
					 , float MinX, float MinY ,float MaxX,float MaxY)
: CQuadNode(pParent,id,pQuadParent,MinX,MinY,MaxX,MaxY)
{
	m_IsRendering = false;
}

bool CQuadTreeNode::DeleteNode(CRenderNode* pObj)
{
	NodeList::iterator it = m_List.begin();
	NodeList::iterator eit = m_List.end();
	for ( ;it!=eit;++it)
	{
		if(pObj == *it)
		{
			m_List.erase(it);
			m_AABB.setNull();
			NodeList::iterator bit = m_List.begin();
			NodeList::iterator beit = m_List.end();
			for ( ;bit!=beit;++bit)
			{
				m_AABB.mergeBox((*bit)->GetAABB());
			}
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void	CQuadTreeNode::InsertNode(CRenderNode* pObj)
{
	m_AABB.mergeBox(pObj->GetAABB());
	m_List.push_back(pObj);
	CVector3f BoxSize = m_AABB.getSize();

	if( stMaxQuadNode > m_List.size() ||
		stMaxQuadSize > MX - mx		  ||
		stMaxQuadSize > MY - my		  )
		return;

	CQuadNode** p = NULL;
	CQuadNode*	pNode = new CQuadTreeSpace(GetParentManager(),GetID(),m_pQuadParent,mx,my,MX,MY);
	NodeList::iterator it = m_List.begin();
	NodeList::iterator eit = m_List.end();

	for ( ;it!=eit;++it)
	{
		pNode->InsertNode((*it));
	};
	
	if( this == m_pQuadParent->m_childTree[0][0] )
		m_pQuadParent->m_childTree[0][0] = pNode;
	else if( this == m_pQuadParent->m_childTree[0][1] )
		m_pQuadParent->m_childTree[0][1] = pNode;
	else if( this == m_pQuadParent->m_childTree[1][0] )
		m_pQuadParent->m_childTree[1][0] = pNode;
	else if( this == m_pQuadParent->m_childTree[1][1] )
		m_pQuadParent->m_childTree[1][1] = pNode;

	delete this;
}

void	CQuadTreeNode::DoCtrl( CtrlFunHandle* pFun,void* param )
{
	NodeList::iterator it = m_List.begin();
	NodeList::iterator eit = m_List.end();
	for ( ;it!=eit;++it)
	{	
		pFun(*it,param);
	}
}

//------------------------------------------------------------------------------
void	CQuadTreeNode::_Render(uint32 CurTime)
{
	m_IsRendering = true;
	NodeList::iterator it = m_List.begin();
	NodeList::iterator eit = m_List.end();
	for ( ;it!=eit;++it)
	{
		SQR_TRY
		{
			if ((*it)->IsVisible())
			{
				//RenderBigBoxDebug((CRenderObject*)(*it));
				(*it)->OnPreRender(CurTime,NULL);
				(*it)->Render();
			}
		}
		SQR_CATCH(exp) {
			GfkLogExpOnce(exp);
		}
		SQR_TRY_END;
	}
}

//------------------------------------------------------------------------------
void	CQuadTreeNode::_unRender()
{
	if ( m_IsRendering )
	{
		NodeList::iterator it,eit = m_List.end();
		for( it = m_List.begin(); it!=eit; ++it )
		{
			(*it)->SetVisible(false);
		}
	}
}

//------------------------------------------------------------------------------
void	CQuadTreeNode::_UpdateClipStatus( ClipStatus::Type status,CCamera* pCam )
{
	Ast(ClipStatus::Outside != status && ClipStatus::Invalid != status);

	if(ClipStatus::Intersect == status)
	{
		m_eClipStatus = pCam->cull(m_AABB);
		if (ClipStatus::Outside == m_eClipStatus)
			return;
	}
	else
	{
		m_eClipStatus = status;
	}

	// update children status
	if (ClipStatus::Intersect == m_eClipStatus)
	{
		for (NodeList::iterator it = m_List.begin(); it != m_List.end(); ++it)
		{
			(*it)->SetVisible(pCam);
		}
	}
	else// inside
	{
		for (NodeList::iterator it = m_List.begin(); it != m_List.end(); ++it)
		{
			(*it)->SetVisible(true);
		}
	}
}

bool CQuadTreeNode::Intersects( const CVector3f& vec )
{
	if( m_AABB.intersects(vec) )
	{
		NodeList::iterator it = m_List.begin();
		NodeList::iterator eit = m_List.end();
		for ( ;it!=eit;++it )
			if ((*it)->GetType() == ROT_MODEL && ((CRenderModel*)(*it))->Intersects(vec))			
				return true;
	}
	return false;
}

#pragma endregion CQuadTreeNode

}// namespace sqr