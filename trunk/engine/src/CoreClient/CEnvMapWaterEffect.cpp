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
namespace sqr
{

//------------------------------------------------------------------------------
CEnvMapWaterEffect::CEnvMapWaterEffect()
{
	SetEffectID(CWaterMgr::EffectTypeToID(CWaterMgr::EnvironmentMapping));
	CGraphic::GetInst()->CreateTexture("tile/scenewater/sky.dds",&m_pEnvironmentMap);
	CGraphic::GetInst()->CreateTexture("tile/scenewater/wave0.dds",&m_pWaveNormalMap[0]);
	CGraphic::GetInst()->CreateTexture("tile/scenewater/wave1.dds",&m_pWaveNormalMap[1]);

	m_WaveSpeed[0] = CVector2f(0.0225f, 0.015f);
	m_WaveSpeed[1] = CVector2f(-0.0125f, 0.02f);
	m_WaveOffset[0] = m_WaveOffset[1] = CVector2f(0,0);
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
	m_WaveOffset[0] += m_WaveSpeed[0] * deltaTime;
	m_WaveOffset[1] += m_WaveSpeed[1] * deltaTime;
	// repeate in range [0,1] or [-1,0]
	if (m_WaveOffset[0].x >= 1.0f || m_WaveOffset[0].x <= -1.0f)
		m_WaveOffset[0].x = 0.0f;
	if (m_WaveOffset[1].x >= 1.0f || m_WaveOffset[1].x <= -1.0f)
		m_WaveOffset[1].x = 0.0f;
	if (m_WaveOffset[0].y >= 1.0f || m_WaveOffset[0].y <= -1.0f)
		m_WaveOffset[0].y = 0.0f;
	if (m_WaveOffset[1].y >= 1.0f || m_WaveOffset[1].y <= -1.0f)
		m_WaveOffset[1].y = 0.0f;
}

//------------------------------------------------------------------------------
void
CEnvMapWaterEffect::OnRenderBegin()
{
	effectParams->setValue("waveOffset0", &m_WaveOffset[0], sizeof(CVector2f));
	effectParams->setValue("waveOffset1", &m_WaveOffset[1], sizeof(CVector2f));
	CRenderPipeline::GetInst()->_SetTexture( 0, m_pEnvironmentMap );
	CRenderPipeline::GetInst()->_SetTexture( 1, m_pWaveNormalMap[0] );
	CRenderPipeline::GetInst()->_SetTexture( 2, m_pWaveNormalMap[1] );
	CWaterEffect::OnRenderBegin();
}

//------------------------------------------------------------------------------
/**
	其实我也不想写这里, 谁叫我们基础库不完善来着.

	add by EndDuke:那就完善呗，Texture那个地方是需要完善的，必须地！哈哈
 */
//IDirect3DTexture9*
//CEnvMapWaterEffect::CreateTexture( const char* filename )
//{
//	IDirect3DTexture9* texture = NULL;
//	CPkgFile pkgFile(filename);
//	vector<byte> buffer(pkgFile.Size());
//	pkgFile.Read(&buffer.front(), buffer.size());
//	V(D3DXCreateTextureFromFileInMemory(GetD3DDevice(),
//	                                    &buffer.front(), buffer.size(), &texture));
//	return texture;
//}
//
////------------------------------------------------------------------------------
//IDirect3DCubeTexture9*
//CEnvMapWaterEffect::CreateCubeTexture( const char* filename )
//{
//	IDirect3DCubeTexture9* texture = NULL;
//	CPkgFile pkgFile(filename);
//	vector<byte> buffer(pkgFile.Size());
//	pkgFile.Read(&buffer.front(), buffer.size());
//	V(D3DXCreateCubeTextureFromFileInMemory(GetD3DDevice(),
//	                                        &buffer.front(), buffer.size(), &texture));
//	return texture;
//}
}// namespace sqr
