//------------------------------------------------------------------------------
//  CEnvMapWaterEffect.cpp
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CEnvMapWaterEffect.h"
#include "CGraphic.h"
#include "CGpuProgram.h"
#include "CGpuProgramParameters.h"
#include "CWaterMgr.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CRenderPipeline.h"
namespace sqr
{

//------------------------------------------------------------------------------
CEnvMapWaterEffect::CEnvMapWaterEffect()
{
	m_pEnvironmentMap	= NULL;
	m_pWaveNormalMap[0] = NULL;
	m_pWaveNormalMap[1] = NULL;

	SetEffectID(CWaterMgr::EffectTypeToID(CWaterMgr::EnvironmentMapping));
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(), "tile/scenewater/sky.dds",&m_pEnvironmentMap);
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(), "tile/scenewater/wave0.dds",&m_pWaveNormalMap[0]);
	CGraphic::GetInst()->CreateTexture(PATH_ALIAS_RES.c_str(), "tile/scenewater/wave1.dds",&m_pWaveNormalMap[1]);

	m_WaveSpeed[0] = CVector2f(0.0225f, 0.015f);
	m_WaveSpeed[1] = CVector2f(-0.0125f, 0.02f);
	m_WaveOffset = CVector4f(0,0,0,0);
}

//------------------------------------------------------------------------------
CEnvMapWaterEffect::~CEnvMapWaterEffect()
{
	SafeRelease(m_pEnvironmentMap);
	SafeRelease(m_pWaveNormalMap[0]);
	SafeRelease(m_pWaveNormalMap[1]);
}

//------------------------------------------------------------------------------
void
CEnvMapWaterEffect::OnFrameUpdate()
{
	CWaterEffect::OnFrameUpdate();

	float deltaTime = GetTimeElapsed() * 0.001f;
	// update texture coordinate offsets
	m_WaveOffset.x += m_WaveSpeed[0].x * deltaTime;
	m_WaveOffset.y += m_WaveSpeed[0].y * deltaTime;
	m_WaveOffset.z += m_WaveSpeed[1].x * deltaTime;
	m_WaveOffset.w += m_WaveSpeed[1].y * deltaTime;
	// repeate in range [0,1] or [-1,0]
	if (m_WaveOffset.x >= 1.0f || m_WaveOffset.x <= -1.0f)
		m_WaveOffset.x = 0.0f;
	if (m_WaveOffset.y >= 1.0f || m_WaveOffset.y <= -1.0f)
		m_WaveOffset.y = 0.0f;
	if (m_WaveOffset.z >= 1.0f || m_WaveOffset.z <= -1.0f)
		m_WaveOffset.z = 0.0f;
	if (m_WaveOffset.w >= 1.0f || m_WaveOffset.w <= -1.0f)
		m_WaveOffset.w = 0.0f;
}

//------------------------------------------------------------------------------
void
CEnvMapWaterEffect::OnRenderBegin()
{	
	CRenderPipeline::GetInst()->SetShader(effectId);
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST, &m_WaveOffset, 1);
	CRenderPipeline::GetInst()->_SetTexture( 0, m_pEnvironmentMap );
	CRenderPipeline::GetInst()->_SetTexture( 1, m_pWaveNormalMap[0] );
	CRenderPipeline::GetInst()->_SetTexture( 2, m_pWaveNormalMap[1] );
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ALPHABLENDENABLE, TRUE);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ALPHATESTENABLE, FALSE);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_SRCBLEND, BLEND_INVSRCALPHA);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_DESTBLEND, BLEND_SRCALPHA);
	CRenderPipeline::GetInst()->_SetRenderStyle(RS_ZWRITEENABLE, FALSE);
	CWaterEffect::OnRenderBegin();
}

void CEnvMapWaterEffect::OnRenderEnd()
{
}

}// namespace sqr
