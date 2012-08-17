#include "stdafx.h"
#include "CLight.h"

CLight::CLight()
{
	m_ListStruct.Type			= LIGHT_DIRECTIONAL;
	m_ListStruct.Diffuse		= CColor4::White;				/* Diffuse color of light */
	m_ListStruct.Specular		= CColor4::White;				/* Specular color of light */
	m_ListStruct.Ambient		= CColor4::Black;				/* Ambient color of light */
	m_ListStruct.Position		= m_v3Position;					/* Position in world space */
	m_ListStruct.Direction		= CVector3f(0.0f,0.0f,1.0f);	/* Direction in world space */

	m_ListStruct.Range			= 0.0f;							/* Cutoff range */
	m_ListStruct.Falloff			= 0.0f;							/* Falloff */
	m_ListStruct.Attenuation0	= 0.0f;							/* Constant attenuation */
	m_ListStruct.Attenuation1	= 0.0f;							/* Linear attenuation */
	m_ListStruct.Attenuation2	= 0.0f;							/* Quadratic attenuation */
	m_ListStruct.Theta			= 0.0f;							/* Inner angle of spotlight cone */
	m_ListStruct.Phi			= 0.0f;							/* Outer angle of spotlight cone */
};	

CLight::~CLight()
{

};

void	CLight::_UpdateEx(void) const
{
	m_ListStruct.Position = m_v3Position;
	m_ListStruct.Direction	= getDIRECTION();
}

const CLight&	CLight::operator = (const CLight& light)
{
	m_ListStruct	= light.m_ListStruct;

	m_Quaternion	= light.m_Quaternion;
	m_v3Position	= light.m_v3Position;
	m_vScale		= light.m_vScale;
	m_matCached		= light.m_matCached; 
	m_bNeedUpdate	= light.m_bNeedUpdate;

	return *this;
}