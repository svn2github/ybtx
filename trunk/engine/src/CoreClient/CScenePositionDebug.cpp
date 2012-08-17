#include "stdafx.h"
#include "CScenePositionDebug.h"
#include "CCoreSceneClient.h"
#include "CRenderSystemClient.h"
#include "CRenderSystem.h"
#include "CGraphic.h"
#include "CSceneManagerClient.h"
#include "CoreCommon.h"

void CScenePositionDebugNode::RenderDebug()
{
	const int MaxSize = 30;
	const int BufferSize = MaxSize*MaxSize;
	VerColorSize pPointBuf[BufferSize];
	int psize = 0;
	CGraphic* pGraphic = CGraphic::GetInst();
	CRenderSpace::SpaceNodeList::iterator it,eit = CClientRenderScene::GetInst()->GetRenderSceneQuene().m_listNode.end();
	for( it = CClientRenderScene::GetInst()->GetRenderSceneQuene().m_listNode.begin(); it!=eit; ++it )
	{
		CRenderObject* pObj = (CRenderObject*)(*it);
		pObj->GetRootPosition( &pPointBuf[psize].p );//CVector3f( (.x, Y , pObj->GetCenterPosition().z );
		pPointBuf[psize].diffuse	= 0xFFFF8000;
		pPointBuf[psize].size		= 10; 
		++psize;
	}

	if(psize!=0)
	{
		VerColorSize* pPVB;
		CMatrix* pMat;
		if( pMat = pGraphic->GetRenderStack( RS_LINE, NULL, NULL, PT_POINTLIST, psize, psize,
			VerColorSize::Format, sizeof(VerColorSize), (void**)&pPVB, NULL ) )
		{
			pMat->Identity();
			memcpy(pPVB,pPointBuf,sizeof(VerColorSize)*psize);
		}
	}
}