#include "stdafx.h"
#include "CSceneBoxDeubg.h"
#include "CRenderDebugFun.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

void CSceneBoxDebugNode::RenderDebug()
{
	CGraphic* pGraphic = CGraphic::GetInst();
	CRenderSpace::SpaceNodeList::iterator it,eit = m_pRenderScene->GetRenderSceneQuene().m_listNode.end();
	for( it = m_pRenderScene->GetRenderSceneQuene().m_listNode.begin(); it!=eit; ++it )
	{
		CRenderObject* pObj = (CRenderObject*)(*it);
		RenderBigBoxDebug(pObj);
		RenderSmallBoxDebug(pObj);
		RenderDebugChild(pObj);
	}
}

void CSceneBoxDebugNode::RenderDebugChild(CRenderObject* pRenderObj)
{
	if(pRenderObj->m_listNode.size()==0)
		return;

	for( CRenderSpace::SpaceNodeList::iterator it = pRenderObj->m_listNode.begin(); it != pRenderObj->m_listNode.end(); ++it )
	{
		if((*it)->GetNodeType()=='REOB')
		{
			RenderDebugChild((CRenderObject*)(*it));
			RenderBigBoxDebug((CRenderObject*)(*it));
			RenderSmallBoxDebug((CRenderObject*)(*it));
		}
	}
}