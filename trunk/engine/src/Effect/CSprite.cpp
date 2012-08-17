#include "stdafx.h"
#include "CSprite.h"
#include "CoreCommon.h"
#include "CRenderTarget.h"
#include "CWindowTarget.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"
#include "ILogicalHandler.h"
#include "CGraphic.h"
#include "CCamera.h"

DYNAMIC_CREATE_EFFECTUNITBASE( eSprite, IDS_SPRITE, CSprite, CSpriteProp )
#define SPRITE_HIGH					3

#define SPRITE_ORIGIN				0
#define SPRITE_ADD_ZBIAS			1
#define SPRITE_ADD_ROTATEOFS		2
#define SPRITE_ADD_XYZCALE			3 //增加XYZ缩放量
#define SPRITE_ADD_UVROTATECENTER	4 //增加朝向地表纹理绕某中心旋转
#define SPRITE_ADD_BIRTH_DELETE_FLAG 5 //增加朝向地表纹理绕某中心旋转

CSpriteProp::CSpriteProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
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
	m_eTextOp		= SPRO_MODULATE;
	m_bLightEnable	= FALSE;
	m_isShowFrame	= FALSE;
	m_fZBias		= 0.0f;
	m_XYZcale[0.0f].x = 1.0f;
	m_XYZcale[0.0f].y = 10.0f;
	m_UVRCenter[0.0f].x   = 0.5f;
	m_UVRCenter[0.0f].y   = 0.0f;
	m_UVRCenter[0.0f].z   = 0.5f;
	m_BirthTime			= 0;
	m_DeleteTime		= 1000;
	m_AngularVelocity = m_vecRotate.Mag();
}

CSpriteProp::~CSpriteProp(void)
{
}

bool	CSpriteProp::GetMaxSize(float& fValue)
{
	fValue = m_fMaxSize;
	return true;
}

int32 CSpriteProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	switch( Version )
	{
	case SPRITE_ADD_BIRTH_DELETE_FLAG:
		File.read( (char*)&m_BirthTime,				sizeof(int) );
		File.read( (char*)&m_DeleteTime,			sizeof(int) );
		File.read( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP) );
	case SPRITE_ADD_UVROTATECENTER:
		LoadMapValue( File, m_UVRCenter );
	case SPRITE_ADD_XYZCALE:
		LoadMapValue( File, m_XYZcale );
	case SPRITE_ADD_ROTATEOFS:
		File.read( (char*)&m_fStartAngle,			sizeof(float) );
		File.read( (char*)&m_fWidthRatio,			sizeof(float) );
	case SPRITE_ADD_ZBIAS:
		File.read( (char*)&m_fZBias,				sizeof(float) );
	default:
		File.read( (char*)&m_vecRotate,				sizeof(CVector3f) );
		File.read( (char*)&m_vecNormal,				sizeof(CVector3f) );
		File.read( (char*)&m_fMinSize,				sizeof(float) );
		File.read( (char*)&m_fMaxSize,				sizeof(float) );
		File.read( (char*)&m_eFaceType,				sizeof(SPR_FACE) );
		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) );
		File.read( (char*)&m_bLightEnable,			sizeof(BOOL) );

		if(Version >=SPRITE_ADD_ROTATEOFS)
		{
			m_XYZcale[0.0f].x = m_fWidthRatio;
			m_XYZcale[0.0f].y = m_fMaxSize;
		}

	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CSpriteProp::CaculateProp()
{
	m_XYZcale[0.0f].x = m_fWidthRatio;
	m_XYZcale[0.0f].y = m_fMaxSize;	
}

void CSpriteProp::_UpdateParams(void)
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

int32 CSpriteProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = SPRITE_ADD_BIRTH_DELETE_FLAG;
	File.write( (char*)&Version,				sizeof(uint32) );


	File.write( (char*)&m_BirthTime,			sizeof(int) );
	File.write( (char*)&m_DeleteTime,			sizeof(int) );
	File.write( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP) );


	SaveMapValue( File, m_UVRCenter );
	SaveMapValue( File, m_XYZcale );
	m_fWidthRatio = m_XYZcale[0.0f].x;
	m_fMaxSize = m_XYZcale[0.0f].y;

	File.write( (char*)&m_fStartAngle,			sizeof(float) );
	File.write( (char*)&m_fWidthRatio,			sizeof(float) );
	File.write( (char*)&m_fZBias,				sizeof(float) );
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

