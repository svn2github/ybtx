#include "stdafx.h"
#include "CAudioProp.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "PropCtrl\CAudioPropCtrlImp.h"

#define PropImp ((CPropCtrlImp*)this->GetCoreCtrl())

CAudioProp::CAudioProp(void* ctrl)
:CBaseProp(ctrl)
{
	m_AudioPosX=0.0f;
	m_AudioPosY=0.0f;
	m_AudioPosZ=0.0f;
}

CAudioProp::~CAudioProp()
{
}

void CAudioProp::UpdateTimerProp()
{
}

void CAudioProp::UpdateProp()
{
	if(PropImp && PropImp->GetAudioPropImp())
	{

	}
}

//CamPos
int CAudioProp::AudioRegion::get()
{
	return m_AudioRegion;
}
void CAudioProp::AudioRegion::set(int value)
{
	m_AudioRegion=value;
}
//CamPos
float  CAudioProp::AudioPosX::get()
{
	return m_AudioPosX;
}
void CAudioProp::AudioPosX::set(float value)
{
	m_AudioPosX=value;
}

float  CAudioProp::AudioPosY::get()
{
	return m_AudioPosY;
}
void CAudioProp::AudioPosY::set(float value)
{
	m_AudioPosY=value;
}

float  CAudioProp::AudioPosZ::get()
{
	return m_AudioPosZ;
}
void CAudioProp::AudioPosZ::set(float value)
{
	m_AudioPosZ=value;
}

System::String^  CAudioProp::AudioName::get()
{
	return m_AudioName;
}
void CAudioProp::AudioName::set(System::String^ value)
{
	m_AudioName=value;
}
