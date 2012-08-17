#include "StdAfx.h"
#include "PropCtrl\CLightPropCtrlImp.h"
#include "CEditContext.h"
#include "CRenderNode.h"

#define LightObj ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CLightPropCtrlImp::CLightPropCtrlImp(void)
{
	m_PropType=PROP_LIGHT;
}

CLightPropCtrlImp::~CLightPropCtrlImp(void)
{
}
//获取选中灯光属性(点光 聚光)
int CLightPropCtrlImp::GetSelLightID()
{ return 0; }
int CLightPropCtrlImp::GetSelLightType()
{ return 0; }
float CLightPropCtrlImp::GetSelLightPosX()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightPosY()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightPosZ()
{ return 0.0f; }
int CLightPropCtrlImp::GetSelLightColor()
{ return 0; }
float CLightPropCtrlImp::GetSelLightDirX()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightDirY()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightDirZ()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightMaxDis()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightFallOff()
{ return 0.0f; } //聚光灯内外衰减
float CLightPropCtrlImp::GetSelLightAtten1()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightAtten2()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightAtten3()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightInnerR()
{ return 0.0f; }
float CLightPropCtrlImp::GetSelLightOuterR()
{ return 0.0f; }
//获取选中灯光属性(点光 聚光)
void CLightPropCtrlImp::SetSelLightID(int light_id)
{   }
void CLightPropCtrlImp::SetSelLightType(int light_type)
{   }
void CLightPropCtrlImp::SetSelLightPos(float px,float py,float pz)
{   }
void CLightPropCtrlImp::SetSelLightColor(int color)
{   }
void CLightPropCtrlImp::SetSelLightDir(float dx,float dy,float dz)
{   }
void CLightPropCtrlImp::SetSelLightMaxDis(float mdis)
{   }
void CLightPropCtrlImp::SetSelLightFallOff(float fall)
{   } //聚光灯内外衰减
void CLightPropCtrlImp::SetSelLightAtten1(float atten1)
{   }
void CLightPropCtrlImp::SetSelLightAtten2(float atten2)
{   }
void CLightPropCtrlImp::SetSelLightAtten3(float atten3)
{   }
void CLightPropCtrlImp::SetSelLightInnerR(float innerR)
{   }
void CLightPropCtrlImp::SetSelLightOuterR(float outerR)
{   }