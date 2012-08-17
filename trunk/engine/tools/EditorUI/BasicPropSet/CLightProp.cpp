#include "StdAfx.h"
#include "CLightProp.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "PropCtrl\CLightPropCtrlImp.h"

#define PropImp ((CPropCtrlImp*)this->GetCoreCtrl())

CLightProp::CLightProp(void* ctrl)
:CBaseProp(ctrl)
{
}

CLightProp::~CLightProp()
{
}

void CLightProp::UpdateTimerProp()
{
}

void CLightProp::UpdateProp()
{
	if(PropImp && PropImp->GetLightPropImp())
	{

	}
}

//ID
int  CLightProp::LightID::get()
{
	return m_LightID;
}
void CLightProp::LightID::set(int value)
{
	m_LightID=value;
}

//类型
LIGHTTYPE  CLightProp::LightType::get()
{
	return m_LightType;
}
void CLightProp::LightType::set(LIGHTTYPE value)
{
	m_LightType=value;
}

//颜色
System::Drawing::Color  CLightProp::LightColor::get()
{
	return m_LightColor;
}
void CLightProp::LightColor::set(System::Drawing::Color value)
{
	m_LightColor=value;
}

//灯光位置
float  CLightProp::LightPosX::get()
{
	return m_LightPosX;
}
void CLightProp::LightPosX::set(float value)
{
	m_LightPosX=value;
}
float  CLightProp::LightPosY::get()
{
	return m_LightPosY;
}
void CLightProp::LightPosY::set(float value)
{
	m_LightPosY=value;
}
float  CLightProp::LightPosZ::get()
{
	return m_LightPosZ;
}
void CLightProp::LightPosZ::set(float value)
{
	m_LightPosZ=value;
}

//光向
float  CLightProp::LightDirX::get()
{
	return m_LightDirX;
}
void CLightProp::LightDirX::set(float value)
{
	m_LightDirX=value;
}
float  CLightProp::LightDirY::get()
{
	return m_LightDirY;
}
void CLightProp::LightDirY::set(float value)
{
	m_LightDirY=value;
}
float  CLightProp::LightDirZ::get()
{
	return m_LightDirZ;
}
void CLightProp::LightDirZ::set(float value)
{
	m_LightDirZ=value;
}

//最大距离
float  CLightProp::LightMaxDis::get()
{
	return m_LightMaxDis;
}
void CLightProp::LightMaxDis::set(float value)
{
	m_LightMaxDis=value;
}

//内外衰减
float  CLightProp::LightFalloff::get()
{
	return m_LightFalloff;
}
void CLightProp::LightFalloff::set(float value)
{
	m_LightFalloff=value;
}

//衰减系数
float  CLightProp::LightAtten0::get()
{
	return m_LightAtten0;
}
void CLightProp::LightAtten0::set(float value)
{
	m_LightAtten0=value;
}
float  CLightProp::LightAtten1::get()
{
	return m_LightAtten1;
}
void CLightProp::LightAtten1::set(float value)
{
	m_LightAtten1=value;
}
float  CLightProp::LightAtten2::get()
{
	return m_LightAtten2;
}
void CLightProp::LightAtten2::set(float value)
{
	m_LightAtten2=value;
}

//内径
float  CLightProp::LightInnerR::get()
{
	return m_LightInnerR;
}
void CLightProp::LightInnerR::set(float value)
{
	m_LightInnerR=value;
}

//外径
float  CLightProp::LightOuterR::get()
{
	return m_LightOuterR;
}
void CLightProp::LightOuterR::set(float value)
{
	m_LightOuterR=value;
}