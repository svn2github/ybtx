#include "stdafx.h"
#include "CTreeScene.h"


void CSceneTreeNode::AddChild( CRenderNode* pChild )
{
	CRenderSpace::AddChild(pChild,LT_CENTER,"");
	if(!pChild->GetAABB().isNull())
		InsertNode(pChild);
}

bool CSceneTreeNode::DelChild( CRenderNode* pChild )
{
	bool tf = DeleteNode(pChild);
	CRenderSpace::DelChild(pChild);
	return tf;
}

CSceneTreeNode*	CTreeScene::GetTreeNode( URegionID ID )
{
	ElementPool::iterator it = m_ElePool.find( ID );
	if(it!=m_ElePool.end())
		return (CSceneTreeNode*)it->second;

	return NULL;
}

bool CTreeScene::Intersects( const CVector3f& vec )
{
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	CSceneTreeNode* pNode;
	for (;it!=ite;++it)
	{
		pNode = (CSceneTreeNode*)it->second;
		if ( pNode->IsValid() && pNode->Intersects(vec) )
				return true;
	}
	return false;
}

void CTreeScene::DoCtrl( CtrlFunHandle* pFun,void* param )
{
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	CSceneTreeNode* pNode;
	for (;it!=ite;++it)
	{
		pNode = (CSceneTreeNode*)it->second;
		pNode->DoCtrl(pFun,param);
	}
}

void CTreeScene::Render(uint32 CurTime)
{
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	CSceneTreeNode* pNode;
	for (;it!=ite;++it)
	{
		pNode = (CSceneTreeNode*)it->second;
		pNode->Render(CurTime);
	}
}

void CTreeScene::Update(CCamera* pCam)
{
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	CSceneTreeNode* pNode;
	for (;it!=ite;++it)
	{
		pNode = (CSceneTreeNode*)it->second;
		pNode->Update(pCam);
	}
}
	