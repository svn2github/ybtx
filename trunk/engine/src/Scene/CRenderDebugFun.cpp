#include "stdafx.h"
#include "CRenderDebugFun.h"

namespace sqr
{
	void RenderSmallBoxDebug(CRenderObject* pRenderObj,CBaseAABB* pBox)
	{
		if (pRenderObj->GetOrgAABB()->isNull())
			return;

		if (pRenderObj->GetBoxMap()->size()>0)
		{
			DWORD color = 0xff0000ff;
			VerColor3D Box[24];
			CModel::BoxPairPool::iterator it = pRenderObj->GetBoxMap()->begin();
			CModel::BoxPairPool::iterator eit = pRenderObj->GetBoxMap()->end();
			CMatrix	TempMat;

			for (int i = 0;it!=eit;++it,++i)
			{
				color = 0xff0000ff;
				if((*it).pAABB == pBox)
					color = 0xFFFF8000;
				CVector3f center = ((*it).pAABB->m_vMinimum + (*it).pAABB->m_vMaximum)/2;
				CVector3f extent = ((*it).pAABB->m_vMaximum - (*it).pAABB->m_vMinimum)/2;
				pRenderObj->GetMatrix(TempMat,(*it).BoxID,uint(-1));
				Box[0].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
				Box[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
				Box[2].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
				Box[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
				Box[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
				Box[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
				Box[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
				Box[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;

				Box[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
				Box[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
				Box[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
				Box[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;
				Box[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
				Box[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
				Box[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
				Box[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;

				Box[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
				Box[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
				Box[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
				Box[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
				Box[20].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
				Box[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
				Box[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
				Box[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;

				Box[0].diffuse = Box[5].diffuse = Box[10].diffuse = Box[15].diffuse = Box[20].diffuse =
					Box[1].diffuse = Box[6].diffuse = Box[11].diffuse = Box[16].diffuse = Box[21].diffuse =
					Box[2].diffuse = Box[7].diffuse = Box[12].diffuse = Box[17].diffuse = Box[22].diffuse =
					Box[3].diffuse = Box[8].diffuse = Box[13].diffuse = Box[18].diffuse = Box[23].diffuse =
					Box[4].diffuse = Box[9].diffuse = Box[14].diffuse = Box[19].diffuse = color;
				CGraphic::GetInst()->DrawPrimitive( PT_LINELIST,Box,24);
			}
		}
	}

	void RenderBigBoxDebug(CRenderObject* pRenderObj)
	{
		if (pRenderObj->GetOrgAABB()->isNull())
			return;

		VerColor3D boxPoints[24];
		CVector3f center = (pRenderObj->GetOrgAABB()->getMaximum()+pRenderObj->GetOrgAABB()->getMinimum())/2;
		CVector3f extent = (pRenderObj->GetOrgAABB()->getMaximum()-pRenderObj->GetOrgAABB()->getMinimum())/2;
		CMatrix m_CenterMatrix = pRenderObj->GetCenterMatrix();

		boxPoints[0].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[2].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;

		boxPoints[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;

		boxPoints[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
		boxPoints[20].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;
		boxPoints[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;

		boxPoints[0].diffuse = boxPoints[5].diffuse = boxPoints[10].diffuse = boxPoints[15].diffuse = boxPoints[20].diffuse =
			boxPoints[1].diffuse = boxPoints[6].diffuse = boxPoints[11].diffuse = boxPoints[16].diffuse = boxPoints[21].diffuse =
			boxPoints[2].diffuse = boxPoints[7].diffuse = boxPoints[12].diffuse = boxPoints[17].diffuse = boxPoints[22].diffuse =
			boxPoints[3].diffuse = boxPoints[8].diffuse = boxPoints[13].diffuse = boxPoints[18].diffuse = boxPoints[23].diffuse =
			boxPoints[4].diffuse = boxPoints[9].diffuse = boxPoints[14].diffuse = boxPoints[19].diffuse = 0xff00ff00;
		CGraphic::GetInst()->DrawPrimitive(PT_LINELIST, boxPoints, 24);
	}
}
