#include "stdafx.h"
#include "CGraphic.h"
#include "CLightSystem.h"
#include "CRenderPipeline.h"
#include "CShadowFilter.h"
#include "CRenderGroup.h"
#include "TSqrAllocator.inl"
#include "CColor4.inl"

CLightSystem::CLightSystem(CRenderGroup* pGroup)
{
	m_bGlobalLightEnable = false;
	m_bDynamicLightEnable = false;
	m_ActiveIndex = 0;
	m_bGlobalLightChange = true;
	m_GlobalAmbient[0] = 0xfff0f0f0;
	m_GlobalAmbient[1] = 0xffdcdcdc;
	m_GlobalLightColor[0] = CColor4(0.47f,0.47f,0.47f,1.0f);
	m_GlobalLightColor[1] = CColor4(0.31f,0.31f,0.31f,1.0f);
	m_ParentGroup = pGroup;

#ifndef AMBIENT_SHADOW_COLOR
	m_GlobalShadowColor = 0xff999999;
#endif

	m_GlobalLight.SetColor( CColor4(0.05f,0.05f,0.05f,1.0f) );
	m_GlobalLight.SetType( LIGHT_DIRECTIONAL );

	for (index_t i = 0; i < MAX_LIGHT_NUM; ++i)
	{
		m_InfluenceTerrain[i] = true;
	}
}

CLightSystem::~CLightSystem(void)
{
}

void	CLightSystem::TurnOnGlobalLight(uint Index )
{
	AstMsg(Index<2, "现在只支持两个不同主灯");
	if ( m_ActiveIndex == Index && !m_bGlobalLightChange )
		return;

	m_GlobalLight.SetColor( m_GlobalLightColor[Index] );
	m_ActiveIndex = Index;
	m_bGlobalLightChange = false;
	UpdateGlobalLight();

}

void CLightSystem::Reset(void)
{
	m_bGlobalLightEnable = true;
	CColor4 color4(m_GlobalShadowColor);
	CRenderPipeline::GetInst()->SetFragmentShaderF(ACT_GLOBAL_SHADOW_COLOR,
		&color4, 1);
	UpdateGlobalLight();
}

#ifndef AMBIENT_SHADOW_COLOR
void
CLightSystem::SetGlobalShadowColor( uint32 color )
{
	if (0 == color)
		return;
	m_GlobalShadowColor = color;
	CColor4 color4(color);
	CRenderPipeline::GetInst()->SetFragmentShaderF(ACT_GLOBAL_SHADOW_COLOR,
		&color4, 1);
}
#endif

void CLightSystem::SetAmbient( uint32 Color,uint Index  )
{
	AstMsg(Index<2, "现在只支持两个不同主灯");
	m_GlobalAmbient[Index] = Color;
	m_bGlobalLightChange = true;
}

void CLightSystem::SetGlobalLightColor( uint32 Color,uint32 Index )
{
	AstMsg(Index<2, "现在只支持两个不同主灯");

	m_GlobalLightColor[Index] = Color;
	m_bGlobalLightChange = true;
}

uint32	CLightSystem::GetGlobalLightColor( uint Index )
{
	return m_GlobalLightColor[Index].MakeDWordSafe();
}

void CLightSystem::SetGlobalLightAttribute( const CVector3f& LightDir, uint32 Color )
{
	CColor4 lColor(Color);

	m_GlobalLightColor[0] = lColor;
	m_GlobalLight.SetColor( lColor );
	m_ActiveIndex = 0;

	m_GlobalLight.setDIRECTION(LightDir);
	m_ParentGroup->GetShadowFilter()->SetLightVector(LightDir);

	UpdateGlobalLight();
}

void CLightSystem::SetGlobalLightDir( const CVector3f& LightDir )
{
	m_GlobalLight.setDIRECTION(LightDir);
	m_ParentGroup->GetShadowFilter()->SetLightVector(LightDir);

	UpdateGlobalLight();
}

void CLightSystem::GetGlobalLightDir( CVector3f& LightDir, const CVector3f& Pos )
{
	if ( m_GlobalLight.GetType() == LIGHT_DIRECTIONAL )
		LightDir = m_GlobalLight.getDIRECTION();
	else
		LightDir = Pos - m_GlobalLight.GetPosition();
}

//uint32  CLightSystem::GetSpecular(uint Index)
//{
//	return m_GlobalSpecular[Index];
//}

//void CLightSystem::SetSpecular( uint32 Color,uint Index )
//{
//	AstMsg(Index<2, "现在只支持两个不同主灯");
//
//	m_GlobalSpecular[Index] = Color;
//	m_bGlobalLightChange = true;
//}

void CLightSystem::AddDynamicPointLight(const CVector3f& Pos,
                                        float Rang,
                                        uint32 Color,
                                        float Attn0,
                                        float Attn1,
                                        float Attn2)
{
	if (m_DynamicLights.size() >= MAX_DYNAMIC_LIGHT)
		return;
	CLight light;
	//ZeroMemory(&light, sizeof(light));

	light.SetType(LIGHT_POINT);
	light.SetPosition(Pos);
	light.SetRange(Rang);
	light.SetColor(Color);


	light.SetAttenuation( Attn0, Attn1, Attn2 );
	m_DynamicLights.push_back(light);
}

