#include "stdafx.h"
#include "CRenderMatrix.h"
#include "BaseHelper.h"
#include "CSkeletalFrame.h"
#include "CAnimationController.h"
#include "TSqrAllocator.inl"
#include "IModelHandler.h"

uint32 CRenderMatrix::GetSkeletalCount()
{
	return m_SkeletalMask.GetSkeletalCount();
}

CRenderMatrix::CRenderMatrix()
//: mResBoenMatrixlist(maxBoneNum,NULL)
: m_pMatrixUnit(1,new CMatrixUnit)
, m_pMatrixUnitOrg(1,new CMatrixUnit)
, m_pSkeletalSepList(1,NULL)
, m_pAniCtrlList(1,NULL)
, m_pBoneMatrixlist(0)
{
	(*m_pMatrixUnit[0])		= CMatrix::UNIT_MATRIX;
	(*m_pMatrixUnitOrg[0])	= CMatrix::UNIT_MATRIX;

	m_CurFrame			= 0;
	m_MaxUseSkeletalID	= 0;
	m_IsBlendStart		= 0;
	m_IsAniBlend		= false;
	m_IsRended			= false;
	m_nStopAni			= false;
	m_CurAnimate		= 0;
	m_CurFrame			= 0;
	m_CurrentTime		= 0xFFFFFFFF;
	m_BlendFrame		= 0;
	m_AniEndFrame		= 0;
	m_AniStartFrame		= 0;
	m_Weight			= 0.0f;
	m_pHandler			= NULL;
}

CRenderMatrix::~CRenderMatrix()
{
	Release();
	if(m_pHandler)
	{
		AdvDelRef(m_pHandler);
		m_pHandler = NULL;
	}
}

void CRenderMatrix::ClearAniSeparation(void)
{
	for ( uint32 i = 0; i <= m_MaxUseSkeletalID; i++ )
		GfkSafeDelete( m_pSkeletalSepList[i] );
}

void CRenderMatrix::ClearAniController(void)
{
	for ( uint32 i = 0; i <= m_MaxUseSkeletalID; i++ )
		SafeRelease( m_pAniCtrlList[i] );
}

void CRenderMatrix::Release()
{
	ClearAniSeparation();

	for ( uint32 i = 0; i <= m_MaxUseSkeletalID; i++ )
	{
		GfkSafeDelete( m_pMatrixUnit[i] );
		GfkSafeDelete( m_pMatrixUnitOrg[i] );
		SafeRelease( m_pAniCtrlList[i] );
	}

	m_SkeletalMask.Empty();
	m_pMatrixUnitOrg.clear();
	m_pMatrixUnit.clear();
	m_pSkeletalSepList.clear();
	m_pAniCtrlList.clear();
	m_pBoneMatrixlist.clear();
}

////////////////////////////////////////////////////////////////////////
// 功能：构造和析构
// 参数：略
// 返回：无
//////////////////////////////////////////////////////////////////////////
void CRenderMatrix::Initialization( CSkeletalFrame* pSkeFrm )
{
	if( NULL==pSkeFrm )
		return;

	ClearAniSeparation();

	pSkeFrm->SetSkeletalMask( m_SkeletalMask );
	
	if ( pSkeFrm->GetSkeletalNum() != m_pBoneMatrixlist.size() )
		m_pBoneMatrixlist.resize(pSkeFrm->GetSkeletalNum());
	
	for ( uint32 i = 0; i <= min( m_MaxUseSkeletalID, pSkeFrm->GetSkeletalNum() - 1 ); i++ )
	{
		if ( m_SkeletalMask.HaveSkeletal( i ) )
		{
			if ( m_pMatrixUnit[i] == NULL )
			{
				m_pMatrixUnit[i] = new CMatrixUnit;
				m_pMatrixUnitOrg[i] = new CMatrixUnit;
			}
		}
		else if ( i )
		{
			GfkSafeDelete( m_pMatrixUnit[i] );
			GfkSafeDelete( m_pMatrixUnitOrg[i] );
		}
	}

	(*m_pMatrixUnit[0]) = CMatrix::UNIT_MATRIX;
	(*m_pMatrixUnitOrg[0]) = CMatrix::UNIT_MATRIX;

	m_IsBlendStart		= true;
	m_IsAniBlend		= false;
	m_IsRended			= false;
	m_CurAnimate		= 0;
	m_AniStartFrame		= 0;
	m_BlendFrame		= 0;
	m_CurFrame			= 0;
	m_CurrentTime		= 0xFFFFFFFF;
	m_Result			= AniResult();
}

