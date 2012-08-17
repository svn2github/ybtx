#include "stdafx.h"
#include "CCameraShake.h"
#include "CGraphic.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "CCamera.h"
DYNAMIC_CREATE_EFFECTUNITBASE( eCameraShake, IDS_CAMERASHAKE, CCameraShake, CCameraShakeProp )

CCameraShakeProp::CCameraShakeProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_fFrequence		= 10;
	m_fStrength			= 2.0f;
	m_fMinRadius		= 200;
	m_fMaxRadius		= 400;
}

CCameraShakeProp::~CCameraShakeProp(void)
{
}

int32 CCameraShakeProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	File.read( (char*)&m_fFrequence,			sizeof(float) );
	File.read( (char*)&m_fStrength,				sizeof(float) );
	File.read( (char*)&m_fMinRadius,			sizeof(float) );
	File.read( (char*)&m_fMaxRadius,			sizeof(float) );
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CCameraShakeProp::_UpdateParams(void)
{
	m_fMaxRadius = max( m_fMaxRadius, m_fMinRadius + 0.001f );
}

int32 CCameraShakeProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = 0;
	File.write( (char*)&Version,				sizeof(uint32) );

	File.write( (char*)&m_fFrequence,			sizeof(float) );
	File.write( (char*)&m_fStrength,			sizeof(float) );
	File.write( (char*)&m_fMinRadius,			sizeof(float) );
	File.write( (char*)&m_fMaxRadius,			sizeof(float) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

//////////////////////////////////////////////////////////////////////////


CCameraShake::CCameraShake( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	const CCameraShakeProp* pProp = (CCameraShakeProp*)m_pEffectUnitProp;
	//m_vecScale += CVector3f( pProp->m_fMaxRadius*2, pProp->m_fMaxRadius*2, pProp->m_fMaxRadius*2 );
}

CCameraShake::~CCameraShake(void)
{
}

const EFFECT_RESULT CCameraShake::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CCameraShakeProp* pProp = (CCameraShakeProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );
	if( !IsInRenderTime() || !CGraphic::GetInst()->GetCameraShake())
		return Result;
	
	CRenderTarget* pTarget = CGraphic::GetInst()->GetActiveRenderTarget();
	CCamera*	pCamera = pTarget->GetCamera();
	CVector3f CameraPos,CameraDest;
	//CGraphic::GetInst()->GetCamera( &pCamera );
	CameraPos = pCamera->GetPosition();
	//CameraDest = pCamera->getTargetPosition();

	//pCamera->GetCamera( CameraPos, CameraDest );

	CVector3f Offset( 0, 0, 0 );
	float Percent = GetUnitPercent();
	if( pProp->m_vecOffset.size() )
		Offset = GetMapValue( pProp->m_vecOffset, Percent );
	Offset.FastMultiply( matWorld );
	m_matWorld = matWorld;
	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;

	Offset -= CameraPos;
	float Len = Offset.Mag();

	float Strength = 0;
	if( Len < pProp->m_fMinRadius )
		Strength = pProp->m_fStrength;
	else if( Len < pProp->m_fMaxRadius )
		Strength = pProp->m_fStrength*( pProp->m_fMaxRadius - Len )/( pProp->m_fMaxRadius - pProp->m_fMinRadius );
	Strength *= GetMapValue( pProp->m_Scale, Percent )/255.0f;

	float fNx = pProp->m_fFrequence*m_CurTime/1000.0f;
	fNx -= (float)(int32)fNx;
	float fNy = pProp->m_fFrequence*m_CurTime*1.1f/1000.0f;
	fNy -= (float)(int32)fNy;
	float fNz = pProp->m_fFrequence*m_CurTime*0.9f/1000.0f;
	fNz -= (float)(int32)fNz;

	pTarget->AddFilterOff( Strength*pProp->m_pEffectClassDesc->Sin( fNx*6.283f ), 
		Strength*pProp->m_pEffectClassDesc->Sin( fNy*6.283f ), 
		Strength*pProp->m_pEffectClassDesc->Sin( fNz*6.283f ) );

	return Result;
}