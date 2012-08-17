#include "stdafx.h"
#include "CNull.h"
#include "ILogicalHandler.h"


DYNAMIC_CREATE_EFFECTUNITBASE( eNull, IDS_NULL, CNull, CNullProp )

#define NULL_ORIGIN				0
#define NULL_ADD_ROTATE			1
#define NULL_ADD_STOP			2
#define NULL_ADD_TERRAIN		3
#define NULL_ADD_FORBIDS		4

CNullProp::CNullProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_vecRotate				= CVector3f( 0, 0, 0 );
	m_fAngularVelocity		= 0;
	m_bFolowSpeed			= false;
	m_bDelay				= false;
	m_bForbidScale			= false;
	m_eFaceType				= SNLF_NORMAL;
}

CNullProp::~CNullProp(void)
{
}

int32 CNullProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	switch( Version )
	{
	case NULL_ADD_FORBIDS:
		File.read( (char*)&m_bForbidScale,		sizeof(bool) );
	case NULL_ADD_TERRAIN:
		File.read( (char*)&m_eFaceType,			sizeof(SNULL_FACE) );
	case NULL_ADD_STOP:
		File.read( (char*)&m_bDelay,			sizeof(bool) ); 
	case NULL_ADD_ROTATE:
		File.read( (char*)&m_vecRotate,			sizeof(CVector3f) );
		File.read( (char*)&m_bFolowSpeed,		sizeof(BOOL) ); //=== bool
	}
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CNullProp::_UpdateParams(void)
{
	m_fAngularVelocity = m_vecRotate.Mag();
}

int32 CNullProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = NULL_ADD_FORBIDS;
	File.write( (char*)&Version,				sizeof(uint32) );

	File.write( (char*)&m_bForbidScale,			sizeof(bool) );
	File.write( (char*)&m_eFaceType,			sizeof(SNULL_FACE) );
	File.write( (char*)&m_bDelay,				sizeof(bool) );
	File.write( (char*)&m_vecRotate,			sizeof(CVector3f) );
	File.write( (char*)&m_bFolowSpeed,			sizeof(BOOL) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CNull::CNull( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	m_CurAngle = 0.0f;
	m_bDelay = false;
}

CNull::~CNull(void)
{
}

void  CNull::ResetTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::ResetTime(STime,DTime);
	m_CurAngle	= 0.0f;
	m_bDelay	= false;
}

void CNull::SetStartTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::SetStartTime(STime,DTime);
	m_bDelay	= false;
}

const EFFECT_RESULT CNull::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CNullProp* pProp = (CNullProp*)m_pEffectUnitProp;
	
	EFFECT_RESULT Result( NULL, true, false );
	
	if( !IsInRenderTime() || (m_bDelay && pProp->m_bDelay) )
		return Result;

	m_bDelay = true;

	float fvalue   = GetUnitPercent();//m_FrameTime/pProp->m_TimeRange;
	
	CVector3f Offset = GetMapValue( pProp->m_vecOffset, fvalue );

	CMatrix tMatWorld = matWorld;
	if(pProp->m_bForbidScale)
	{

		tMatWorld.SetWithRow( matWorld.GetAxis(0) / m_matWorld.GetAxis(0).Mag(),
			matWorld.GetAxis(1) / m_matWorld.GetAxis(1).Mag(), matWorld.GetAxis(2) / m_matWorld.GetAxis(2).Mag() );
	}

	switch( pProp->m_eFaceType)
	{
	case CNullProp::SNLF_NORMAL:
		{
			Offset.FastMultiply( tMatWorld );
		}
		break;
	case CNullProp::SNLF_TERRAIN:
		{
			if(m_pLogicHandler)
				Offset.y +=  m_pLogicHandler->GetTerrainHeight(tMatWorld._41,tMatWorld._43);
			Offset.x += tMatWorld._41;
			Offset.z += tMatWorld._43;
		}
		break;
	case CNullProp::SNLF_TERRAIN_ROTATE:
		{
			if(m_pLogicHandler)
				Offset.y += m_pLogicHandler->GetTerrainHeight(tMatWorld._41,tMatWorld._43);
			Offset.Rotate( tMatWorld );
			Offset.x += tMatWorld._41;
			Offset.z += tMatWorld._43;
		}
		break;
	default:
		break;
	}


	CMatrix matRotateY;
	if( pProp->m_fAngularVelocity > 0.001f ) 
	{
		m_CurAngle += pProp->m_fAngularVelocity * m_DeltaTime * 0.001f;
		if( m_CurAngle > 6.283f )
			m_CurAngle = 0.0;
		if( m_CurAngle < 0.0f )
			m_CurAngle = 6.283f;
		pProp->m_pEffectClassDesc->SetRotate( matRotateY, CVector3f( 0, 1, 0 ), m_CurAngle );
	}
	else
		matRotateY.Identity();

	CVector3f vecV = pProp->m_bFolowSpeed ? Offset - m_matWorld.GetAxis( 3 ) : pProp->m_vecRotate;
	float fLen = vecV.Mag();
	float fDot = 1.0f;
	if( fLen > 0.00001f )
	{
		vecV.Normalize();
		fDot = vecV.Dot( CVector3f( 0, 1, 0 ) );
	}

	if( abs( fDot ) < 0.9999f )
	{
		float Rxz = sqrtf( vecV.x*vecV.x + vecV.z*vecV.z );
		m_matWorld._11 = vecV.y*vecV.x/Rxz;
		m_matWorld._12 = -Rxz;
		m_matWorld._13 = vecV.y*vecV.z;
		m_matWorld._14 = 0;
		m_matWorld._21 = vecV.x;
		m_matWorld._22 = vecV.y;
		m_matWorld._23 = vecV.z;
		m_matWorld._24 = 0;
		m_matWorld._31 = -vecV.z/Rxz;
		m_matWorld._32 = 0;
		m_matWorld._33 = vecV.x/Rxz;
		m_matWorld._34 = 0;
		m_matWorld._41 = 0;
		m_matWorld._42 = 0;
		m_matWorld._43 = 0;
		m_matWorld._44 = 1;
	}
	else 
	{
		m_matWorld.Identity();
		if( fDot < 0 )
			m_matWorld._11  = m_matWorld._22  = -1;
	}
	m_matWorld = matRotateY*m_matWorld;

	if( !pProp->m_bFolowSpeed )
		m_matWorld *= tMatWorld;

	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;
	return Result;
}

