#include "stdafx.h"
#include "CModelProp.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "PropCtrl\CModelPropCtrlImp.h"

#define PropImp ((CPropCtrlImp*)this->GetCoreCtrl())

CModelProp::CModelProp(void* ctrl)
:CBaseProp(ctrl)
{
	m_ModelName="Default Model";
	m_ModelPosX=0.0f;
	m_ModelPosY=0.0f;
	m_ModelPosZ=0.0f;
	m_ModelRotX=0.0f;
	m_ModelRotY=0.0f;
	m_ModelRotZ=0.0f;
	m_ModelScaleX=0.0f;
	m_ModelScaleY=0.0f;
	m_ModelScaleZ=0.0f;
}
CModelProp::~CModelProp()
{
}

void CModelProp::UpdateTimerProp()
{
}

void CModelProp::UpdateProp()
{
	if(PropImp && PropImp->GetModelPropImp())
	{

	}
}
//ID
System::String^  CModelProp::ModelName::get()
{
	return m_ModelName;
}
void CModelProp::ModelName::set(System::String^ value)
{
	m_ModelName=value;
}

//Î»ÖÃ
float  CModelProp::ModelPosX::get()
{
	return m_ModelPosX;
}
void CModelProp::ModelPosX::set(float value)
{
	m_ModelPosX=value;
}
float  CModelProp::ModelPosY::get()
{
	return m_ModelPosY;
}
void CModelProp::ModelPosY::set(float value)
{
	m_ModelPosY=value;
}
float  CModelProp::ModelPosZ::get()
{
	return m_ModelPosZ;
}
void CModelProp::ModelPosZ::set(float value)
{
	m_ModelPosZ=value;
}

//Ðý×ª
float  CModelProp::ModelRotX::get()
{
	return m_ModelRotX;
}
void CModelProp::ModelRotX::set(float value)
{
	m_ModelRotX=value;
}
float  CModelProp::ModelRotY::get()
{
	return m_ModelRotY;
}
void CModelProp::ModelRotY::set(float value)
{
	m_ModelRotY=value;
}
float  CModelProp::ModelRotZ::get()
{
	return m_ModelRotZ;
}
void CModelProp::ModelRotZ::set(float value)
{
	m_ModelRotZ=value;
}

//Ëõ·Å
float  CModelProp::ModelScaleX::get()
{
	return m_ModelScaleX;
}
void CModelProp::ModelScaleX::set(float value)
{
	m_ModelScaleX=value;
}
float  CModelProp::ModelScaleY::get()
{
	return m_ModelScaleY;
}
void CModelProp::ModelScaleY::set(float value)
{
	m_ModelScaleY=value;
}
float  CModelProp::ModelScaleZ::get()
{
	return m_ModelScaleZ;
}
void CModelProp::ModelScaleZ::set(float value)
{
	m_ModelScaleZ=value;
}
