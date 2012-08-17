#pragma once
#include "CLight.h"
#include "CVector4.h"
#define MAX_DYNAMIC_LIGHT		32	//这个量应该删掉, 由场景管理控制可见灯数

namespace sqr
{
class CRenderGroup;
struct RangeInfo : public CGraphicMallocObject
{
	float range;
	float attenuation0;
	float attenuation1;
	float attenuation2;
};
struct SpotInfo : public CGraphicMallocObject
{
	float lightType; // 1 point, 2 spot; 负值不影响地表
	float fallOff;
	float theta;
	float phi;
};

class CLightSystem : public CGraphicMallocObject
{
	bool					m_bGlobalLightEnable;
	bool					m_bDynamicLightEnable;

	bool					m_bGlobalLightChange;
	uint32					m_ActiveIndex;
	uint32					m_GlobalAmbient[2];
	CColor4					m_GlobalLightColor[2];

	//uint32					m_GlobalSpecular[2];

	CLight					m_GlobalLight;
	uint32					m_GlobalShadowColor;
	
	CColor4					m_DynamicLightColor[MAX_LIGHT_NUM];
	CVector4f				m_DynamicLightPosition[MAX_LIGHT_NUM];
	CVector4f				m_DynamicLightDir[MAX_LIGHT_NUM];
	RangeInfo				m_DynamicLightRangeInfo[MAX_LIGHT_NUM];
	SpotInfo				m_DynamicLightSpotInfo[MAX_LIGHT_NUM];
	bool					m_InfluenceTerrain[MAX_LIGHT_NUM];// 脑残功能

	GVector<CLight>			m_DynamicLights;

	CRenderGroup*			m_ParentGroup;
public:
	CLightSystem(CRenderGroup* pGroup);
	~CLightSystem(void);

	CLight* GetGlobalLight();

	uint32  GetAmbient(uint Index = 0);
	void	SetAmbient( uint32 Color,uint Index = 0  );

#ifndef AMBIENT_SHADOW_COLOR
	void	SetGlobalShadowColor(uint32 color);
	uint32	GetGlobalShadowColor() const;
#endif

	void	SetGlobalLightColor( uint32 Color,uint Index = 0  );
	uint32	GetGlobalLightColor( uint Index );
	void	SetGlobalLightDir( const CVector3f& LightDir );
	void	SetGlobalLightAttribute( const CVector3f& LightDir, uint32 Color );
	void	GetGlobalLightDir( CVector3f& LightDir, const CVector3f& Pos );

	//uint32  GetSpecular(uint Index = 0);
	//void	SetSpecular( uint32 Color,uint Index = 0  );

	void	EnableGlobalLight( bool bEnable );
	void	EnableDynamicLight( bool bEnable );
	void	TurnOnGlobalLight(uint Index = 0);
	void	Reset(void);

	/// 这个也是个临时方案
	void	AddDynamicPointLight( const CVector3f& Pos, float Rang, uint32 Color, float Attn0, float Attn1, float Attn2 );
	///为了地编先临时弄个接口
	void	SetLight(index_t lightIndex, CLight* light, bool isInfluenceTerrain = true);
	//void	SetInfluenceTerrain(bool b);

	void	UpdateLights(const CVector3f& pos, bool updateDynamic = true);
	void	UpdateGlobalLight();
	void	UpdateDynamicLights();

private:
	void SetDynamicLight(index_t lightIndex, CLight& light);
};

//------------------------------------------------------------------------------
inline void CLightSystem::EnableGlobalLight( bool bEnable )
{
	m_bGlobalLightEnable = bEnable;
}

//------------------------------------------------------------------------------
inline void CLightSystem::EnableDynamicLight( bool bEnable )
{
	m_bDynamicLightEnable = bEnable;
}

#ifndef AMBIENT_SHADOW_COLOR
inline uint32
CLightSystem::GetGlobalShadowColor() const
{
	return m_GlobalShadowColor;
}
#endif

//------------------------------------------------------------------------------
inline CLight*
CLightSystem::GetGlobalLight()
{
	return &m_GlobalLight;
}

//------------------------------------------------------------------------------
inline uint32
CLightSystem::GetAmbient( uint Index /*= 0*/ )
{
	return m_GlobalAmbient[Index];
}
};