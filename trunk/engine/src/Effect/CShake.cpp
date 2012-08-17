#include "stdafx.h"
#include "CShake.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "CModel.h"
DYNAMIC_CREATE_EFFECTUNITBASE( eShake, IDS_SHAKE, CShake, CShakeProp )

CShakeProp::CShakeProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
	, m_isCtrlParent(true)
	, m_vecFrequence(11, 10, 9)
	, m_vecDirCtrl(1, 1, 1)
	, m_fStrength(2.0f)
{
}

CShakeProp::~CShakeProp(void)
{
}

int32 CShakeProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	File.read( (char*)&m_vecDirCtrl,			sizeof(CVector3f) );
	File.read( (char*)&m_vecFrequence,			sizeof(CVector3f) );
	File.read( (char*)&m_fStrength,				sizeof(float) );
	File.read( (char*)&m_isCtrlParent,			sizeof(bool) );
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
	{
		return GERROR;
	}
	return 0;
}

int32 CShakeProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = 0;
	File.write( (char*)&Version,				sizeof(uint32) );

	File.write( (char*)&m_vecDirCtrl,			sizeof(CVector3f) );
	File.write( (char*)&m_vecFrequence,			sizeof(CVector3f) );
	File.write( (char*)&m_fStrength,			sizeof(float) );
	File.write( (char*)&m_isCtrlParent,			sizeof(bool) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
	{
		return GERROR;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////


CShake::CShake( IEffectUnitProp* pEffectUnitProp )
	: CEffectUnit( pEffectUnitProp )
	, m_fRotateYDegree(0.0f)
{
	const CShakeProp* pProp = (CShakeProp*)m_pEffectUnitProp;
}

CShake::~CShake(void)
{
}

void CShake::SetRotate( float fY )
{
	m_fRotateYDegree = float(PI - fY);
}

const EFFECT_RESULT CShake::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CShakeProp* pProp = (CShakeProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL,true, false );
	if( !IsInRenderTime() || m_DeltaTime == 0.0f )
	{
		return Result;
	}

	float Strength	= pProp->m_fStrength;
	float fRate		= m_FrameTime/1000.0f;

	// 方向振动频率控制
	float fNx = pProp->m_vecFrequence.x*fRate;
	fNx -= (float)(int32)fNx;
	float fNy = pProp->m_vecFrequence.y*fRate;
	fNy -= (float)(int32)fNy;
	float fNz = pProp->m_vecFrequence.z*fRate;
	fNz -= (float)(int32)fNz;

	// 方向振幅控制
	fNx = Strength*pProp->m_pEffectClassDesc->Sin( fNx*6.283f )*pProp->m_vecDirCtrl.x;
	fNy = Strength*pProp->m_pEffectClassDesc->Sin( fNy*6.283f )*pProp->m_vecDirCtrl.y;
	fNz = Strength*pProp->m_pEffectClassDesc->Sin( fNz*6.283f )*pProp->m_vecDirCtrl.z;

	CMatrix tRot;
	tRot.SetRotateY(m_fRotateYDegree);
	CVector3f temp(fNx, fNy, fNz);
	temp.Rotate(tRot);
	m_matWorld.SetTranslate(temp);

	CMatrix tMat = matWorld;

	tMat.SetWithRow( tMat.GetAxis(0) / matWorld.GetAxis(0).Mag(),
		tMat.GetAxis(1) / matWorld.GetAxis(1).Mag(), 
		tMat.GetAxis(2) /  matWorld.GetAxis(2).Mag(),
		tMat.GetAxis(3) );

	m_matWorld *= tMat;
	if( m_pParent && pProp->m_isCtrlParent )
	{
		if(m_pParent->GetType() == ROT_MODEL)
		{
			((CModel*)m_pParent)->SetFixMatrix(m_matWorld);
			((CModel*)m_pParent)->UpdateRenderStyle(pRORS);
		}
		//m_pParent->Render( m_matWorld, m_CurTime, pRORS );
	}

	return Result;
}