void CLightSystem::UpdateLights(const CVector3f& pos, bool updateDynamic/* = true*/)
{
	if (updateDynamic && m_bDynamicLightEnable)
	{
		struct NearerLight
		{
			CVector3f position;
			bool operator()(const CLight& light1, const CLight& light2)
			{
				CVector3f dis1 = light1.GetPosition() - position;
				CVector3f dis2 = light2.GetPosition() - position;

				return dis1.MagSqr() < dis1.MagSqr();
			}
		};
		CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
		int numLights = m_DynamicLights.size();
		if (numLights > MAX_LIGHT_NUM)
		{
			NearerLight nearerLight;
			nearerLight.position = pos;
			std::sort(m_DynamicLights.begin(), m_DynamicLights.end(), nearerLight);
		}
		for (int i = 0; i < MAX_LIGHT_NUM; ++i)
		{
			if (i < numLights)
			{
				renderPipeline->SetLight(i + 1, m_DynamicLights[i].GetLightStruct() );
				SetDynamicLight(i, m_DynamicLights[i]);
			}
			else
			{
				renderPipeline->SetLight(i + 1, NULL);
			}
		}
	}
	this->UpdateGlobalLight();

#ifndef AMBIENT_SHADOW_COLOR
	this->SetGlobalShadowColor(m_GlobalShadowColor);
#endif

	this->UpdateDynamicLights();
}

//------------------------------------------------------------------------------
void
CLightSystem::UpdateGlobalLight()
{
	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	renderPipeline->SetAmbient( m_GlobalAmbient[m_ActiveIndex] );

	if (m_bGlobalLightEnable)
	{
		renderPipeline->SetLight(0, m_GlobalLight.GetLightStruct());
		renderPipeline->SetVertexShaderF(ACT_GLOBAL_LIGHT_COLOR, &m_GlobalLight.GetColor(), 1);
		renderPipeline->SetVertexShaderF(ACT_GLOBAL_LIGHT_DIRECTION, &m_GlobalLight.getDIRECTION(), 1);
	}
	else
	{
		renderPipeline->SetLight(0, NULL);
	}
}

//------------------------------------------------------------------------------
void
CLightSystem::UpdateDynamicLights()
{
	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	renderPipeline->SetVertexShaderF(ACT_LIGHT_COLOR_ARRAY, m_DynamicLightColor, MAX_LIGHT_NUM);
	renderPipeline->SetVertexShaderF(ACT_LIGHT_POSITION_ARRAY, m_DynamicLightPosition, MAX_LIGHT_NUM);
	renderPipeline->SetVertexShaderF(ACT_LIGHT_DIRECTION_ARRAY, m_DynamicLightDir, MAX_LIGHT_NUM);
	renderPipeline->SetVertexShaderF(ACT_LIGHT_RANGE_INFO_ARRAY, m_DynamicLightRangeInfo, MAX_LIGHT_NUM);
	renderPipeline->SetVertexShaderF(ACT_LIGHT_SPOT_INFO_ARRAY, m_DynamicLightSpotInfo, MAX_LIGHT_NUM);

	// remove all the lights, add new ones in the next frame
	m_DynamicLights.clear();
}


//------------------------------------------------------------------------------
void
CLightSystem::SetLight( index_t lightIndex, CLight* light, bool isInfluenceTerrain /*= true*/ )
{
	if (0 == lightIndex)
	{
		Ast(LIGHT_DIRECTIONAL == light->GetType());
		m_GlobalLight = *light;
	}
	else if (lightIndex > MAX_LIGHT_NUM)
	{
		return;
	}
	else
	{
		static CLight invalidLight;
		m_InfluenceTerrain[lightIndex-1] = isInfluenceTerrain;
		if (NULL != light)
		{
			this->SetDynamicLight(lightIndex-1, *light);
			CRenderPipeline::GetInst()->SetLight(lightIndex, light->GetLightStruct());
		}
		else
		{
			this->SetDynamicLight(lightIndex-1, invalidLight);
			CRenderPipeline::GetInst()->SetLight(lightIndex, NULL);
		}
	}
}

//------------------------------------------------------------------------------
void
CLightSystem::SetDynamicLight( index_t lightIndex,CLight& light )
{
	Ast(lightIndex < MAX_LIGHT_NUM);
	LightStruct& tLight = *light.GetLightStruct();
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_LIGHT_COUNT, lightIndex + 1.0f);
	m_DynamicLightColor[lightIndex]		= light.GetColor();
	m_DynamicLightPosition[lightIndex].x	= tLight.Position.x;
	m_DynamicLightPosition[lightIndex].y	= tLight.Position.y;
	m_DynamicLightPosition[lightIndex].z	= tLight.Position.z;

	m_DynamicLightDir[lightIndex].x		= tLight.Direction.x;
	m_DynamicLightDir[lightIndex].y		= tLight.Direction.y;
	m_DynamicLightDir[lightIndex].z		= tLight.Direction.z;
	
	m_DynamicLightRangeInfo[lightIndex].range = tLight.Range;
	m_DynamicLightRangeInfo[lightIndex].attenuation0 = tLight.Attenuation0;
	m_DynamicLightRangeInfo[lightIndex].attenuation1 = tLight.Attenuation1;
	m_DynamicLightRangeInfo[lightIndex].attenuation2 = tLight.Attenuation2;
	m_DynamicLightSpotInfo[lightIndex].lightType = (tLight.Type == LIGHT_POINT) ? 1.0f : 2.0f;
	if (!m_InfluenceTerrain[lightIndex])
		m_DynamicLightSpotInfo[lightIndex].lightType = -m_DynamicLightSpotInfo[lightIndex].lightType;
	m_DynamicLightSpotInfo[lightIndex].fallOff = tLight.Falloff;
	m_DynamicLightSpotInfo[lightIndex].theta = tLight.Theta;
	m_DynamicLightSpotInfo[lightIndex].phi = tLight.Phi;
}
