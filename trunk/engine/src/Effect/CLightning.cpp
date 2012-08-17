#include "stdafx.h"
#include "CLightning.h"
#include "TimeHelper.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "CCamera.h"

DYNAMIC_CREATE_EFFECTUNITBASE( eLightning, IDS_LIGHTNING, CLightning, CLightningProp )

// 文件版本定义
#define LIGHTNING_ORIGIN				0

// 属性
CLightningProp::CLightningProp( UINT ID, IEffectClassDesc* pEffectClassDesc )
: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_vecRotate		= CVector3f( 0, 0, 0 );
	m_vecNormal		= CVector3f( 0, 1, 0 );
	m_fStartAngle	= 0;
	m_fWidthRatio	= 1.0f;
	m_fMinSize		= 0;
	m_fMaxSize		= 10;
	m_eFaceType		= SPRF_CAMERA;
	m_eBlendModel	= SPRB_ADD;
	m_eZTest		= Z_TESTONLY;
	m_bLightEnable	= FALSE;
	m_fZBias		= 0.0f;
	m_vecSrc		= CVector3f( -10.0f, -5.0f, 0.0f );
	m_fDitherMax	= 5;
	m_fDitherMin	= 0;
	m_nSubNum		= 9;
}

CLightningProp::~CLightningProp(void)
{
}

int CLightningProp::LoadBin( IEffectRead& File )
{
	UINT Version;
	File.read( (char*)&Version,					sizeof(UINT) );

	switch( Version )
	{
	case LIGHTNING_ORIGIN:
	default:
		File.read( (char*)&m_vecRotate,				sizeof(CVector3f) );
		File.read( (char*)&m_vecNormal,				sizeof(CVector3f) );
		File.read( (char*)&m_fStartAngle,			sizeof(float) );
		File.read( (char*)&m_fWidthRatio,			sizeof(float) );
		File.read( (char*)&m_fMinSize,				sizeof(float) );
		File.read( (char*)&m_fMaxSize,				sizeof(float) );
		File.read( (char*)&m_vecSrc,				sizeof(CVector3f) );
		File.read( (char*)&m_fDitherMax,			sizeof(float) );
		File.read( (char*)&m_fDitherMin,			sizeof(float) );
		File.read( (char*)&m_nSubNum,				sizeof(int) );
		File.read( (char*)&m_eFaceType,				sizeof(SPR_FACE) );
		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) ); //=== bool
		File.read( (char*)&m_bLightEnable,			sizeof(BOOL) ); //=== bool
		File.read( (char*)&m_fZBias,				sizeof(float) );
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	return 0;
}

void  CLightningProp::_UpdateParams(void)
{
	m_AngularVelocity = m_vecRotate.Mag();
	limit( m_fStartAngle, 0.0f, 6.28f );

	float fNormal = m_vecNormal.Mag();
	if( fNormal > 0.0001 )
		m_vecNormal /= fNormal;
	float ProjectXZ = sqrt( m_vecNormal.x*m_vecNormal.x + m_vecNormal.z*m_vecNormal.z );
	CMatrix matRotateX;
	matRotateX.SetRotateX( asinf( m_vecNormal.y ) );
	m_matNormal = matRotateX;
	if( ProjectXZ > 0.001f )
	{
		m_matNormal.Identity();
		m_matNormal._11 = -m_vecNormal.z/ProjectXZ; 
		m_matNormal._13 =  m_vecNormal.x/ProjectXZ;
		m_matNormal._31 = -m_matNormal._13;
		m_matNormal._33 =  m_matNormal._11;
		m_matNormal = matRotateX*m_matNormal;
	}
}

