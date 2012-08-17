#include "stdafx.h"
#include "CCombination.h"
#include "IEffectGroup.h"
#include "IEffect.h"
#include "IEffectManager.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"
#include "CCamera.h"
#include "BaseHelper.h"
#include "TraceHelper.h"
DYNAMIC_CREATE_EFFECTUNITBASE( eEfxCombination, IDS_COMBINATION, CCombination, CCombinationProp )

enum
{
	COMBINATION_ORIGIN,
};	

CCombinationProp::CCombinationProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_nEffectIndex = 0;
	m_pEffectGroup = NULL;
	LoadFullRes();
}

CCombinationProp::~CCombinationProp(void)
{
	SafeRelease( m_pEffectGroup );
}

int32 CCombinationProp::LoadBin( IEffectRead& File )
{
	uint32 Version = 0;
	File.read( (char*)&Version,	sizeof(uint32) );

	LoadStringT( File, m_strEffectFileName );
	File.read( (char*)&m_nEffectIndex,	sizeof(m_nEffectIndex) );

	if ( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	return 0;
}

bool CCombinationProp::IsValid()
{
	return m_pEffectGroup != NULL && m_pEffectGroup->IsValid();
}

int32 CCombinationProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = COMBINATION_ORIGIN;
	File.write( &Version, sizeof(uint32) );

	SaveStringT(File, m_strEffectFileName.c_str());
	File.write( &m_nEffectIndex, sizeof(m_nEffectIndex) );

	if ( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;

	return 0;
}

void CCombinationProp::_UpdateParams(void)
{
	if( this->m_strEffectFileName.empty() )
		return ;

	if (FAILED(IEffectManager::GetInst()->CreateEffectGroup( this->m_strEffectFileName.c_str(), &m_pEffectGroup )))
	{
		ostringstream strm;
		strm<<"CreateEffectGroup failed with file \""<<this->m_strEffectFileName<<"\"!";
		ArtErr(strm.str());
	}
}

////////////////////////////////////////////////////////////////////////

CCombination::CCombination( IEffectUnitProp* pEffectUnitProp )
		  : CEffectUnit( pEffectUnitProp )
{
	m_pEffectRenderObj = NULL;
}

CCombination::~CCombination(void)
{
	/* if( m_pEffectRenderObj )
		 m_pEffectRenderObj->ClearLink();*/

	 SafeRelease(m_pEffectRenderObj);
}

void CCombination::SetStartTime( uint32 STime,uint32 DTime )
{
CEffectUnit::SetStartTime( STime,DTime );
}

void CCombination::ResetTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::ResetTime( STime,DTime );
}

bool CCombination::SetProp( IEffectUnitProp* pEffectUnitProp )	
{ 
	if( !CEffectUnit::SetProp( pEffectUnitProp ) )
		return false;

	CCombinationProp* pNewProp = NULL;
	pNewProp = (CCombinationProp*)m_pEffectUnitProp;
	if( pNewProp == NULL )
		return false;

	SQR_TRY
	{
		if( pNewProp->IsValid() && pNewProp->m_nEffectIndex > 0 )
		{
			EString effectname = pNewProp->m_pEffectGroup->GetEffectName(pNewProp->m_nEffectIndex - 1);
			if ( !effectname.empty() && m_strLastName != effectname )
			{
				IEffect* pEffect			= NULL;
				if( FAILED( IEffectManager::GetInst()->CreateEffect( pNewProp->m_pEffectGroup, effectname.c_str(), &pEffect ) ) )
				{
					ostringstream strm;
					strm<<"CreateEffect failed with file \""<<effectname<<"\"!";
					ArtErr(strm.str());
				}

				DebugOut("CreateEffect %s, %lx\n", effectname.c_str(), pEffect);
				m_pEffectRenderObj = pEffect;

				m_strLastName = effectname;
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CreateFx");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;

	return true;
}

void CCombination::OnUnlinked(void)
{
	m_pParent = NULL;
}

void CCombination::SetVisible( bool isVisible )
{
	CEffectUnit::SetVisible(isVisible);
}

const EFFECT_RESULT CCombination::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CCombinationProp* pProp = (CCombinationProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );
	if( !IsInRenderTime() || m_DeltaTime == 0.0f )
	{
		return Result;
	}

	float fPercent = GetUnitPercent();
	CVector3f OrgOffset( 0, 0, 0 );
	ITexture* pCurText = NULL;
	uint32 Color=0;
	float fScale = 1.0f;
	CVector3f XYZcale(0,0,0);
	pProp->GetCurAdvanceProp( fPercent, OrgOffset, pCurText, Color, fScale, XYZcale);

	SQR_TRY
	{
		if( m_pEffectRenderObj  )
		{
			CColor matColor = CColor( 0xff, 0x80, 0x80, 0x80 );
			sMaterial Mat = 
			{
				CColor4((uint32)matColor ), 
				CColor4((uint32)matColor ), 
				CColor4((uint32)0),
				CColor4((uint32)0xffffffff),
				0
			};

			RenderObjStyle objStyle = 
			{
				ROS_LIGHTMAT1,
				true,
				true,
				DELTARS_DIFFUSE|DELTARS_AMBIENT,
				(uint32)&Mat
			};

			m_matWorld = matWorld;
			OrgOffset.FastMultiply( m_matWorld );
			m_matWorld.m[3][0] = OrgOffset.x;
			m_matWorld.m[3][1] = OrgOffset.y;
			m_matWorld.m[3][2] = OrgOffset.z;
			m_matWorld.m[0][0] = XYZcale.x;
			m_matWorld.m[1][1] = XYZcale.y;
			m_matWorld.m[2][2] = XYZcale.z;
			
			m_pEffectRenderObj->UpdateTime((uint32)(m_StartTime + m_FrameTime) );
			m_pEffectRenderObj->Render( m_matWorld, &objStyle );
		}
	}
	SQR_CATCH(exp) 
	{
		exp.AppendType("AmbFxRender");
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;

	return Result;
}
