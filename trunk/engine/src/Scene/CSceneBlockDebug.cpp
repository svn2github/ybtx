#include "stdafx.h"
#include "CSceneBlockDebug.h"
#include "CRenderScene.h"
#include "CLogicDataMgr.h"
#include "CDataScene.h"
#include "CGraphic.h"

void CSceneBlockDebugNode::RenderDebug()
{
	uint32	GridPoint = 6;
	int HalfSize = m_uRange / 2;
	int BufferSize = m_uRange*m_uRange;
	DWORD BarrierColor[3];
	BarrierColor[eBT_LowBarrier-1] =  0x800000FF;
	BarrierColor[eBT_MidBarrier-1] =  0x8000FF00;
	BarrierColor[eBT_HighBarrier-1] = 0x80FF0000;
	EBarrierType BarrierType = eBT_NoBarrier;
	vector<VerColor3D> pBuf(BufferSize * GridPoint);

	VerColor3D* pVB;

	int32	size = 0;
	CGraphic* pGraphic = CGraphic::GetInst();
	CPos tempPos;
	int xd = 0, yd = 0;
	
	CDataScene*		pDataScene = m_pRenderScene->GetDataScene();
	CLogicDataMgr*	pLogicData = pDataScene->GetLogicDataMgr();
	CVector3f posCurCenterGrid = m_pRenderScene->GetCameraDest();
	int GridX = (int)(posCurCenterGrid.x/64);
	int GridY = (int)(posCurCenterGrid.z/64);
	for ( uint32 w = 0; w<m_uRange; ++w )
	{
		for( uint32 h = 0; h<m_uRange && size<BufferSize; ++h)
		{
			tempPos.x = (w - HalfSize) + GridX;
			tempPos.y = (h - HalfSize) + GridY;
			if ( !pDataScene->IsGridValid( tempPos.x, tempPos.y ) )
				continue;


			xd = tempPos.x == 0 ? 0 : -1;
			yd = tempPos.y == 0 ? 0 : -1;

			if ((BarrierType=GetBarrierType(tempPos))<1 )
				continue;
			

			if(0==(tempPos.x + tempPos.y)%2)
			{
				pBuf[size * GridPoint + 0].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x+xd	 , tempPos.y+yd    )	+ 1.0f,	float(tempPos.y		*eGridSpan) );
				pBuf[size * GridPoint + 1].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x+xd	 , tempPos.y+1+yd  )	+ 1.0f,	float((tempPos.y+1)	*eGridSpan) );
				pBuf[size * GridPoint + 2].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x+1+xd  , tempPos.y+yd    )	+ 1.0f,	float(tempPos.y		*eGridSpan) );
				pBuf[size * GridPoint + 3].p = pBuf[size * GridPoint + 2].p;
				pBuf[size * GridPoint + 4].p = pBuf[size * GridPoint + 1].p;
				pBuf[size * GridPoint + 5].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x+1+xd  , tempPos.y+1+yd  )	+ 1.0f,	float((tempPos.y+1)	*eGridSpan) );
			}
			else
			{
				pBuf[size * GridPoint + 0].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x+xd	 , tempPos.y+yd    )	+ 1.0f,	float(tempPos.y		*eGridSpan) );
				pBuf[size * GridPoint + 1].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x+1+xd  , tempPos.y+1+yd  )	+ 1.0f,	float((tempPos.y+1)	*eGridSpan) );
				pBuf[size * GridPoint + 2].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x+1+xd  , tempPos.y+yd    )	+ 1.0f,	float(tempPos.y		*eGridSpan) );

				pBuf[size * GridPoint + 3].p = pBuf[size * GridPoint + 0].p;
				pBuf[size * GridPoint + 4].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x + xd  , tempPos.y+1+yd  )	+ 1.0f,	float((tempPos.y+1)	*eGridSpan) );
				pBuf[size * GridPoint + 5].p = pBuf[size * GridPoint + 1].p;
			}
				pBuf[size * GridPoint + 0].diffuse = pBuf[size * GridPoint + 1].diffuse = pBuf[size * GridPoint + 2].diffuse =
				pBuf[size * GridPoint + 3].diffuse = pBuf[size * GridPoint + 4].diffuse = pBuf[size * GridPoint + 5].diffuse = BarrierColor[BarrierType-1];

			++size;
		}
	}
	if (size!=0)
	{
		CMatrix* pMat;
		if(	m_RenderObjStyle!=ROS_UNKNOWN )
		{
			if(pMat = pGraphic->GetRenderStack(RS_LINE, m_RenderObjStyle, NULL, NULL, PT_TRIANGLELIST, size*GridPoint, size*2,
				VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL ) )
			{
				pMat->Identity();
				memcpy(pVB,&pBuf[0],sizeof(VerColor3D)*GridPoint*size);
			}
		}
		else
		{
			if(pMat = pGraphic->GetRenderStack(RS_LINE, NULL, NULL, PT_TRIANGLELIST, size*GridPoint, size*2,
				VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL ) )
			{
				pMat->Identity();
				memcpy(pVB,&pBuf[0],sizeof(VerColor3D)*GridPoint*size);
			}
		}
	}
}

CSceneBlockDebugNode::CSceneBlockDebugNode( ERenderObjStyle style , CRenderScene* pScene ) 
: m_pRenderScene(pScene)
, m_RenderObjStyle(style)
, m_uRange(30)
{

}

EBarrierType sqr::CSceneBlockDebugNode::GetBarrierType( CPos pos )
{
	return eBT_NoBarrier;
}