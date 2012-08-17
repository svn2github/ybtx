#include "stdafx.h"
#include "CCameraController.h"
#include "CCamera.h"

bool CCameraController::_IsValid( CCtrlNode* pNode )
{
	return pNode->GetNodeType() == CCamera::st_CameraType;
}

void CCameraController::SetTargetPos(const CVector3f& vec)
{
	m_TargetPos = vec;
}

void CCameraController::SetTargetPos(float x, float y,float z)
{
	SetTargetPos(CVector3f(x,y,z));
}