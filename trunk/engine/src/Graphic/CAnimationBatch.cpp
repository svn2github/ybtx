#include "stdafx.h"
#include "CAnimationBatch.h"
#include "CMatrixUnit.h"
#include "CAnimation.h"
#include "CGpuProgramMgr.h"

CAnimationBatch::CAnimationBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CGeometryBatch(fFlag,fHandle,pParams,pParent)
, m_pBatchAnimate((CAnimate*)fHandle)
, m_pSkeletalFrame(NULL)
, m_CurTime(0)
{
	if(m_pBatchAnimate)
	{
		m_RenderMatrix.m_CurAnimate = m_pBatchAnimate->GetRootGroup()->GetAniIDbyName(m_pBatchAnimate->GetName());
		m_RenderMatrix.m_AniEndFrame  = (uint16)m_pBatchAnimate->GetFrameNum();
		m_pSkeletalFrame = m_pBatchAnimate->GetRootGroup()->GetSkeletalFrame();
		m_RenderMatrix.Initialization( m_pSkeletalFrame );
	}
}

CAnimationBatch::~CAnimationBatch()
{

}

void CAnimationBatch::_UpdateBatch(void)
{
	if(m_pBatchAnimate&&m_pSkeletalFrame&&
		m_pGeometryParams&&m_pGeometryParams->m_CurTime != m_CurTime)
	{
		float FrameDelta;
		if( m_CurTime == 0 )
			FrameDelta = 0.0f;
		else
			FrameDelta = (m_pGeometryParams->m_CurTime - m_CurTime)/ m_pBatchAnimate->GetFrameRatio();

		m_RenderMatrix.m_Result.m_FrameString	= NULL;
		m_RenderMatrix.m_Result.m_IsAniEnd		= true;
		m_RenderMatrix.m_Result.m_IsReset		= true;



		if ( m_RenderMatrix.m_AniEndFrame - m_RenderMatrix.m_CurFrame < 0.001f )
		{
			m_RenderMatrix.m_CurFrame = m_RenderMatrix.m_AniEndFrame;
			m_RenderMatrix.m_Result.m_IsAniEnd		= true;
			m_RenderMatrix.m_Result.m_IsReset		= true;
		}
		else
		{
			if( !m_RenderMatrix.m_nStopAni )
				m_RenderMatrix.m_CurFrame += FrameDelta;
		}

		m_RenderMatrix.m_Result.m_PreAnimationName = m_pBatchAnimate->GetName();
		
		if ( m_RenderMatrix.m_IsAniBlend && !m_RenderMatrix.m_IsBlendStart )
		{
			m_RenderMatrix.m_CurFrame = FrameDelta;
			m_RenderMatrix.m_IsBlendStart = TRUE;
		}

		m_RenderMatrix.CalculateMatrix();
		m_pSkeletalFrame->GetRootSkeletal().CalculateMatrix(CMatrixUnit::CMatrixUnitZero, m_RenderMatrix, *m_pBatchAnimate->GetRootGroup(), *m_pSkeletalFrame );
		
		if ( m_RenderMatrix.m_CurFrame < m_RenderMatrix.m_AniEndFrame )
		{
			m_RenderMatrix.m_Result.m_IsAniEnd = false;
			m_RenderMatrix.m_Result.m_IsReset  = false;
		}
		
		if ( m_RenderMatrix.m_Result.m_IsAniEnd  )
		{
			m_RenderMatrix.m_CurFrame = (float)( (int32)( m_RenderMatrix.m_CurFrame - m_RenderMatrix.m_AniEndFrame )%m_pBatchAnimate->GetFrameNum());
			m_RenderMatrix.m_IsAniBlend			= false;
			m_RenderMatrix.m_Result.m_IsAniEnd	= false;
			m_RenderMatrix.m_Result.m_IsReset	= true;
		}

		m_CurTime = m_pGeometryParams->m_CurTime;
		m_RenderMatrix.m_CurrentTime = m_CurTime;
	}
}

void CAnimationBatch::_UpdateInst( CGeometryInst* pGInst )
{
	m_RenderMatrix.SetSkeletalMask(pGInst->ANI_PARAMS->SKE_MASK);
	pGInst->ANI_PARAMS->pAniInst = &m_RenderMatrix;
}

CAnimate* CAnimationBatch::GetAnimate(void)
{
	return m_pBatchAnimate;
}

float CAnimationBatch::GetCurFrame(void)
{
	return m_RenderMatrix.m_CurFrame;
}

CRenderMatrix&	CAnimationBatch::GetRenderMatrix(void)
{
	return m_RenderMatrix;
}

void	CAnimationBatch::ModifyRS(STATIC_RS& RS)
{
	if( m_pBatchAnimate && m_pSkeletalFrame && CGpuProgramMgr::GetInst()->GetShaderVer() >= Shader_V_2 )
	{
		RS.m_ShaderType |= (MakeSysVS_ID(SSI_SKELETAL_ANI) + ((RS.m_UVSrc_S0 + RS.m_UVSrc_S1*2 + RS.m_UVSrc_S2*4 + RS.m_UVSrc_S3*8)>>16));
	}
}