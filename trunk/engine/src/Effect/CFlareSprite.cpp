#include "stdafx.h"
#include "CFlareSprite.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "TSqrAllocator.inl"
#include "CGraphic.h"
#include "CCamera.h"

DYNAMIC_CREATE_EFFECTUNITBASE( eFlareSprite, IDS_FLARESPRITE, CFlareSprite, CFlareSpriteProp )

CFlareSpriteProp::CFlareSpriteProp( UINT ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_vecRotate		= CVector3f( 0, 0, 0 );
	m_vecNormal		= CVector3f( 0, 0, -1 );
	m_fMaxAngle		= 0.785f;
	m_byMinAlpha	= 0x00;
	m_byMaxAlpha	= 0xff;
	m_fMinSize		= 0;
	m_fMaxSize		= 10;
	m_eFaceType		= SPRF_CAMERA;
	m_eBlendModel	= SPRB_ADD;
	m_eZTest		= Z_TESTONLY;
	m_bLightEnable	= FALSE;
}

CFlareSpriteProp::~CFlareSpriteProp(void)
{
}

int CFlareSpriteProp::LoadBin( IEffectRead& File )
{
	UINT Version;
	File.read( (char*)&Version,					sizeof(UINT) );

	File.read( (char*)&m_vecRotate,				sizeof(CVector3f) );
	File.read( (char*)&m_vecNormal,				sizeof(CVector3f) );
	File.read( (char*)&m_fMinSize,				sizeof(float) );
	File.read( (char*)&m_fMaxSize,				sizeof(float) );
	File.read( (char*)&m_eFaceType,				sizeof(SPR_FACE) );
	File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.read( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.read( (char*)&m_bLightEnable,			sizeof(BOOL) ); 
	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CFlareSpriteProp::_UpdateParams(void)
{
	m_byMaxAlpha = max( m_byMinAlpha, m_byMaxAlpha );
	m_MaxCosAngle = cos( m_fMaxAngle );
	m_vecNormal.Normalize();
	m_AngularVelocity = m_vecRotate.Mag();

	float ProjectXZ = sqrt( m_vecNormal.x*m_vecNormal.x + m_vecNormal.z*m_vecNormal.z );
	CMatrix matRotateX;
	matRotateX.SetRotateX( asinf( m_vecNormal.y ) );
	m_matNormal = matRotateX;
	if( ProjectXZ > 0.001f )
	{
		if( m_vecNormal.z < 0.0f )
			m_matNormal.SetRotateY( asinf( -m_vecNormal.x/ProjectXZ ) );
		else
			m_matNormal.SetRotateY( asinf( m_vecNormal.x/ProjectXZ ) );
		m_matNormal = matRotateX*m_matNormal;
	}
}

int CFlareSpriteProp::SaveBin( IEffectWrite& File )
{
	UINT Version = 0;
	File.write( (char*)&Version,				sizeof(UINT) );

	File.write( (char*)&m_vecRotate,			sizeof(CVector3f) );
	File.write( (char*)&m_vecNormal,			sizeof(CVector3f) );
	File.write( (char*)&m_fMinSize,				sizeof(float) );
	File.write( (char*)&m_fMaxSize,				sizeof(float) );
	File.write( (char*)&m_eFaceType,			sizeof(SPR_FACE) );
	File.write( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.write( (char*)&m_bLightEnable,			sizeof(BOOL) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

void	CFlareSpriteProp::_CalculateAABB()
{
	CEffectUnitProp::_CalculateAABB();
	CVector3f vecMax = m_MaxAABB.getMaximum();
	CVector3f vecMin = m_MaxAABB.getMinimum();

	if (!m_vecRotate.IsZero())
	{
		float min_f = vecMin.x; min_f = min(vecMin.y, min_f); min_f = min(vecMin.z, min_f);
		float max_f = vecMax.x; max_f = max(vecMax.y, max_f); max_f = max(vecMax.z, max_f);
		if ( abs(min_f) < abs(max_f) )
			max_f = abs(max_f);
		else
			max_f = abs(min_f);
		vecMax.x = max_f;	vecMax.y = max_f;	vecMax.z = max_f;
		vecMin.x = -max_f;	vecMin.y = -max_f;	vecMin.z = -max_f;
	}
	
	float sz = m_fMaxSize * 0.8f;
	vecMax.x += sz;	vecMax.y += sz;	vecMax.z += sz;
	vecMin.x -= sz;	vecMin.y -= sz;	vecMin.z -= sz;
	
	m_MaxAABB.setExtents(vecMin, vecMax);
}

//////////////////////////////////////////////////////////////////////////


CFlareSprite::CFlareSprite( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	m_CurAngle		= 0;

	const CFlareSpriteProp* pProp = (CFlareSpriteProp*)m_pEffectUnitProp;
}

CFlareSprite::~CFlareSprite(void)
{
}

const EFFECT_RESULT CFlareSprite::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CFlareSpriteProp* pProp = (CFlareSpriteProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );

	if( !IsInRenderTime() )
		return Result;

	float fPercent = GetUnitPercent();
	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
	//CGraphic::GetInst()->GetCamera( &pCamera );

	float DeltaTime = m_DeltaTime / 1000.0f;
	CVector3f OrgOffset( 0, 0, 0 );
	ITexture* pCurText = NULL;
	uint32 Color = 0xff000000;
	if( pProp->m_vecOffset.size() )
		OrgOffset = GetMapValue( pProp->m_vecOffset, fPercent );
	if( pProp->m_Texture.size() )
		pCurText = GetMapState( pProp->m_Texture, fPercent*pProp->m_nTextureCircle - (int)( fPercent*pProp->m_nTextureCircle ) );
	if( pProp->m_Color.size() )
		Color = GetMapValue( pProp->m_Color, fPercent )&0x00ffffff;

	VerNCT* pVB;
	RenderState* pRS;
	CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
		VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );

	CVector3f InvNormal( 0, 0, 1 );
	if( pProp->m_eFaceType == SPRF_CAMERA || ( pProp->m_vecNormal.x == 0 && pProp->m_vecNormal.y == 0 && pProp->m_vecNormal.z == 0 ) )
		pCamera->GetBillboardAll( m_matWorld );
	else
	{
		if( pProp->m_AngularVelocity > 0.00001f )
		{
			m_CurAngle += pProp->m_AngularVelocity*DeltaTime;
			if( m_CurAngle > 6.283f )
				m_CurAngle = 0.0;
			if( m_CurAngle < 0.0f )
				m_CurAngle = 6.283f;
			CMatrix Rotate;
			pProp->m_pEffectClassDesc->SetRotate( Rotate, pProp->m_vecRotate/pProp->m_AngularVelocity, m_CurAngle );
			if( pProp->m_eFaceType == SPRF_CAMERANONEX )
			{
				pCamera->GetBillboardNoneX( m_matWorld );
				m_matWorld = Rotate*m_matWorld;
			}
			else
				m_matWorld = pProp->m_matNormal*Rotate;
		}
		else
		{
			if( pProp->m_eFaceType == SPRF_CAMERANONEX )
				pCamera->GetBillboardNoneX( m_matWorld );
			else
				m_matWorld = pProp->m_matNormal;
		}

		if( pProp->m_eFaceType == SPRF_NORMAL )
			InvNormal    = -pProp->m_vecNormal;
	}

	CVector3f Offset,CameraDir;
	CameraDir = pCamera->getDIRECTION();//pCamera->GetCameraDir( CameraDir );
	OrgOffset.FastMultiply( m_matWorld );
	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;

	*pMatrix = m_matWorld;

	float Scale = pProp->m_fMinSize;
	uint32 Alpha = pProp->m_byMinAlpha;
	float Dot = InvNormal.Dot( CameraDir );
	if( Dot > pProp->m_MaxCosAngle )
	{
		Scale += abs( pProp->m_fMaxSize - pProp->m_fMinSize )*( Dot - pProp->m_MaxCosAngle )/( 1 - pProp->m_MaxCosAngle );
		Alpha += (uint32)( abs( (int)pProp->m_byMaxAlpha - (int)pProp->m_byMinAlpha )*( Dot - pProp->m_MaxCosAngle )/( 1 - pProp->m_MaxCosAngle ) );
		Alpha =  ( (uint32)(BYTE)Alpha ) << 24;
	}
	Color |= Alpha;


	CVector3f vecS( m_matWorld._11 + m_matWorld._12 + m_matWorld._13, 
					m_matWorld._21 + m_matWorld._22 + m_matWorld._23,
					m_matWorld._31 + m_matWorld._32 + m_matWorld._33 );
	Scale = vecS.Mag()*Scale;

	pRS->m_Cull			= CULLT_NONE;				//D3DCULL_NONE;
	pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
	pRS->m_LightEnable	= pProp->m_bLightEnable;
	pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
	pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;

	if( pProp->m_eBlendModel == SPRB_NONE )
		pRS->m_AlphaBlendEnable = FALSE;
	else
	{
		pRS->m_AlphaBlendEnable = TRUE;
		if( pProp->m_eBlendModel == SPRB_ADD )
			pRS->m_DestBlend = BLEND_ONE;			//D3DBLEND_ONE;
		else if( pProp->m_eBlendModel == SPRB_LIGHT )
		{
			VerNCT* pVB;
			RenderState* pMulRS;
			CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
				VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
			*pMat = m_matWorld;

			pVB[0].Set( -Scale, -Scale, 0.0f, 0, 0, 1, Color, 0.0f, 1.0f );
			pVB[1].Set( -Scale,  Scale, 0.0f, 0, 0, 1, Color, 0.0f, 0.0f );
			pVB[2].Set(  Scale, -Scale, 0.0f, 0, 0, 1, Color, 1.0f, 1.0f );
			pVB[3].Set(  Scale,  Scale, 0.0f, 0, 0, 1, Color, 1.0f, 0.0f );

			pMulRS->m_DestBlend = 2;			//D3DBLEND_ONE;
			pMulRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
			pRS->m_DestBlend = BLEND_ONE;				//D3DBLEND_ONE;
		}
	}

	pVB[0].Set( -Scale, -Scale, 0.0f, 0, 0, 1, Color, 0.0f, 1.0f );
	pVB[1].Set( -Scale,  Scale, 0.0f, 0, 0, 1, Color, 0.0f, 0.0f );
	pVB[2].Set(  Scale, -Scale, 0.0f, 0, 0, 1, Color, 1.0f, 1.0f );
	pVB[3].Set(  Scale,  Scale, 0.0f, 0, 0, 1, Color, 1.0f, 0.0f );

	return Result;
}	

