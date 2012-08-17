//------------------------------------------------------------------------------
//  CBloomTarget.cpp
//  为了兼容老代码, 这个类现在只管理参数.
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "CBloomTarget.h"
#include "CPostProcessManager.h"
#include "BaseHelper.h"
#include "CGpuProgramParameters.h"
#include "CGpuProgramMgr.h"
#include "CRenderTarget.h"
#include "CRenderPipeline.h"

namespace sqr
{
CBloomTarget::CBloomTarget(CRenderTarget* pRenderTarget)
: m_pRenderTarget(pRenderTarget)
{
	m_IsInit = false;

	//设置初始值 - 直接关系到效果 - 这个很重要
	m_fBrightPass		= 0.000f;	//光通量（0~1）
	m_fGaussMultiplier	= 0.35f;	//泛光强度(0~2)
	m_fGaussMean		= 0.292f;	//泛光偏移(-1~+1)
	m_fGaussStdDev      = 0.363f;	//融合度 (0~1)
	m_fExposure			= 1.00f;	//最终亮度0~2
	m_fSample           = 0.0f;
}

CBloomTarget::~CBloomTarget()
{
	if (CPostProcessManager::HasInst())
		CPostProcessManager::Destroy();
	Release();
}

bool CBloomTarget::Create(void)
{
	if (CGpuProgramMgr::GetInst()->isLoaded())
		CPostProcessManager::GetInst()->Setup();
	return true;
}

bool CBloomTarget::Release(void)
{
	if (CPostProcessManager::HasInst())
		CPostProcessManager::GetInst()->Discard();
	m_IsInit = false;
	return true;
}

bool CBloomTarget::IsUseful()
{
	return m_IsInit;
}

//如果参数产生了变化 则在渲染前应当更新-（运行此函数）
//出于性能考虑，没有使其在渲染流程中，应当手动调用
void CBloomTarget::InitParams(void)
{
	if (m_IsInit)
		return;

	CIRect viewport;
	m_pRenderTarget->GetRect(viewport);

	float fInvWidthA = 1.0f / ( viewport.Width() * 0.25f );
	float fInvHeightA = 1.0f / ( viewport.Height() * 0.25f );

	for ( int i = 0; i < 9; i++ )
	{
		m_HBloomOffsets[i] = (float( i ) - 4.0f) * fInvWidthA;
		float x = (float( i ) - 4.0f) / 4.0f;
		m_HBloomWeights[i] = m_fGaussMultiplier * ComputeGaussianValue( x, m_fGaussMean, m_fGaussStdDev );
	}

	for ( int i = 0; i < 9; i++ )
	{
		m_VBloomOffsets[i] = (float( i ) - 4.0f) * fInvHeightA;
		float x = (float( i ) - 4.0f) / 4.0f;
		m_VBloomWeights[i] = m_fGaussMultiplier * ComputeGaussianValue( x, m_fGaussMean, m_fGaussStdDev );
	}
	
	m_IsInit = true;
}

////------------------------------------------------------------------------------
//bool CBloomTarget::BeginTarget( CRenderGroup* pRenderGroup )
//{
//	
//
//	//manager->Begin(pRenderGroup);
//	return true;
//}

//------------------------------------------------------------------------------
bool CBloomTarget::DoPostProcess( void )
{
	if (CPostProcessManager::GetInst()->IsEffectEnabled("Bloom"))
	{
		InitParams();
		CRenderPipeline* pPipeline = CRenderPipeline::GetInst();
		pPipeline->SetFragmentShaderF(PPSP_BRIGHT_PASS_THRESHOLD_PS, &m_fBrightPass, 1);
		pPipeline->SetFragmentShaderF(PPSP_HBLOOM_OFFSETS_PS, m_HBloomOffsets, 3);
		pPipeline->SetFragmentShaderF(PPSP_HBLOOM_WEIGHTS_PS, m_HBloomWeights, 3);
		pPipeline->SetFragmentShaderF(PPSP_VBLOOM_OFFSETS_PS, m_VBloomOffsets, 3);
		pPipeline->SetFragmentShaderF(PPSP_VBLOOM_WEIGHTS_PS, m_VBloomWeights, 3);
		pPipeline->SetFragmentShaderF(PPSP_SCALER_PS, &m_fSample, 1);
		pPipeline->SetFragmentShaderF(PPSP_EXPOSURE_PS, &m_fExposure, 1);
	}

	CPostProcessManager::GetInst()->DoPostProcess();
	return true;
}

//------------------------------------------------------------------------------
bool CBloomTarget::IsTurnOn( const string& effect )
{
	return CPostProcessManager::GetInst()->IsEffectEnabled(effect);
}

//------------------------------------------------------------------------------
void CBloomTarget::SetTurnOn( const string& effect, bool b )
{
	CPostProcessManager::GetInst()->SetEffectEnabled(effect, b);
}
}