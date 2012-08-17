#include "StdAfx.h"
#include "PropCtrl\CModelPropCtrlImp.h"
#include "CEditContext.h"
#include "CRenderNode.h"
#include "CEditModel.h"

CModelPropCtrlImp::CModelPropCtrlImp(void)
{
}

CModelPropCtrlImp::~CModelPropCtrlImp(void)
{
}

//获取选中模型的属性
std::string CModelPropCtrlImp::GetSelModelName()
{ 
	return 0; 
}

float CModelPropCtrlImp::GetSelModelPosX()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelPosY()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelPosZ()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelRotateX()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f	rot=model->GetModelRotate();
		return rot.x;
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelRotateY()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f	rot=model->GetModelRotate();
		return rot.y;
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelRotateZ()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f	rot=model->GetModelRotate();
		return rot.z;
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelScaleX()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f	scale=model->GetModelScale();
		return scale.x;
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelScaleY()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f	scale=model->GetModelScale();
		return scale.y;
	}
	return  0.0f;
}
float CModelPropCtrlImp::GetSelModelScaleZ()
{
	CEditModel* model=(CEditModel*)(m_pOwnContext->GetActiveObj());
	if(model)
	{
		CVector3f scale=model->GetModelScale();
		return scale.z;
	}
	return  0.0f;
}

//获取选中模型的属性
void CModelPropCtrlImp::SetSelModelName(std::string name)
{   }
void CModelPropCtrlImp::SetSelModelPos(int px,int py,int pz)
{   }
void CModelPropCtrlImp::SetSelModelScale(int sx,int sy,int sz)
{   }
void CModelPropCtrlImp::SetSelModelRotate(float rx,float ry,float rz)
{   }
