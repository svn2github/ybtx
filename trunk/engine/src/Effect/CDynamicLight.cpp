#include "stdafx.h"
#include ".\cdynamiclight.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CColor.inl"
DYNAMIC_CREATE_EFFECTUNITBASE( eDynamicLight, IDS_DYNAMICLIGHT, CDynamicLight, CDynamicLightProp )

CDynamicLightProp::CDynamicLightProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc  )
{
	m_fRange			= 500.0f;
	m_fMinStrong		= 1.0f;
	m_fMaxStrong		= 100.0f;

	m_vecOffset[0.0f]	= CVector3f( 0, 10, 0 );
}

CDynamicLightProp::~CDynamicLightProp(void)
{
}

int32 CDynamicLightProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	File.read( (char*)&m_fRange,				sizeof(float) );
	File.read( (char*)&m_fMinStrong,			sizeof(float) );
	File.read( (char*)&m_fMaxStrong,			sizeof(float) );
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CDynamicLightProp::_UpdateParams(void)
{
	m_fMaxStrong = max( m_fMaxStrong, m_fMinStrong );
}

int32 CDynamicLightProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = 0;
	File.write( (char*)&Version,				sizeof(uint32) );

	File.write( (char*)&m_fRange,				sizeof(float) );
	File.write( (char*)&m_fMinStrong,			sizeof(float) );
	File.write( (char*)&m_fMaxStrong,			sizeof(float) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CDynamicLight::CDynamicLight( IEffectUnitProp* pEffectUnitProp )
			  : CEffectUnit( pEffectUnitProp )
{
	const CDynamicLightProp* pProp = (CDynamicLightProp*)m_pEffectUnitProp;
	//m_vecScale += CVector3f( pProp->m_fRange*2, pProp->m_fRange*2, pProp->m_fRange*2 );
}

CDynamicLight::~CDynamicLight(void)
{
}


const EFFECT_RESULT CDynamicLight::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CDynamicLightProp* pProp = (CDynamicLightProp*)m_pEffectUnitProp;
	EFFECT_RESULT Result( NULL, true, false );
	if( !IsInRenderTime() )
	{
		return Result;
	}
		
	
	CLightSystem* pLightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();

	float Percent = GetUnitPercent();
	CVector3f OrgOffset = GetMapValue( pProp->m_vecOffset, Percent );
	OrgOffset.FastMultiply( matWorld );
	m_matWorld._41 = OrgOffset.x;
	m_matWorld._42 = OrgOffset.y;
	m_matWorld._43 = OrgOffset.z;

	m_matWorld = matWorld;

	uint32 Color = GetMapValue( pProp->m_Color, Percent );
	float Scale = pProp->m_fMinStrong + ( pProp->m_fMaxStrong - pProp->m_fMinStrong )*GetMapValue( pProp->m_Scale, Percent )/255.0f;

	pLightSystem->AddDynamicPointLight( OrgOffset, pProp->m_fRange, Color, 0, 1.0f/Scale, 0 );
	return Result;
}