void	CSpriteProp::_CalculateAABB()
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
	// 缩放
	float fScale = 0.0f;
	CVector3f XYZcale(0,0,0);
	for( EMap< float, BYTE >::iterator it = m_Scale.begin(); 
		it != m_Scale.end(); it++ )
		fScale = max( it->second/255.f, fScale );

	for( EMap< float, CVector3f >::iterator it = m_XYZcale.begin(); 
		it != m_XYZcale.end(); it++ )
	{
		XYZcale.x = max( it->second.x, XYZcale.x );
		XYZcale.y = max( it->second.y, XYZcale.y );
	}
	fScale = m_fMinSize + ( XYZcale.y - m_fMinSize )*fScale;
	fScale *=XYZcale.x;
	float sz = fScale * 0.8f;
	// 朝向
	switch(m_eFaceType)
	{
	case SPRF_NORMAL:
	case SPRF_CAMERA:			// 摄像机
	case SPRF_NORMAL_STATIC:
	case SPRF_CAMERANONEX:
	case SPRF_WORLDNORPAR:		// 场景
		vecMax.x += sz;	vecMax.y += sz;	vecMax.z += sz;
		vecMin.x -= sz;	vecMin.y -= sz;	vecMin.z -= sz;
		break;
	case SPRF_TERRAIN:			// 地形
	case SPRF_TERRAIN_ROTATE:
		vecMax.x += sz;	vecMax.y += 1.0f; vecMax.z += sz;
		vecMin.x -= sz;	vecMax.y += 1.0f; vecMin.z -= sz;
		break;
	}

	m_MaxAABB.setExtents(vecMin, vecMax);
}

//////////////////////////////////////////////////////////////////////////


CSprite::CSprite( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	m_CurAngle		= 0;
	m_pParent		= NULL;
	m_DelayRender	= (float)((CSpriteProp*)m_pEffectUnitProp)->m_DeleteTime;
	const CSpriteProp* pProp = (CSpriteProp*)m_pEffectUnitProp;
	//CVector3f XYZcale = GetMapValue( pProp->m_XYZcale, 0.0f );
	//m_vecScale += CVector3f( XYZcale.y*2, XYZcale.y*2, XYZcale.y*2 );

	m_vecRotateCenter = GetMapValue( pProp->m_UVRCenter, 0.0f );

	m_DelayRender	= (float)((CSpriteProp*)m_pEffectUnitProp)->m_DeleteTime;
	m_BirthRender	= 0.0f;//(float)((CSpriteProp*)m_pEffectUnitProp)->m_BirthTime;
}

CSprite::~CSprite(void)
{
}

void CSprite::ResetTime( uint32 STime,uint32 DTime )
{
	CEffectUnit::ResetTime(STime,DTime);
	m_CurAngle = 0.0f;
}

void CSprite::OnLinked( CRenderable* pParentObj  )
{
	if( pParentObj->GetType() != ROT_MODEL )
		return; 

	CEffectUnit::OnLinked(pParentObj);
}

void CSprite::OnUnlinked(void)
{
	m_pParent = NULL;
}