void CRenderMatrix::SetSkeletalMask( const SKELETAL_MASK& Mask )
{
	m_SkeletalMask = Mask;

	int nSkeletal = 0;
	for ( uint32 i = 0; i < 256; i++ )
		if ( Mask.HaveSkeletal(i) )
			nSkeletal = i;

	if( nSkeletal != m_MaxUseSkeletalID )
	{
		for ( uint32 i = nSkeletal + 1; i <= m_MaxUseSkeletalID; ++i )
		{
			GfkSafeDelete( m_pMatrixUnit[i] );
			GfkSafeDelete( m_pMatrixUnitOrg[i] );
			GfkSafeDelete( m_pSkeletalSepList[i] );
			m_pAniCtrlList[i] = NULL;
		}

		m_pMatrixUnit.resize(nSkeletal + 1,NULL);
		m_pMatrixUnitOrg.resize(nSkeletal + 1,NULL);
		m_pSkeletalSepList.resize(nSkeletal + 1,NULL);
		m_pAniCtrlList.resize(nSkeletal + 1,NULL);

		m_MaxUseSkeletalID = (BYTE)nSkeletal;
	}

	for ( uint32 i = 0; i <= m_MaxUseSkeletalID; ++i )
	{
		if ( Mask.HaveSkeletal( i ) || i == 0 )
		{
			if ( m_pMatrixUnit[i] == NULL )
			{
				m_pMatrixUnit[i]		= new CMatrixUnit;
				m_pMatrixUnitOrg[i]		= new CMatrixUnit;
			}
		}
		else
		{
			GfkSafeDelete(m_pMatrixUnit[i]);
			GfkSafeDelete(m_pMatrixUnitOrg[i]);
		}
	}

	(*m_pMatrixUnit[0]) = CMatrix::UNIT_MATRIX;
	(*m_pMatrixUnitOrg[0]) = CMatrix::UNIT_MATRIX;
}

void CRenderMatrix::ClearSkeletaInfo()
{
	m_pBoneMatrixlist.clear();
}

void CAnimateSeparation::Update( float DeltaFrame, CRenderMatrix & RenderMatrix )
{
	if( m_nStopAni )
		return;

	m_CurFrame += (DeltaFrame * m_AniSpeed);

	if( APS_FADEIN == m_State )
	{
		m_Weight = 0.0f;
		if ( m_BlendFrame < m_CurFrame )
		{
			m_State		= APS_NORMAL;
			m_CurFrame -= m_BlendFrame;
		}
		else
			m_Weight = m_BlendFrame!=0.0f ? ( m_BlendFrame - m_CurFrame )/m_BlendFrame : 0.0f;
	}

	if( APS_NORMAL == m_State )
	{
		m_Weight = RenderMatrix.m_Weight;
		if( m_AniEndFrame <= m_CurFrame )
		{
			switch ( m_PlayMode )
			{
			case APM_LOOP_PLAY:
				{
					m_CurFrame    = 0;
					break;
				}
			case APM_PLAY_ONCE:
				{
					m_CurFrame = 0;
					m_State	   = APS_FADEOUT;
					break;
				}
			case APM_PLAY_BACK:
				{
					m_CurFrame -= DeltaFrame;
					break;
				}
			case APM_KEEP_PLAY:
				{
					m_CurFrame = m_AniEndFrame;
					break;
				}
			}
		}
	}

	if( APS_FADEOUT == m_State )
	{
		m_CurAnimate = RenderMatrix.m_CurAnimate;
		if ( m_BlendFrame < m_CurFrame )
		{
			if(RenderMatrix.m_pHandler)
			{
				SQR_TRY
				{
					RenderMatrix.m_pHandler->OnSepAnimationEnd( (const char*)m_AnimationName );
				}
				SQR_CATCH(exp)
				{
					exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":OnSepAnimationEnd");
					LogExp(exp);
				}
				SQR_TRY_END;
			}
			GfkSafeDelete(RenderMatrix.m_pSkeletalSepList[m_SkeletalID]);
			return;
		}
		else
		{
			m_Weight = m_BlendFrame != 0.0f ? ( m_BlendFrame - m_CurFrame ) / m_BlendFrame:1.0f;
		}
	}
}

void CRenderMatrix::CalculateMatrix(void)
{
	if ( m_IsAniBlend )
	{
		m_Weight = m_IsRended ? ( m_BlendFrame - m_CurFrame )/ m_BlendFrame  : 0.0f;

		if ( m_CurFrame >= m_BlendFrame )
		{
			m_Weight = 0.0f;
			m_IsAniBlend = false;
			m_CurFrame = FLOAT(m_AniStartFrame);
		}
		else
		{
			m_Result.m_CurFrame = 0.0f;
			m_Result.m_IsAniEnd = false;
			m_Result.m_IsReset	= false;
		}
	}
}