int CLightningProp::SaveBin( IEffectWrite& File )
{
	UINT Version = LIGHTNING_ORIGIN;
	File.write( (char*)&Version,				sizeof(UINT) );

	File.write( (char*)&m_vecRotate,			sizeof(CVector3f) );
	File.write( (char*)&m_vecNormal,			sizeof(CVector3f) );
	File.write( (char*)&m_fStartAngle,			sizeof(float) );
	File.write( (char*)&m_fWidthRatio,			sizeof(float) );
	File.write( (char*)&m_fMinSize,				sizeof(float) );
	File.write( (char*)&m_fMaxSize,				sizeof(float) );
	File.write( (char*)&m_vecSrc,				sizeof(CVector3f) );
	File.write( (char*)&m_fDitherMax,			sizeof(float) );
	File.write( (char*)&m_fDitherMin,			sizeof(float) );
	File.write( (char*)&m_nSubNum,				sizeof(int) );
	File.write( (char*)&m_eFaceType,			sizeof(SPR_FACE) );
	File.write( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.write( (char*)&m_bLightEnable,			sizeof(BOOL) );
	File.write( (char*)&m_fZBias,				sizeof(float) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}


// 实现 ------------------------------------------------------------------
CLightning::CLightning( IEffectUnitProp* pEffectUnitProp )
	: CEffectUnit( pEffectUnitProp )
{
	m_CurAngle		= 0;
	m_pParent		= NULL;

	const CLightningProp* pProp = (CLightningProp*)m_pEffectUnitProp;
	//m_vecScale += CVector3f( pProp->m_fMaxSize*2, pProp->m_fMaxSize*2, pProp->m_fMaxSize*2 );

	m_nSubTick	= 0;
}

CLightning::~CLightning(void)
{
}

const EFFECT_RESULT CLightning::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CLightningProp* pProp = (CLightningProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );
	
	if( !IsInRenderTime() )
	{
		return Result;
	}
	float Percent = GetUnitPercent();
	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();

	CVector3f OrgOffset( 0, 0, 0 );
	ITexture* pCurText = NULL;
	uint32 Color;
	float Scale = 1.0f;
	
	pProp->GetCurAdvanceProp( Percent, OrgOffset, pCurText, Color, Scale );
	Scale = pProp->m_fMinSize + ( pProp->m_fMaxSize - pProp->m_fMinSize )*Scale;

	// 生成节点
	long VBNum = pProp->m_nSubNum*2+2;
	//CVector3f * pos = new CVector3f[VBNum];
	EVector<CVector3f> pos(VBNum);
	// 设置起点
	float fHeight = OrgOffset.y - pProp->m_vecSrc.y;
	pos[0].Init( pProp->m_vecSrc.x, pProp->m_vecSrc.y*pProp->m_fWidthRatio, pProp->m_vecSrc.z );
	pos[1].Init( pProp->m_vecSrc.x, (pProp->m_vecSrc.y + fHeight)*pProp->m_fWidthRatio, pProp->m_vecSrc.z );

	// 设置终点，根据轨迹设置
	pos[VBNum-2].Init( OrgOffset.x, (OrgOffset.y - fHeight)*pProp->m_fWidthRatio, OrgOffset.z );
	pos[VBNum-1].Init( OrgOffset.x, OrgOffset.y*pProp->m_fWidthRatio, OrgOffset.z );


	// 确定每段的长度
	float SubLength = (OrgOffset.x - pProp->m_vecSrc.x)/(float)pProp->m_nSubNum;

	// 初始化中间的点
	for( long i = 2; i < VBNum-2; i += 2 )
	{
		pos[i].Init( pProp->m_vecSrc.x + (i/2)*SubLength, pProp->m_vecSrc.y*pProp->m_fWidthRatio, pProp->m_vecSrc.z );
		pos[i+1].Init( pProp->m_vecSrc.x + (i/2)*SubLength, OrgOffset.y*pProp->m_fWidthRatio, OrgOffset.z );
	}

	VerNCT* pVB;
	STATIC_RS* pRS;
	CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pRORS->m_eType, pCurText, NULL, 
		PT_TRIANGLESTRIP, VBNum, pProp->m_nSubNum*2, VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );

	bool bScale = true;

	// 面向摄像机
	if( pProp->m_eFaceType == SPRF_CAMERA || ( pProp->m_vecNormal.x == 0 && pProp->m_vecNormal.y == 0 && pProp->m_vecNormal.z == 0 ) )
	{
		pCamera->GetBillboardAll( m_matWorld );
		if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
		{
			if( pProp->m_fStartAngle > 0.00001f )
				m_CurAngle = pProp->m_fStartAngle;
			else
				m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime;

			if( m_CurAngle > 6.283f )
				m_CurAngle = 0.0;
			if( m_CurAngle < 0.0f )
				m_CurAngle = 6.283f;
			CMatrix Rotate;
			pProp->m_pEffectClassDesc->SetRotate( Rotate, CVector3f( 0, 0, pProp->m_vecRotate.z > 0 ? 1.0f : -1.0f ), m_CurAngle );
			m_matWorld = Rotate * m_matWorld;
		}
	}
	else
	{
		if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
		{
			if( pProp->m_fStartAngle > 0.00001f )
				m_CurAngle = pProp->m_fStartAngle;
			else
				m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime;

			if( m_CurAngle > 6.283f )
				m_CurAngle = 0.0f;
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
			{
				m_matWorld = pProp->m_matNormal*Rotate;
				if( m_pParent && m_pParent->GetType() == ROT_MODEL )
				{
					CMatrix mat;
					((CModel*)m_pParent)->GetMatrix( mat, 0, 0xffffffff ); 
					m_matWorld = m_matWorld*mat;
					bScale = false;
				}
			}
		}
		else
		{
			if( pProp->m_eFaceType == SPRF_CAMERANONEX )
				pCamera->GetBillboardNoneX( m_matWorld );
			else
			{
				m_matWorld = pProp->m_matNormal;
				if( m_pParent && m_pParent->GetType() == ROT_MODEL )
				{
					CMatrix mat;
					((CModel*)m_pParent)->GetMatrix( mat, 0, 0xffffffff );
					m_matWorld = m_matWorld*mat;
					bScale = false;
				}
			}
		}
	}

	// 位置
	CVector3f Offset = OrgOffset;
	Offset.FastMultiply( matWorld );
	m_matWorld._41 = Offset.x;
	m_matWorld._42 = Offset.y;
	m_matWorld._43 = Offset.z;

	*pMatrix = m_matWorld;

	if( bScale )
	{
		CVector3f vecS( matWorld._11 + matWorld._12 + matWorld._13, 
			matWorld._21 + matWorld._22 + matWorld._23,
			matWorld._31 + matWorld._32 + matWorld._33 );
		Scale = vecS.Mag()*Scale;
	}
	else
		Scale = 1.732f*Scale;

	CVector3f vecS( matWorld._11 + matWorld._12 + matWorld._13, 
		matWorld._21 + matWorld._22 + matWorld._23,
		matWorld._31 + matWorld._32 + matWorld._33 );
	Scale = vecS.Mag()*Scale;

	// 抖动中间的点
	float fSubTexLength = 1/(float)pProp->m_nSubNum;

	for( long i = 2; i < VBNum-2; i += 2 )
	{
		float DitherOffset = CQuickRand::Rand2( pProp->m_fDitherMin, pProp->m_fDitherMax )*CQuickRand::Rand2( -1, 1 );
		pVB[i].Set( pos[i].x*Scale, (pos[i].y+DitherOffset)*Scale, pos[i].z*Scale, 0, 0, 1, Color, (i/2)*fSubTexLength, 1.0f );
		pVB[i+1].Set( pos[i+1].x*Scale, (pos[i+1].y+DitherOffset)*Scale, pos[i+1].z*Scale, 0, 0, 1, Color, (i/2)*fSubTexLength, 0.0f );
	}

	// ----------------
	pRS->m_Cull		= CULLT_NONE;				//D3DCULL_NONE;
	pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
	pRS->m_LightEnable	= pProp->m_bLightEnable;
	pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
	pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
	float zbias = 0.0f;
	if ( pProp->m_fZBias != 0.0f )
		zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
		pCamera->getFarDist(), pCamera->getNearDist() );
	//pRS->m_fSlopeScaleZBias	= 0.0f;
	pRS->m_fZBias			= zbias;

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
			CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, VBNum, pProp->m_nSubNum*2, 
				VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
			*pMat = *pMatrix;

			for( long i = 2; i < VBNum-2; i += 2 )
			{
				float DitherOffset = CQuickRand::Rand2( pProp->m_fDitherMin, pProp->m_fDitherMax )*CQuickRand::Rand2( -1, 1 );
				pVB[i].Set( pos[i].x*Scale, (pos[i].y+DitherOffset)*Scale, pos[i].z*Scale, 0, 0, 1, Color, (i/2)*fSubTexLength, 1.0f );
				pVB[i+1].Set( pos[i+1].x*Scale, (pos[i+1].y+DitherOffset)*Scale, pos[i+1].z*Scale, 0, 0, 1, Color, (i/2)*fSubTexLength, 0.0f );
			}
			pVB[0].Set( pos[0].x*Scale, pos[0].y*Scale, pos[0].z*Scale, 0, 0, 1, Color, 0.0f, 1.0f );
			pVB[1].Set( pos[1].x*Scale, pos[1].y*Scale, pos[1].z*Scale, 0, 0, 1, Color, 0.0f, 0.0f );
			pVB[VBNum-2].Set( pos[VBNum-2].x*Scale, pos[VBNum-2].y*Scale, pos[VBNum-2].z*Scale, 0, 0, 1, Color, 1.0f, 1.0f );
			pVB[VBNum-1].Set( pos[VBNum-1].x*Scale, pos[VBNum-1].y*Scale, pos[VBNum-1].z*Scale, 0, 0, 1, Color, 1.0f, 0.0f );


			pMulRS->m_DestBlend = 2;			//D3DBLEND_ONE;
			pMulRS->m_Cull			= 1;		//D3DCULL_NONE;
			pMulRS->m_AlpOP_S0		= 4;		//D3DTOP_MODULATE;
			pMulRS->m_LightEnable	= pProp->m_bLightEnable;
			pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite				= pProp->m_eZTest != Z_TESTONLY;

			pRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
			pRS->m_DestBlend = BLEND_ONE;				//D3DBLEND_ONE;
		}
	}

	pVB[0].Set( pos[0].x*Scale, pos[0].y*Scale, pos[0].z*Scale, 0, 0, 1, Color, 0.0f, 1.0f );
	pVB[1].Set( pos[1].x*Scale, pos[1].y*Scale, pos[1].z*Scale, 0, 0, 1, Color, 0.0f, 0.0f );
	pVB[VBNum-2].Set( pos[VBNum-2].x*Scale, pos[VBNum-2].y*Scale, pos[VBNum-2].z*Scale, 0, 0, 1, Color, 1.0f, 1.0f );
	pVB[VBNum-1].Set( pos[VBNum-1].x*Scale, pos[VBNum-1].y*Scale, pos[VBNum-1].z*Scale, 0, 0, 1, Color, 1.0f, 0.0f );


	pos.clear();
	return Result;
}