const EFFECT_RESULT	CSprite::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	bool bRenderTerrain = false;
	const CSpriteProp* pProp = (CSpriteProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );

	float fPercent = GetUnitPercent();//m_FrameTime/pProp->m_TimeRange;
	
	if( !IsInRenderTime() )
	{
		return Result;
	}

	m_matWorld = matWorld;
	m_ParentMat = matWorld;
	CCamera* pCamera = NULL;
	if ( CGraphic::GetInst()->GetActiveRenderTarget() )
		pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	else	
		pCamera = CMainWindowTarget::GetInst()->GetCamera();

	CVector3f OrgOffset( 0, 0, 0 );
	ITexture* pCurText = NULL;
	uint32 Color=0;
	uint32 ColorFrame;
	ColorFrame = CColor(255,255,0,0);
	float fScale = 1.0f;
	float fzoomScale = 1.0f;
	CVector3f XYZcale(0,0,0);
	
	pProp->GetCurAdvanceProp( fPercent, OrgOffset, pCurText, Color, fScale, XYZcale, m_vecRotateCenter);
	fScale = pProp->m_fMinSize + ( XYZcale.y - pProp->m_fMinSize )*fScale;

	if(m_BirthRender>=pProp->m_BirthTime)
		Color = Color & 0x00ffffff |(((uint32)(Color * (m_DelayRender / pProp->m_DeleteTime))) & 0xff000000);
	else
	{
		Color = Color & 0x00ffffff | (((uint32)(Color * (m_BirthRender / pProp->m_BirthTime ))) & 0xff000000);
		m_BirthRender += m_DeltaTime;
	}

	CVector3f pos[4];

	CVector3f vecDir;
	vecDir = pCamera->getDIRECTION();//pCamera->GetCameraDir( vecDir );
	vecDir = vecDir*pProp->m_fZBias;

	switch(pProp->m_eFaceType)
	{
	case SPRF_NORMAL_STATIC:
	case SPRF_NORMAL:
		{
			VerNCT* pVB;
			VerNCT* pVBframe;
			STATIC_RS* pRS;
			CMatrix* pMatrixFrame;
			CMatrix* pMatrix;
			if(pRORS)	
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}
			else
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}
			
			bool bScale = true;

			if(  pProp->m_vecNormal.x == 0 && pProp->m_vecNormal.y == 0 && pProp->m_vecNormal.z == 0  )
			{
				pCamera->GetBillboardAll( m_matWorld );
				if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
				{
					if( pProp->m_fStartAngle > 0.00001f )
						m_CurAngle = pProp->m_fStartAngle;
					else
						m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime * 0.001f;

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
						m_CurAngle += pProp->m_AngularVelocity * m_DeltaTime * 0.001f;

					if( m_CurAngle > 6.283f )
						m_CurAngle = 0.0f;
					if( m_CurAngle < 0.0f )
						m_CurAngle = 6.283f;
					CMatrix Rotate;
					pProp->m_pEffectClassDesc->SetRotate( Rotate, pProp->m_vecRotate/pProp->m_AngularVelocity, m_CurAngle );
			
					m_matWorld = pProp->m_matNormal*Rotate;
					if( m_pParent && m_pParent->GetType() == ROT_MODEL && pProp->m_eFaceType!=SPRF_NORMAL_STATIC)
					{
						CMatrix temp;
						((CModel*)m_pParent)->GetMatrix(temp,0,0xffffffff);
						m_matWorld = m_matWorld*temp;
						bScale = false;
					}
					else
						m_matWorld = m_matWorld * matWorld;
				}
				else
				{
					m_matWorld = pProp->m_matNormal;
 					if( m_pParent && m_pParent->GetType() == ROT_MODEL && pProp->m_eFaceType!=SPRF_NORMAL_STATIC)
 					{
 						CMatrix temp;
 						((CModel*)m_pParent)->GetMatrix(temp,0,0xffffffff);
 
 						m_matWorld = m_matWorld * temp;
 						bScale = false;
 					}
					else
						m_matWorld = m_matWorld * matWorld;
				}
			}

			CVector3f Offset = OrgOffset;
			Offset.FastMultiply( matWorld );

			m_matWorld._41 = Offset.x;
			m_matWorld._42 = Offset.y+SPRITE_HIGH;
			m_matWorld._43 = Offset.z;

			*pMatrix = m_matWorld;
			if(pProp->m_isShowFrame)
				*pMatrixFrame = m_matWorld;

			if( bScale )
			{
				CVector3f vecS( matWorld._11 + matWorld._12 + matWorld._13, 
					matWorld._21 + matWorld._22 + matWorld._23,
					matWorld._31 + matWorld._32 + matWorld._33 );
				fzoomScale = vecS.Mag();
				fScale = fzoomScale*fScale;
				fzoomScale = fzoomScale/1.732f;
			}
			else
				fScale = 1.732f*fScale;
			CMatrix mat;
			mat.SetScale(fzoomScale,fzoomScale,fzoomScale);
			m_matWorld = mat * m_matWorld;

			pos[0].Init( -fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[1].Init( -fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[2].Init(  fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[3].Init(  fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );

			pRS->m_Cull		= CULLT_NONE;				//D3DCULL_NONE;
			pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
			pRS->m_LightEnable	= pProp->m_bLightEnable;
			pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
			float zbias = 0.0f;
			if ( pProp->m_fZBias != 0.0f )
				zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
				pCamera->getFarDist(), pCamera->getNearDist() );
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
					CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
					*pMat = *pMatrix;
					
					pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
					pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
					pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
					pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

					pMulRS->m_DestBlend		= 2;			//D3DBLEND_ONE;
					pMulRS->m_Cull			= 1;				//D3DCULL_NONE;
					pMulRS->m_AlpOP_S0		= 4;				//D3DTOP_MODULATE;
					pMulRS->m_LightEnable	= pProp->m_bLightEnable;
					pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
					pRS->m_ZWrite				= pProp->m_eZTest != Z_TESTONLY;
					pMulRS->m_TexOP_S0			= 2 + pProp->m_eTextOp;
					pRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
					pRS->m_DestBlend = BLEND_ONE;				//D3DBLEND_ONE;
				}
			}


			if( !bRenderTerrain )
			{
				pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
				pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
				pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
				pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

				if(pProp->m_isShowFrame)
				{
					pVBframe[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
					pVBframe[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, ColorFrame, 0.0f, 0.0f );
					pVBframe[2].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, ColorFrame, 1.0f, 0.0f );
					pVBframe[3].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, ColorFrame, 1.0f, 1.0f );
					pVBframe[4].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
				}
			}
		}
		break;
	case SPRF_CAMERA:
		{
			VerNCT* pVB;
			VerNCT* pVBframe;
			STATIC_RS* pRS = NULL;
			CMatrix* pMatrixFrame;
			CMatrix* pMatrix;
			if(pRORS)	
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}
			else
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}

			bool bScale = true;

			pCamera->GetBillboardAll( m_matWorld );
			if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
			{
				if( pProp->m_fStartAngle > 0.00001f )
					m_CurAngle = pProp->m_fStartAngle;
				else
					m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime* 0.001f;

				if( m_CurAngle > 6.283f )
					m_CurAngle = 0.0;
				if( m_CurAngle < 0.0f )
					m_CurAngle = 6.283f;
				CMatrix Rotate;
				pProp->m_pEffectClassDesc->SetRotate( Rotate, CVector3f( 0, 0, pProp->m_vecRotate.z > 0 ? 1.0f : -1.0f ), m_CurAngle );
				m_matWorld = Rotate * m_matWorld;
			}

			CVector3f Offset = OrgOffset;
			Offset.FastMultiply( matWorld );
			m_matWorld._41 = Offset.x;
			m_matWorld._42 = Offset.y+SPRITE_HIGH;
			m_matWorld._43 = Offset.z;

			*pMatrix = m_matWorld;
			if(pProp->m_isShowFrame)
				*pMatrixFrame = m_matWorld;

			if( bScale )
			{
				CVector3f vecS( matWorld._11 + matWorld._12 + matWorld._13, 
					matWorld._21 + matWorld._22 + matWorld._23,
					matWorld._31 + matWorld._32 + matWorld._33 );
				fzoomScale = vecS.Mag();
				fScale = fzoomScale*fScale;
				fzoomScale = fzoomScale/1.732f;
			}
			else
				fScale = 1.732f*fScale;
			CMatrix mat;
			mat.SetScale(fzoomScale,fzoomScale,fzoomScale);
			m_matWorld = mat * m_matWorld;

			pos[0].Init( -fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[1].Init( -fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[2].Init(  fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[3].Init(  fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
		
			float zbias = 0.0f;
			if ( pProp->m_fZBias != 0.0f )
				zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
							   pCamera->getFarDist(), pCamera->getNearDist() );
			pRS->m_Cull		= CULLT_NONE;				//D3DCULL_NONE;
			pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
			pRS->m_LightEnable	= pProp->m_bLightEnable;
			pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite		= pProp->m_eZTest != Z_TESTONLY;
			//pRS->m_fSlopeScaleZBias	= 0.0f;
			pRS->m_fZBias		= zbias;

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
					CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
					*pMat = *pMatrix;

					pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
					pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
					pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
					pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

					pMulRS->m_DestBlend		= 2;			//D3DBLEND_ONE;
					pMulRS->m_Cull			= 1;			//D3DCULL_NONE;
					pMulRS->m_AlpOP_S0		= 4;			//D3DTOP_MODULATE;
					pMulRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
					pMulRS->m_LightEnable	= pProp->m_bLightEnable;
					pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
					pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;

					pRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
					pRS->m_DestBlend = BLEND_ONE;		//D3DBLEND_ONE;
				}
			}


			if( !bRenderTerrain )
			{
				pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
				pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
				pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
				pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

				if(pProp->m_isShowFrame)
				{
					pVBframe[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
					pVBframe[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, ColorFrame, 0.0f, 0.0f );
					pVBframe[2].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, ColorFrame, 1.0f, 0.0f );
					pVBframe[3].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, ColorFrame, 1.0f, 1.0f );
					pVBframe[4].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
				}
			}
		}
		break;
	case SPRF_CAMERANONEX:
		{
			VerNCT* pVB;
			VerNCT* pVBframe;
			STATIC_RS* pRS;
			CMatrix* pMatrixFrame;
			CMatrix* pMatrix;
			if(pRORS)	
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}
			else
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}

			bool bScale = true;

			if(  pProp->m_vecNormal.x == 0 && pProp->m_vecNormal.y == 0 && pProp->m_vecNormal.z == 0 )
			{
				pCamera->GetBillboardAll( m_matWorld );
				if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
				{
					if( pProp->m_fStartAngle > 0.00001f )
						m_CurAngle = pProp->m_fStartAngle;
					else
						m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime* 0.001f;

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
						m_CurAngle += pProp->m_AngularVelocity*m_DeltaTime* 0.001f;

					if( m_CurAngle > 6.283f )
						m_CurAngle = 0.0f;
					if( m_CurAngle < 0.0f )
						m_CurAngle = 6.283f;
					CMatrix Rotate;
					pProp->m_pEffectClassDesc->SetRotate( Rotate, pProp->m_vecRotate/pProp->m_AngularVelocity, m_CurAngle );
					pCamera->GetBillboardNoneX( m_matWorld );
					m_matWorld = Rotate*m_matWorld;
		
				}
				else
					pCamera->GetBillboardNoneX( m_matWorld );
			}

			CVector3f Offset = OrgOffset;
			Offset.FastMultiply( matWorld );
			m_matWorld._41 = Offset.x;
			m_matWorld._42 = Offset.y+SPRITE_HIGH;
			m_matWorld._43 = Offset.z;

			*pMatrix = m_matWorld;
			if(pProp->m_isShowFrame)
				*pMatrixFrame = m_matWorld;

			if( bScale )
			{
				CVector3f vecS( matWorld._11 + matWorld._12 + matWorld._13, 
					matWorld._21 + matWorld._22 + matWorld._23,
					matWorld._31 + matWorld._32 + matWorld._33 );
				
				fzoomScale = vecS.Mag();
				fScale = fzoomScale*fScale;
				fzoomScale = fzoomScale/1.732f;
			}
			else
				fScale = 1.732f*fScale;
			CMatrix mat;
			mat.SetScale(fzoomScale,fzoomScale,fzoomScale);
			m_matWorld = mat * m_matWorld;

			pos[0].Init( -fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[1].Init( -fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[2].Init(  fScale, -fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pos[3].Init(  fScale,  fScale*XYZcale.x/*pProp->m_fWidthRatio*/, 0.0f );
			pRS->m_Cull		= CULLT_NONE;				//D3DCULL_NONE;
			pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
			pRS->m_LightEnable	= pProp->m_bLightEnable;
			pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
			float zbias = 0.0f;
			if ( pProp->m_fZBias != 0.0f )
				zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
				pCamera->getFarDist(), pCamera->getNearDist() );
			//pRS->m_fSlopeScaleZBias	= 1.0f;
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
					CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
					*pMat = *pMatrix;

					pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
					pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
					pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
					pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

					pMulRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
					pMulRS->m_DestBlend		= 2;			//D3DBLEND_ONE;
					pMulRS->m_Cull			= 1;				//D3DCULL_NONE;
					pMulRS->m_AlpOP_S0		= 4;				//D3DTOP_MODULATE;
					pMulRS->m_LightEnable	= pProp->m_bLightEnable;
					pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
					pRS->m_ZWrite				= pProp->m_eZTest != Z_TESTONLY;

					pRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
					pRS->m_DestBlend = BLEND_ONE;				//D3DBLEND_ONE;
				}
			}


			if( !bRenderTerrain )
			{
				pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
				pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
				pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
				pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

				if(pProp->m_isShowFrame)
				{
					pVBframe[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
					pVBframe[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, ColorFrame, 0.0f, 0.0f );
					pVBframe[2].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, ColorFrame, 1.0f, 0.0f );
					pVBframe[3].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, ColorFrame, 1.0f, 1.0f );
					pVBframe[4].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
				}
			}
		}
		break;
	case SPRF_WORLDNORPAR://绘制朝向场景
		{
			VerNCT* pVB;
			VerNCT* pVBframe;
			STATIC_RS* pRS;
			CMatrix* pMatrixFrame;
			CMatrix* pMatrix;
			if(pRORS)	
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}
			else
			{
				pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, //PT_TRIANGLESTRIP
					VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pRS );
				pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
				if(pProp->m_isShowFrame)
				{
					pMatrixFrame = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 5, 4, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				}
			}

			pMatrix->Identity();
			m_matWorld = matWorld;
			OrgOffset.FastMultiply( m_matWorld );
			m_matWorld.m[3][0] = OrgOffset.x;
			m_matWorld.m[3][1] = OrgOffset.y;
			m_matWorld.m[3][2] = OrgOffset.z;

			CVector3f vecSpeed = m_matWorld.m[0];
			vecSpeed *= fScale;
			CVector3f vecTemp;
			if( abs( pProp->m_vecNormal.x ) < 0.0001f &&
				abs( pProp->m_vecNormal.y ) < 0.0001f &&
				abs( pProp->m_vecNormal.z ) < 0.0001f  ) 
			{
				vecTemp = pCamera->getDIRECTION();//pCamera->GetCameraDir( vecTemp );
				vecTemp = vecSpeed.Cross( vecTemp );
				vecTemp.Normalize();
				vecTemp  *= vecSpeed.Mag();
			}
			else
			{
				CVector3f vecX;
				CVector3f vNor = pProp->m_vecNormal;
				vNor.Rotate( m_matWorld );

				vecTemp = vecSpeed.Cross( vNor );
				vecX    = vecTemp.Cross( vecSpeed );

				float fSin = pProp->m_pEffectClassDesc->Sin( pProp->m_fStartAngle );
				float fCos = pProp->m_pEffectClassDesc->Cos( pProp->m_fStartAngle );

				vecX.Normalize();
				vecX  *= vecSpeed.Mag();

				vecTemp  = vecSpeed*fSin + vecX*fCos;
				vecSpeed = vecSpeed*fCos - vecX*fSin;
			}
			vecSpeed *=  XYZcale.x;
			//vecSpeed *= pProp->m_fWidthRatio;

			CVector3f Pos = m_matWorld.m[3]; 

			CVector3f p1( -vecSpeed - vecTemp );
			pos[0].Init( p1.x + Pos.x, p1.y + Pos.y, p1.z + Pos.z );
			CVector3f p2( -vecSpeed + vecTemp );
			pos[1].Init( p2.x + Pos.x, p2.y + Pos.y, p2.z + Pos.z );
			CVector3f p3( vecSpeed - vecTemp );
			pos[2].Init( p3.x + Pos.x, p3.y + Pos.y, p3.z + Pos.z );
			CVector3f p4( vecSpeed + vecTemp );
			pos[3].Init( p4.x + Pos.x, p4.y + Pos.y, p4.z + Pos.z );

			pRS->m_Cull		= CULLT_NONE;				//D3DCULL_NONE;
			pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
			pRS->m_LightEnable	= pProp->m_bLightEnable;
			pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
			float zbias = 0.0f;
			if ( pProp->m_fZBias != 0.0f )
				zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
				pCamera->getFarDist(), pCamera->getNearDist() );
			//pRS->m_fSlopeScaleZBias	= 1.0f;
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
					CMatrix* pMat = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 4, 2, 
						VerNCT::Format, sizeof(VerNCT), (void**)&pVB, NULL, (void**)&pMulRS );
					*pMat = *pMatrix;

					pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
					pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
					pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
					pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

					pMulRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
					pMulRS->m_DestBlend		= 2;			//D3DBLEND_ONE;
					pMulRS->m_Cull			= 1;				//D3DCULL_NONE;
					pMulRS->m_AlpOP_S0		= 4;				//D3DTOP_MODULATE;
					pMulRS->m_LightEnable	= pProp->m_bLightEnable;
					pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
					pRS->m_ZWrite				= pProp->m_eZTest != Z_TESTONLY;

					pRS->m_SrcBlend = 9;				//D3DBLEND_DESTCOLOR;
					pRS->m_DestBlend = BLEND_ONE;				//D3DBLEND_ONE;
				}
			}


			if( !bRenderTerrain )
			{
				pVB[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, Color, 0.0f, 1.0f );
				pVB[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, Color, 0.0f, 0.0f );
				pVB[2].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, Color, 1.0f, 1.0f );
				pVB[3].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, Color, 1.0f, 0.0f );

				if(pProp->m_isShowFrame)
				{
					pVBframe[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
					pVBframe[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, ColorFrame, 0.0f, 0.0f );
					pVBframe[2].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, ColorFrame, 1.0f, 0.0f );
					pVBframe[3].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, ColorFrame, 1.0f, 1.0f );
					pVBframe[4].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
				}
			}
		}
		break;
	case SPRF_TERRAIN: //绘制朝向地表
	case SPRF_TERRAIN_ROTATE:
		{
			//ILogicHandler* pHandler = NULL;
			//if(m_pLogicHandler)
			//	pHandler = m_pLogicHandler;//pProp->m_pEffectClassDesc->GetEffectHandler();
			float fUnitSize = float(eGridSpan);
			CMatrix Rotate;
			if( pProp->m_AngularVelocity > 0.00001f || pProp->m_fStartAngle > 0.00001f )
			{
				if( pProp->m_fStartAngle > 0.00001f )
					m_CurAngle = pProp->m_fStartAngle;
				else
					m_CurAngle += pProp->m_AngularVelocity * m_DeltaTime* 0.001f;

				//m_CurAngle = 0.0f;
				if( m_CurAngle > 6.283f )
					m_CurAngle = 0.0f;
				if( m_CurAngle < 0.0f )
					m_CurAngle = 6.283f;
				CVector3f vecRotate;
				vecRotate = pProp->m_vecRotate;
				vecRotate.y = -vecRotate.y;

				pProp->m_pEffectClassDesc->SetRotate( Rotate, vecRotate/pProp->m_AngularVelocity, m_CurAngle );
			}
			
			CVector3f Pos = m_matWorld.m[3];
			fScale = fScale * 1.732f * m_matWorld._22;
			pos[0].Init( Pos.x - fScale, Pos.y, Pos.z - fScale );
			pos[1].Init( Pos.x - fScale, Pos.y, Pos.z + fScale );
			pos[2].Init( Pos.x + fScale, Pos.y, Pos.z - fScale );
			pos[3].Init( Pos.x + fScale, Pos.y, Pos.z + fScale );

			// 计算出光环矩形内各个点的坐标
			CFRect rtUnit		= CFRect( pos[0].x/fUnitSize, pos[0].z/fUnitSize, pos[2].x/fUnitSize, pos[1].z/fUnitSize );
			CIRect rtSprite		= CIRect( int32(floor( rtUnit.left )), int32(floor( rtUnit.top )),
											int32(floor( rtUnit.right ) + 1), int32(floor( rtUnit.bottom ) + 1) );
			int32 nWidth		= rtSprite.Width();
			int32 nHeight		= rtSprite.Height();
			float fSize			= fScale*2;		
			int32 nTotalPos		= ( nWidth + 1 )*( nHeight + 1);	// 计算顶点数目		
			int32 nTotalPrim	= nWidth*nHeight*2;					// 计算面片数目

			if( !nTotalPrim || fScale == 0.0f )
				return Result;

			VerColorTex* pVB;
			VerNCT* pVBframe;
			uint16 * pIB;
			RenderState* pRS;
			CMatrix *pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLELIST, nTotalPos, 
				nTotalPrim, VerColorTex::Format, sizeof(VerColorTex), (void**)&pVB, (void**)&pIB, (void**)&pRS );

			pRS->m_TexOP_S0		= 2 + pProp->m_eTextOp;
			CVector3f Offset = OrgOffset;
			Offset.Rotate( m_matWorld );
			pMatrix->SetTranslate(Offset);


			if(pProp->m_isShowFrame)
			{
				CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, NULL, NULL, PT_LINESTRIP, 5, 4, 
					VerNCT::Format, sizeof(VerNCT), (void**)&pVBframe, NULL, (void**)&pRS );
				pVBframe[0].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
				pVBframe[1].Set( pos[1].x, pos[1].y, pos[1].z, 0, 0, 1, ColorFrame, 0.0f, 0.0f );
				pVBframe[2].Set( pos[3].x, pos[3].y, pos[3].z, 0, 0, 1, ColorFrame, 1.0f, 0.0f );
				pVBframe[3].Set( pos[2].x, pos[2].y, pos[2].z, 0, 0, 1, ColorFrame, 1.0f, 1.0f );
				pVBframe[4].Set( pos[0].x, pos[0].y, pos[0].z, 0, 0, 1, ColorFrame, 0.0f, 1.0f );
			}
			pRS->m_Cull				= CULLT_NONE;				//D3DCULL_NONE;
			pRS->m_AlpOP_S0			= TOP_MODULATE;				//D3DTOP_MODULATE;
			pRS->m_LightEnable		= pProp->m_bLightEnable;
			pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
			pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
			pRS->m_AlphaBlendEnable = pProp->m_eBlendModel != SPRB_NONE;
			pRS->m_DestBlend		= pProp->m_eBlendModel == SPRB_ADD ? 2 : pRS->m_DestBlend;
			pRS->m_UAddr_S0			= TADDRESS_CLAMP;
			pRS->m_VAddr_S0			= TADDRESS_CLAMP;
			int32 m = 0;

			CVector3f VectorUV;
			//static CVector3f Trans(0.5f,0.0f,0.5f);
			if(SPRF_TERRAIN_ROTATE == pProp->m_eFaceType)
			{
				CMatrix temp;
				temp.SetWithColumn( m_matWorld.GetAxis(0).Normalize(),
					m_matWorld.GetAxis(1).Normalize(),
					m_matWorld.GetAxis(2).Normalize() );
				Rotate *= temp;
			}

			for( int h = rtSprite.top; h <= rtSprite.bottom; ++h )
			{
				for( int i = rtSprite.left; i <= rtSprite.right; ++i, ++m )
				{
					pVB[m].p.x = i*fUnitSize;
					pVB[m].p.z = h*fUnitSize;
					pVB[m].diffuse = Color;

					VectorUV.x = (pVB[m].p.x - pos[0].x)/fSize;
					VectorUV.y = 0;
					VectorUV.z = (pVB[m].p.z - pos[0].z)/fSize;


					VectorUV = VectorUV - m_vecRotateCenter;
					VectorUV.Rotate(Rotate);
					VectorUV = VectorUV + m_vecRotateCenter;
					pVB[m].tu = VectorUV.x ;
					pVB[m].tv = VectorUV.z;

					pVB[m].p.y = m_pLogicHandler ? m_pLogicHandler->GetTerrainHeight( pVB[m].p.x + Offset.x, pVB[m].p.z + Offset.z ) : pos[0].y;
					pVB[m].p.y += SPRITE_HIGH;
				}
			}

			int n = 0, k = 0, nWidthA = nWidth + 1;
			bool isBackGrid;
			for( int32 v = 0; v < nHeight; ++v )
			{
				isBackGrid = m_pLogicHandler ? m_pLogicHandler->IsBackGrid(pVB[n].p) : false;
				for( int32 u = 0; u < nWidth; ++u, ++n )
				{
					if (isBackGrid)	// |\|
					{
						// 第一个三角形
						pIB[k++] = (uint16)n;
						pIB[k++] = (uint16)(n+nWidthA);
						pIB[k++] = (uint16)(n+1);
						// 第二个三角形
						pIB[k++] = (uint16)(n+1);
						pIB[k++] = (uint16)(n+nWidthA);
						pIB[k++] = (uint16)(n+nWidthA+1);
					}
					else	// |/|
					{
						// 第一个三角形
						pIB[k++] = (uint16)n;
						pIB[k++] = (uint16)(n+nWidthA);
						pIB[k++] = (uint16)(n+nWidthA+1);
						// 第二个三角形
						pIB[k++] = (uint16)(n);
						pIB[k++] = (uint16)(n+nWidthA+1);
						pIB[k++] = (uint16)(n+1);
					}
					isBackGrid = !isBackGrid;
				}
				++n;
			}
		}
		break;

	default:
		break;
	}
	return Result;
}

void CSprite::Free()
{
	if( IsInRenderTime() && m_FrameTime!= 0 && m_pEffectUnitProp->m_bSlownDisappear )
	{
		m_pEffectUnitProp->m_pEffectClassDesc->AddLeavingsEffectUnit( this );
	}
	else
		delete this;
}

const EFFECT_RESULT	CSprite::RenderEx( const CMatrix& matWorld,uint32 uCurTime,  RenderObjStyle* pRORS)
{
	uCurTime += m_DistortTime;
	m_DeltaTime = uCurTime>m_CurTime?(uCurTime - m_CurTime):0.0f;
	m_CurTime = uCurTime;
	m_FrameTime += m_DeltaTime;

	if(!IsInRenderTime())
		m_FrameTime = float(m_pEffectUnitProp->m_TimeRange + m_DelayTime);
	RenderEx( m_ParentMat , pRORS );

	m_DelayRender -= m_DeltaTime;
	return EFFECT_RESULT( NULL, true, false , m_DelayRender <= 0.0f || m_DelayRender > (float)((CSpriteProp*)m_pEffectUnitProp)->m_DeleteTime );
}
