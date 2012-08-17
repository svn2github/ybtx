#include "stdafx.h"
#include "CRenderNode.h"
#include "CCamera.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"

const AniResult CRenderNode::Render(  const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	SetParentMatrix(matWorld);
	return Render(0,pRORS);
}

void CRenderNode::SetParentMatrix( const CMatrix& mat4 )
{
	m_matParent = mat4;
	m_bNeedUpdate = true;
}

void CRenderNode::_UpdateEx(void) const
{
	m_matCached *= m_matParent;
}

const CAxisAlignedBox CRenderNode::GetAABB()
{
	CAxisAlignedBox temp = m_OrgAABB;
	temp.transformBox(m_CenterMatrix);
	return temp;
}

IntersectFlag CRenderNode::IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir )
{
	CVector3f Pos = rayPos;
	CVector3f Dir = rayDir;

	CMatrix TempMat = m_CenterMatrix;
	TempMat.Invert();
	Pos.FastMultiply(m_CenterMatrix);
	Dir.Rotate(m_CenterMatrix);
	return  TRUE==CRenderPipeline::GetInst()->boxProbe(m_OrgAABB.getMinimum(), m_OrgAABB.getMaximum(), rayPos, rayDir)?IF_POSSIBLE:IF_IMPOSSIBLE;
}

void CRenderNode::UpdateBox(void)
{
	m_CenterMatrix = GetWorldMatrix();
	//m_CurAABB.transformBox(GetWorldMatrix());
}

// void CRenderNode::RenderDebugLine(void)
// {
// 	if (!ShowBox)
// 		return;
// 
// 	VerColor3D boxPoints[8];
// 	for (int i = 0; i < 8; i++)
// 	{
// 		boxPoints[i].p = m_OrgAABB.getAllCorners()[i] * m_CenterMatrix;
// 		boxPoints[i].diffuse = 0xff00ff00;
// 	}
// 	static VerColor3D linePoints[24];
// 	const TBYTE LINE_INDEX[24] = { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 6, 1, 7, 2, 4, 3, 5 };
// 	for (int i = 0; i< 24; i++)
// 	{
// 		linePoints[i] = boxPoints[LINE_INDEX[i]];
// 	}
// 	CGraphic::GetInst()->DrawPrimitive(PT_LINELIST, linePoints, 24);
// }

void CRenderNode::SetVisible( bool isVisible )
{
	UpdateBox();
	m_isVisible = isVisible;
}

void CRenderNode::SetVisible( CCamera* pCamera )
{
	UpdateBox();
	if(pCamera->IsVisible(m_v3Position))
	{
		SetVisible(true);
		return;
	}

	if( m_OrgAABB.isNull() )
	{
		SetVisible(true);
		return;
	}

	CAxisAlignedBox TempBox = m_OrgAABB;
	TempBox.transformBox(m_CenterMatrix);

	SetVisible ( pCamera->IsVisible(TempBox) );
}



//const CVector3f& CRenderNode::GetBoxScale()	
//{ return m_vScale; }
//
//const CVector3f& CRenderNode::GetBoxOff()	
//{ return m_v3Position; }