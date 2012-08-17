#include "stdafx.h"
#include "CSceneGridDeubg.h"
#include "CGraphic.h"
#include "CRenderScene.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"

void CSceneGridDebugNode::RenderDebug()
{
	uint32	GridPoing = 8;
	int		BufferSize = m_uRange * 4 * ( m_uRange * 2 + 1 );
	int32   lsize = 0;
	CPos	tempPos;
	vector<VerColor3D> pLineBuf(BufferSize*GridPoing);
	VerColor3D* pVB;


	CDataScene*		pDataScene = m_pRenderScene->GetDataScene();
	CLogicDataMgr*	pLogicData = pDataScene->GetLogicDataMgr();
	CVector3f		posCurCenterGrid = m_pRenderScene->GetCameraDest();
	CGraphic*		pGraphic = CGraphic::GetInst();

	int GridX = (int)((posCurCenterGrid.x + 32) /64);
	int GridY = (int)((posCurCenterGrid.z + 32) /64);
	uint32	UINT_SIZE = eGridSpan / 2;

	const DWORD tLineColor		= 0x80808080;
	const DWORD tTickLineColor	= 0xffffffff;
	const DWORD tCenterLineColor= 0xffff0000;
	DWORD PColor = 0;
	for ( uint32 w = 0; w<m_uRange * 2; ++w )
	{
		for( uint32 h = 0; h<m_uRange * 2; ++h)
		{
			tempPos.x = (w - m_uRange) + GridX;
			tempPos.y = (h - m_uRange) + GridY;

			if( w == 0 )
			{
				pLineBuf[lsize + 0].p = CVector3f( float((tempPos.x)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x - 1, tempPos.y-1 )	+ 1.0f,	float(tempPos.y		*eGridSpan) );
				pLineBuf[lsize + 1].p = CVector3f( float((tempPos.x)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x - 1, tempPos.y   ) + 1.0f,	float((tempPos.y+1)	*eGridSpan) );
				pLineBuf[lsize + 0].diffuse = pLineBuf[lsize + 1].diffuse =	tTickLineColor;
				lsize += 2;
			}

			if( h == 0 )
			{
				pLineBuf[lsize + 0].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x - 1, tempPos.y -1 ) + 1.0f, float((tempPos.y)	*eGridSpan) );
				pLineBuf[lsize + 1].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x	, tempPos.y -1 ) + 1.0f, float((tempPos.y)	*eGridSpan) );	
				pLineBuf[lsize + 0].diffuse = pLineBuf[lsize + 1].diffuse =	tTickLineColor;
				lsize += 2;
			}

			pLineBuf[lsize + 0].p = CVector3f( float(tempPos.x    *eGridSpan)				,	0.0f,	float((tempPos.y+1)	*eGridSpan) - UINT_SIZE);
			pLineBuf[lsize + 1].p = CVector3f( float((tempPos.x+1)*eGridSpan)				,	0.0f,	float((tempPos.y+1)	*eGridSpan)	- UINT_SIZE);
			pLineBuf[lsize + 2].p = CVector3f( float(tempPos.x    *eGridSpan) + UINT_SIZE	,	0.0f,	float(tempPos.y		*eGridSpan));
			pLineBuf[lsize + 3].p = CVector3f( float(tempPos.x    *eGridSpan) + UINT_SIZE	,	0.0f,	float((tempPos.y+1)	*eGridSpan));

			pLineBuf[lsize + 0].p.y = pDataScene->GetOffsetHeightInPixel(pLineBuf[lsize + 0].p.x,pLineBuf[lsize + 0].p.z) + 3.0f;
			pLineBuf[lsize + 1].p.y = pDataScene->GetOffsetHeightInPixel(pLineBuf[lsize + 1].p.x,pLineBuf[lsize + 1].p.z) + 3.0f;
			pLineBuf[lsize + 2].p.y = pDataScene->GetOffsetHeightInPixel(pLineBuf[lsize + 2].p.x,pLineBuf[lsize + 2].p.z) + 3.0f;
			pLineBuf[lsize + 3].p.y = pDataScene->GetOffsetHeightInPixel(pLineBuf[lsize + 3].p.x,pLineBuf[lsize + 3].p.z) + 3.0f;
			
			pLineBuf[lsize + 4].p = CVector3f( float(tempPos.x    *eGridSpan),	pLogicData->GetLogicHight( tempPos.x - 1		, tempPos.y   ) + 1.0f,	float((tempPos.y+1)	*eGridSpan) );
			pLineBuf[lsize + 5].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x			, tempPos.y   ) + 1.0f,	float((tempPos.y+1)	*eGridSpan) );	
			pLineBuf[lsize + 6].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x			, tempPos.y-1 )	+ 1.0f,	float(tempPos.y		*eGridSpan) );
			pLineBuf[lsize + 7].p = CVector3f( float((tempPos.x+1)*eGridSpan),	pLogicData->GetLogicHight( tempPos.x			, tempPos.y   ) + 1.0f,	float((tempPos.y+1)	*eGridSpan) );
		
			pLineBuf[lsize + 0].diffuse = pLineBuf[lsize + 1].diffuse =	tLineColor;
			pLineBuf[lsize + 2].diffuse = pLineBuf[lsize + 3].diffuse = tLineColor;
			if(h == m_uRange - 1)
				pLineBuf[lsize + 4].diffuse = pLineBuf[lsize + 5].diffuse =	tCenterLineColor;
			else
				pLineBuf[lsize + 4].diffuse = pLineBuf[lsize + 5].diffuse =	tTickLineColor;
			
			if(w == m_uRange - 1)
				pLineBuf[lsize + 6].diffuse = pLineBuf[lsize + 7].diffuse = tCenterLineColor;
			else
				pLineBuf[lsize + 6].diffuse = pLineBuf[lsize + 7].diffuse = tTickLineColor;
				
			lsize += GridPoing;
		}
	}

	if (lsize!=0)
	{
		CMatrix* pMat;

		if(	m_RenderObjStyle!=ROS_UNKNOWN )
		{
			if( pMat = pGraphic->GetRenderStack( RS_LINE, ROS_LIGHTMAT1, NULL, NULL, PT_LINELIST, lsize, lsize/2,
				VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL ) )
			{
				pMat->Identity();
				memcpy(pVB,&pLineBuf[0],sizeof(VerColor3D)*lsize);
			}
		}
		else
		{
			if( pMat = pGraphic->GetRenderStack( RS_LINE, NULL, NULL, PT_LINELIST, lsize*GridPoing, lsize/2,
				VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL ) )
			{
				pMat->Identity();
				memcpy(pVB,&pLineBuf[0],sizeof(VerColor3D)*lsize);
			}
		}		
	}
}

CSceneGridDebugNode::CSceneGridDebugNode( ERenderObjStyle style , CRenderScene* pScene ) 
: m_pRenderScene(pScene)
, m_RenderObjStyle(style)
, m_uRange(15)
{
	
}