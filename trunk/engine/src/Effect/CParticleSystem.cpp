#include "stdafx.h"
#include "CParticleSystem.h"
#include "TimeHelper.h"
#include "CRenderTarget.h"
#include "CEffect.h"
#include "CWindowTarget.h"
#include "CRenderPipeline.h"
#include "TSqrAllocator.inl"
#include "CCamera.h"
#include "CColor.inl"
DYNAMIC_CREATE_EFFECTUNITBASE( eParticleSystem, IDS_PARTICLESYSTEM, CParticleSystem, CParticleSystemProp )

enum
{
	PARTICLESYSTEM_ORIGIN,
	PARTICLESYSTEM_ADD_TEXTURE_DIR, 
	PARTICLESYSTEM_ADD_INISPEEDPERCENT,
	PARTICLESYSTEM_ADD_LOCALSKELETAL,
	PARTICLESYSTEM_ADD_ZIBAS,
	PARTICLESYSTEM_ADD_BIANCHANG_BILV,
	PARTICLESYSTEM_ADD_SCALE,
	PARTICLESYSTEM_ADD_CLIPPLANE,
	PARTICLESYSTEM_CHANGE_CLIP_DIST,
	PARTICLESYSTEM_EMISSION_MINMAXRADIUS,
	PARTICLESYSTEM_TEXOP_FLAG,
};



const float			invMaxRand = 1.0f / 32768.0f;
const CVector3f		Fix_Unit_Y = CVector3f::UNIT_Y + CVector3f(0.00001f,-0.00001f,0.00001f);

CParticleSystemProp::CParticleSystemProp( uint32 ID, IEffectClassDesc* pEffectClassDesc ) 
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_nEmissionInterval			= 10;		
	m_nParticlesPerEmission		= 15;	
	m_fMinSize					= 0;
	m_fMaxSize					= 1;
	m_vEmissionPlan				= CVector3f( 0,  1, 0 );
	m_vMinVelocity				= CVector3f( 0, 70, 0 );
	m_vMaxVelocity				= CVector3f( 0, 70, 0 );
	m_vAcceleration				= CVector3f( 0,  0, 0 );
	m_fMinAngularVelocity		= 0;	
	m_fMaxAngularVelocity		= 0;	
	m_nMinLifeSpan				= 200;	
	m_nMaxLifeSpan				= 300;	
	m_fMinRadius				= 0;	
	m_fMaxRadius				= 0;
	m_fLongShortRadius			= 1.0f;
	m_fCornerDivision			= 3.0f;
	m_fVelocityPercent			= 0;
	m_eFollowType				= PSPC_Free;	
	m_eEmissionType				= PSEMS_eSphere;
	m_eVelType					= PSVEL_eFROMCENTER;	
	m_eTextDir					= PSTD_eDirCamera;
	m_fWidthRatio				= 1.0f;
	m_fZBias					= 0.0f;
	m_fLongShortDRadius			= 1.0f;
	m_eBlendModel				= SPRB_ADD;
	m_eTextOp					= SPRO_MODULATE;
	m_eZTest					= Z_TESTONLY;
	m_bLightEnable				= FALSE;

	m_vecOffset[0.0f] = CVector3f( 0, 0, 0 );
	m_TextName[0.0f] = "None";
	m_Color[0.0f] = (uint32)0xffff0000;
	m_Color[1.0f] = (uint32)0xffcc8800;
	m_Scale[0.0f] = (BYTE)( ((uint32)0xff000000)>>24 );
	m_Scale[1.0f] = (BYTE)( ((uint32)0xff000000)>>24 );
	m_XYZcale[0.0f] = CVector3f( 1.0f, 1.0f, 1.0f);
	m_EmissionMinRadius[0.0f] = 0.0f;
	m_EmissionMaxRadius[0.0f] = 0.0f;
	LoadFullRes();
}

CParticleSystemProp::~CParticleSystemProp(void)
{
}

int32 CParticleSystemProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	m_eTextDir											= PSTD_eDirCamera;
	switch( Version )
	{
	case PARTICLESYSTEM_TEXOP_FLAG:
		File.read( (char*)&m_eTextOp,			sizeof(SPR_BLENDOP) );
	case PARTICLESYSTEM_EMISSION_MINMAXRADIUS:
		LoadMapValue( File, m_EmissionMinRadius );
		LoadMapValue( File, m_EmissionMaxRadius );

	case PARTICLESYSTEM_CHANGE_CLIP_DIST:
		LoadMapValue(File, m_ClipDistMap);
	case PARTICLESYSTEM_ADD_CLIPPLANE:
		File.read(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
		//if (m_ClipPlane.originPlane.IsValid() && m_ClipDistMap.empty())
		//{
		//	m_ClipDistMap[0.0f] = -m_ClipPlane.originPlane.m_Dist;
		//	m_ClipDistMap[1.0f] = -m_ClipPlane.originPlane.m_Dist;
		//}
	case PARTICLESYSTEM_ADD_SCALE:
		LoadMapValue( File, m_XYZcale );
	case PARTICLESYSTEM_ADD_BIANCHANG_BILV:
		File.read( (char*)&m_fLongShortRadius,			sizeof(float) );
		File.read( (char*)&m_fCornerDivision,			sizeof(float) );
		File.read( (char*)&m_fLongShortDRadius,			sizeof(float) );
	case PARTICLESYSTEM_ADD_ZIBAS:
		File.read( (char*)&m_fZBias,					sizeof(float) );
	case PARTICLESYSTEM_ADD_LOCALSKELETAL:
		File.read( (char*)&m_sRefSkelatal,				sizeof(IDNAME) );
		File.read( (char*)&m_fWidthRatio,				sizeof(float) );
	case PARTICLESYSTEM_ADD_INISPEEDPERCENT: 
		File.read( (char*)&m_fVelocityPercent,			sizeof(float) );		
	case PARTICLESYSTEM_ADD_TEXTURE_DIR:
		File.read( (char*)&m_eTextDir,					sizeof(ePS_TextureDir) );
	default:	
		File.read( (char*)&m_nEmissionInterval,		sizeof(int32) );
		File.read( (char*)&m_nParticlesPerEmission,	sizeof(int32) );
		File.read( (char*)&m_fMinSize,					sizeof(float) );
		File.read( (char*)&m_fMaxSize,					sizeof(float) );
		File.read( (char*)&m_vEmissionPlan,			sizeof(CVector3f) );
		File.read( (char*)&m_vMinVelocity,				sizeof(CVector3f) );
		File.read( (char*)&m_vMaxVelocity,				sizeof(CVector3f) );
		File.read( (char*)&m_vAcceleration,			sizeof(CVector3f) );
		File.read( (char*)&m_fMinAngularVelocity,		sizeof(float) );
		File.read( (char*)&m_fMaxAngularVelocity,		sizeof(float) );
		File.read( (char*)&m_nMinLifeSpan,				sizeof(int32) );
		File.read( (char*)&m_nMaxLifeSpan,				sizeof(int32) );
		File.read( (char*)&m_fMinRadius,				sizeof(float) );
		File.read( (char*)&m_fMaxRadius,				sizeof(float) );
		File.read( (char*)&m_eFollowType,				sizeof(ePS_ParticlCoord) );
		File.read( (char*)&m_eEmissionType,			sizeof(ePS_EMSType) );
		File.read( (char*)&m_eVelType,					sizeof(ePS_VELType) );
		File.read( (char*)&m_eBlendModel,				sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,					sizeof(Z_TEST) );
		File.read( (char*)&m_bLightEnable,				sizeof(BOOL) );
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	if( Version < PARTICLESYSTEM_EMISSION_MINMAXRADIUS )
	{
		m_EmissionMinRadius[0.0f] = m_fMinRadius;
		m_EmissionMaxRadius[0.0f] = m_fMaxRadius;
	}

	return 0;
}

void  CParticleSystemProp::_UpdateParams(void)
{
	m_nEmissionInterval		= max( 0, m_nEmissionInterval );	
	m_nParticlesPerEmission	= max( 0, m_nParticlesPerEmission );
	m_vEmissionPlan			= m_vEmissionPlan.Normalize();	
	m_nMinLifeSpan			= max( 0, m_nMinLifeSpan );
	m_nMaxLifeSpan			= max( m_nMinLifeSpan + 1, m_nMaxLifeSpan );
}

int32 CParticleSystemProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = PARTICLESYSTEM_TEXOP_FLAG;	
	File.write( (char*)&Version,					sizeof(uint32) );
	
	File.write( (char*)&m_eTextOp,					sizeof(SPR_BLENDOP) );

	SaveMapValue(File, m_EmissionMinRadius);
	SaveMapValue(File, m_EmissionMaxRadius);

	SaveMapValue(File, m_ClipDistMap);
	File.write(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
	SaveMapValue( File, m_XYZcale );
	File.write( (char*)&m_fLongShortRadius,			sizeof(float) );
	File.write( (char*)&m_fCornerDivision,			sizeof(float) );
	File.write( (char*)&m_fLongShortDRadius,		sizeof(float) );
	File.write( (char*)&m_fZBias,					sizeof(float) );
	File.write( (char*)&m_sRefSkelatal,				sizeof(IDNAME) );
	File.write( (char*)&m_fWidthRatio,				sizeof(float) );
	File.write( (char*)&m_fVelocityPercent,			sizeof(float) );
	File.write( (char*)&m_eTextDir,					sizeof(ePS_TextureDir) );
	File.write( (char*)&m_nEmissionInterval,		sizeof(int32) );
	File.write( (char*)&m_nParticlesPerEmission,	sizeof(int32) );
	File.write( (char*)&m_fMinSize,					sizeof(float) );
	File.write( (char*)&m_fMaxSize,					sizeof(float) );
	File.write( (char*)&m_vEmissionPlan,			sizeof(CVector3f) );
	File.write( (char*)&m_vMinVelocity,				sizeof(CVector3f) );
	File.write( (char*)&m_vMaxVelocity,				sizeof(CVector3f) );
	File.write( (char*)&m_vAcceleration,			sizeof(CVector3f) );
	File.write( (char*)&m_fMinAngularVelocity,		sizeof(float) );
	File.write( (char*)&m_fMaxAngularVelocity,		sizeof(float) );
	File.write( (char*)&m_nMinLifeSpan,				sizeof(int32) );
	File.write( (char*)&m_nMaxLifeSpan,				sizeof(int32) );
	File.write( (char*)&m_fMinRadius,				sizeof(float) );
	File.write( (char*)&m_fMaxRadius,				sizeof(float) );
	File.write( (char*)&m_eFollowType,				sizeof(ePS_ParticlCoord) );
	File.write( (char*)&m_eEmissionType,			sizeof(ePS_EMSType) );
	File.write( (char*)&m_eVelType,					sizeof(ePS_VELType) );
	File.write( (char*)&m_eBlendModel,				sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,					sizeof(Z_TEST) );
	File.write( (char*)&m_bLightEnable,				sizeof(BOOL) );

	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}

void CParticleSystemProp::_CalculateAABB()
{
	// 计算出轨迹大小
	CEffectUnitProp::_CalculateAABB();
	CVector3f vecMax = m_MaxAABB.getMaximum();
	CVector3f vecMin = m_MaxAABB.getMinimum();

	// 速度 声明周期大小
	float max_f = 0.0f;
	if( m_eVelType == CParticleSystemProp::PSVEL_eRANDOM || m_eVelType == CParticleSystemProp::PSVEL_eFROMCENTER )
	{
		//按照设计的方向发射
		float v = abs(m_vMaxVelocity.x);
		v = max(abs(m_vMaxVelocity.y), v);	v = max(abs(m_vMaxVelocity.z), v);
		max_f = v * m_nMaxLifeSpan / 1000.0f;
		if (m_nEmissionInterval > 1.0f)
		{
			float zoom1 = m_nParticlesPerEmission*m_nMaxLifeSpan*1.0f/m_nEmissionInterval;
			if (zoom1 < 10.0f)
				zoom1 = 0.6f;
			else
				zoom1 = max(0.6f, (zoom1 - 10) / zoom1);
			float zoom2 = 0.6f;
			if (m_fMaxSize > 1.0f)
				zoom2 = (m_fMaxSize - 1)/m_fMaxSize;zoom2 = max(0.6f, zoom2);
			zoom1 = (zoom1 + zoom2)/2.0f;
			max_f *= zoom1;
		}
		
	}
	else
		max_f = m_fMaxRadius;
	if (m_eEmissionType == CParticleSystemProp::PSEMS_eCircle )
	{
		CVector3f dir = m_vEmissionPlan;
		dir.normalize();
		dir = dir * max_f;
		if (dir.x > 0)
		{
			vecMax.x += dir.x;
			vecMin.x -= 1;
		}
		else
		{
			vecMax.x += 1;
			vecMin.x += dir.x;
		}
		if (dir.y > 0)
		{
			vecMin.y -= 1;
			vecMax.y += dir.y;
		}
		else
		{
			vecMin.y += dir.y;
			vecMin.y -= 1;
		}
		if (dir.z > 0)
		{
			vecMin.z -= 1;
			vecMax.z += dir.z;
		}
		else
		{
			vecMin.z += dir.z;
			vecMin.z -= 1;
		}
	}
	else
	{
		vecMax.x += max_f;	vecMax.y += max_f;	vecMax.z += max_f;
		vecMin.x -= max_f;	vecMin.y -= max_f;	vecMin.z -= max_f;
	}

	m_MaxAABB.setExtents(vecMin, vecMax);
}

//////////////////////////////////////////////////////////////////////////

CParticleSystem::CParticleSystem( IEffectUnitProp* pEffectUnitProp ) 
				: CEffectUnit( pEffectUnitProp )
				, m_nTotalParticleNum(0)
				, m_nRefSkeletal(-1)
{
	SQR_TRY
	{
		m_RemainTime				= 0;
		const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;
		m_it = pProp->m_pEffectClassDesc->InsertParticleSys( this );
		SetProp( m_pEffectUnitProp );

		float fMinRadius = 0.0f, fMaxRadius = 0.0f;
		pProp->GetCurAdvanceProp(0.0, fMinRadius, fMaxRadius);
		//m_vecScale +=  CVector3f( fMaxRadius*2, fMaxRadius*2, fMaxRadius*2 );
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

CParticleSystem::~CParticleSystem(void)
{
	const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;

	for( EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.begin(); 
		itParticle != m_ParticleOrderByTexture.end(); ++itParticle )
	{
		Particle* pParticle = itParticle->second.pParticle;
		while( pParticle )
		{
			Particle* Temp = pParticle->m_Next;
			pProp->m_pEffectClassDesc->FreeParticle( pParticle );
			pParticle = Temp;
		}
	}
	pProp->m_pEffectClassDesc->RemoveParticleSys( m_it );
}

bool CParticleSystem::SetProp( IEffectUnitProp* pEffectUnitProp )
{
	if( !CEffectUnit::SetProp( pEffectUnitProp ) )
		return false;
	const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;

	EMap< ITexture*, ParticleList > NewParticle;
	for( EMap<float,ITexture*>::const_iterator it = pProp->m_Texture.begin(); it != pProp->m_Texture.end(); ++it )
	{
		EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.find( it->second );
		if( itParticle == m_ParticleOrderByTexture.end() )
			NewParticle[it->second].pParticle = NULL;
		else
		{
			NewParticle[it->second] = itParticle->second;
			itParticle->second.pParticle = NULL;
		}
	}

	for( EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.begin(); 
		itParticle != m_ParticleOrderByTexture.end(); ++itParticle )
	{
		Particle* pParticle = itParticle->second.pParticle;
		while( pParticle )
		{
			Particle* Temp = pParticle->m_Next;
			pProp->m_pEffectClassDesc->FreeParticle( pParticle );
			pParticle = Temp;
		}
	}
	m_ParticleOrderByTexture = NewParticle;

	pProp->m_ClipPlane.RenderDebug();
	return true;
}

void CParticleSystem::OnLinked( CRenderable* pParentObj )
{
	const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;

	if( pParentObj->GetType() != ROT_MODEL )
		return; 

	if( pProp->m_sRefSkelatal[0] == 0 )
		m_nRefSkeletal = -1;
	else
	{
		CEffectUnit::OnLinked(pParentObj);
		CSkeletalFrame* pSkeFrame = ( (CModel*)m_pParent )->GetSketetalFrame();
		if( pSkeFrame )
			m_nRefSkeletal = (int16)pSkeFrame->GetSkeletalIDByName( pProp->m_sRefSkelatal );
		else
			m_nRefSkeletal = 0;
	}
}

void CParticleSystem::OnUnlinked( void )
{
	m_pParent = NULL;
	m_nRefSkeletal = -1;
	m_matRefence.Identity();
}

const CMatrix& CParticleSystem::GetMatrix()	
{ 
	static CMatrix mat;
	mat = m_matWorld*m_matRefence;
	return mat;
}

const EFFECT_RESULT CParticleSystem::RenderEx( const CMatrix& matWorld , RenderObjStyle* pRORS )
{
	CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp; 

	EFFECT_RESULT Result( NULL, true, false );
	
	CMatrix matRender = matWorld;
	CMatrix matPre = m_matWorld;

	CVector3f vecDir, XYZScale;
	CMatrix matBillboard,matView;
	CCamera* pCamera = NULL;
	if ( CGraphic::GetInst()->GetActiveRenderTarget() )
		pCamera = CGraphic::GetInst()->GetActiveRenderTarget()->GetCamera();
	else	
		pCamera = CMainWindowTarget::GetInst()->GetCamera();


	//CGraphic::GetInst()->GetCamera( &pCamera );
	pCamera->GetBillboardAll( matBillboard );
	matView = pCamera->getViewMatrix();
	vecDir = pCamera->getDIRECTION();//pCamera->GetCameraDir( vecDir );

	CVector3f xCamera =pCamera->getRIGHT(); 
	CVector3f yCamera =pCamera->getUP();

	float Percent = GetUnitPercent();

	if( GetRefCount() )
	{
		if( m_nRefSkeletal >= 0 )
		{
			( (CModel*)m_pParent )->GetMatrix( m_matRefence, m_nRefSkeletal, uint32(-1) );
			if( pProp->m_eUpdatePos == UP_SOCKETOFFSET || pProp->m_eUpdatePos == UP_NODEOFFSET )
			{
				CVector3f v( m_matRefence._11, m_matRefence._12, m_matRefence._13 );
				m_matRefence._11 = m_matRefence._22 = m_matRefence._33 = v.Mag();
				m_matRefence._12 = m_matRefence._13 = m_matRefence._14 = 0.0f;
				m_matRefence._21 = m_matRefence._23 = m_matRefence._24 = 0.0f;
				m_matRefence._31 = m_matRefence._32 = m_matRefence._34 = 0.0f;
				m_matRefence._44 = 1.0f;
			}
			CMatrix matInvRef = m_matRefence;
			if( pProp->m_eUpdatePos != UP_SOCKETOFFSET && pProp->m_eUpdatePos != UP_NODEOFFSET )
				matInvRef.InvertFast();
			else
			{
				matInvRef._11 = matInvRef._22 = matInvRef._33 = 1.0f/matInvRef._11; 
				matInvRef._41 = -matInvRef._41;
				matInvRef._42 = -matInvRef._42;
				matInvRef._43 = -matInvRef._43;
			}
			matRender = matRender*matInvRef;


			xCamera.Rotate( matInvRef );
			yCamera.Rotate( matInvRef );
			vecDir.Rotate( matInvRef );
		}

		XYZScale = GetMapValue( pProp->m_XYZcale, Percent );

		CVector3f Offset( 0, 0, 0 );
		if(pProp->m_vecOffset.size())
			Offset = GetMapValue( pProp->m_vecOffset, Percent );

		Offset.FastMultiply( matRender );

		if(0.0f<m_DeltaTime)
		{
			m_vecOffsetVelocity = Offset - CVector3f( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
			m_vecOffsetVelocity *= 1000*pProp->m_fVelocityPercent/m_DeltaTime;
		}

		m_matWorld = matRender;  
		m_matWorld._41 = Offset.x;
		m_matWorld._42 = Offset.y;
		m_matWorld._43 = Offset.z;
	}
	else
	{
		XYZScale = GetMapValue( pProp->m_XYZcale, 1.0f );
	}

	UpdateParticle( m_CurTime, matPre, true );

	CVector3f vecS( matRender._11 + matRender._12 + matRender._13, 
					matRender._21 + matRender._22 + matRender._23,
					matRender._31 + matRender._32 + matRender._33 );
	float fWordScale = vecS.Mag();

	float zbias = 0.0f;
	if ( pProp->m_fZBias != 0.0f )
		zbias = CalculateDifZBias( pProp->m_fZBias, m_matWorld, pCamera->getViewMatrix(),
		pCamera->getFarDist(), pCamera->getNearDist() );


	CPlane plane;
	if (!pProp->m_ClipDistMap.empty())
	{
		pProp->m_ClipPlane.originPlane.m_Dist = -GetMapValue(pProp->m_ClipDistMap, Percent);
		pProp->m_ClipPlane.SetTransform(matWorld);
		plane = pProp->m_ClipPlane.transformedPlane;
	}

	CVector3f p1,p2,p3,p4;
	uint32 m;
	CVector3f v1,v2,vDir;
	CMatrix matRot;
	uint32 Color;
	float fScale;
	float xScale;
	float yScale;
	CVector3f Pos;

	for( EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.begin(); 
		itParticle != m_ParticleOrderByTexture.end(); ++itParticle )
	{
		if( !itParticle->second.nParticleNum )
			continue;
		Result.m_UnitbEmpty = false;

		VerColorTex* pVB;
		WORD* pIB;

		STATIC_RS* pRS;

		CMatrix* pMatrix;
		if(pRORS)
		pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, itParticle->first, NULL, PT_TRIANGLELIST, 
			itParticle->second.nParticleNum*4, itParticle->second.nParticleNum*2, VerColorTex::Format, sizeof(VerColorTex),
			(void**)&pVB, (void**)&pIB, (void**)&pRS );
		else
		pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, itParticle->first, NULL, PT_TRIANGLELIST, 
			itParticle->second.nParticleNum*4, itParticle->second.nParticleNum*2, VerColorTex::Format, sizeof(VerColorTex),
			(void**)&pVB, (void**)&pIB, (void**)&pRS );
		pRS->m_TexOP_S0		= pProp->m_eTextOp + 2;
		pRS->m_Cull			= CULLT_NONE;				//D3DCULL_NONE;
		pRS->m_AlpOP_S0		= TOP_MODULATE;				//D3DTOP_MODULATE;
		pRS->m_LightEnable	= pProp->m_bLightEnable;
		pRS->m_ZTestEnable	= pProp->m_eZTest != Z_NONE;
		pRS->m_ZWrite		= pProp->m_eZTest != Z_TESTONLY;
		pRS->m_fZBias		= zbias;

		if( pProp->m_eBlendModel == SPRB_NONE )
			pRS->m_AlphaBlendEnable = FALSE;
		else
		{
			pRS->m_AlphaBlendEnable = TRUE;
			if( pProp->m_eBlendModel != SPRB_MULTIPLY )
				pRS->m_DestBlend = BLEND_ONE;			//D3DBLEND_ONE;
		}
		pRS->m_ClipPlane =plane;
		*pMatrix = m_matRefence;

		float fHPerW = pProp->m_fWidthRatio;
		if( fHPerW == 1.0f && itParticle->first )
			fHPerW = float( itParticle->first->GetHeight() )/itParticle->first->GetWidth();

		Particle* pParticle = itParticle->second.pParticle;

		for( uint32 n = 0; pParticle; ++n )
		{
			float Percent = 0.0f;
			if ( pParticle->m_LifeSpan - 0.0001f > 0.0f )
				Percent =1.0f - pParticle->m_LeftLife / pParticle->m_LifeSpan;
			pParticle->m_LeftLife	 -= m_DeltaTime;
			
			Color = GetMapValue( pProp->m_Color, Percent );
			fScale = GetMapValue( pProp->m_Scale, Percent )/255.0f;
			xScale = ( pProp->m_fMinSize + ( pProp->m_fMaxSize - pProp->m_fMinSize )*fScale )*fWordScale;
			yScale = xScale*fHPerW;
			xScale *= XYZScale.x;
			yScale *= XYZScale.y;

			Pos = pParticle->m_CurPos;

			switch(pProp->m_eFollowType)
			{
			case CParticleSystemProp::PSPC_Follow:
			case CParticleSystemProp::PSPC_Static:
				Pos.x += m_matWorld._41;
				Pos.y += m_matWorld._42;
				Pos.z += m_matWorld._43;
				break;
			case CParticleSystemProp::PSPC_Normal:
				{
					CMatrix NormGetMat;
					NormGetMat.SetRotateX(-CMath::half_pi);
					NormGetMat *= m_matWorld;
					Pos.FastMultiply(NormGetMat);
				}	
				break;
			default:
				break;
			}

			switch(pProp->m_eTextDir)
			{
			case CParticleSystemProp::PSTD_eDirVelocity:
				v1 = pParticle->m_CurSpd;
				v1.Normalize();
				v2 = v1.Cross( vecDir );
				v1 *= xScale;
				v2 *= yScale;
				vDir = v1.Cross(v2);
				break;
			case CParticleSystemProp::PSTD_eDirFaceToVelocity:
				vDir = pParticle->m_CurSpd;
				vDir.Normalize();
				v1 = vDir.Cross( Fix_Unit_Y );
				v1.Normalize();
				v2 = v1.Cross( vDir );
				v1 *= xScale;
				v2 *= yScale;
				break;
			case CParticleSystemProp::PSTD_eDirAlwaysUP:
				vDir = pParticle->m_CurSpd;
				v1 = vDir.Cross( Fix_Unit_Y );
				v1.Normalize();
				v2 = v1.Cross( Fix_Unit_Y  );
				v1 *= xScale;
				v2 *= yScale;
				vDir = CVector3f::UNIT_Y ;
				break;

			case CParticleSystemProp::PSTD_eDirUpUpUp:
				v1 = pParticle->m_CurSpd;
				v1.Normalize();
				v2 = v1.Cross( Fix_Unit_Y );
				v2.Normalize();
				v1 *= xScale;
				v2 *= yScale;
				vDir = CVector3f::UNIT_Y;
				break;
			case CParticleSystemProp::PSTD_eDirCamera:
			default:
				v1		= xCamera * xScale;
				v2		= yCamera * yScale;
				vDir	= vecDir;
				break;
			}	

			p1 = ( -v1 - v2 );
			p2 = ( -v1 + v2 );
			p3 = (  v1 - v2 );
			p4 = (  v1 + v2 );

			if(pParticle->m_CurAngular > 0.01f)
			{
				matRot.SetRotation( vDir, -pParticle->m_CurAngular );

				p1.Rotate( matRot );
				p2.Rotate( matRot );	
				p3.Rotate( matRot );	
				p4.Rotate( matRot );
			}

			m  = n<<2;

			pVB[ m + 0 ].Set( p1.x + Pos.x, p1.y + Pos.y, p1.z + Pos.z, Color, 0.0f, 1.0f );
			pVB[ m + 1 ].Set( p2.x + Pos.x, p2.y + Pos.y, p2.z + Pos.z, Color, 0.0f, 0.0f );
			pVB[ m + 2 ].Set( p3.x + Pos.x, p3.y + Pos.y, p3.z + Pos.z, Color, 1.0f, 1.0f );
			pVB[ m + 3 ].Set( p4.x + Pos.x, p4.y + Pos.y, p4.z + Pos.z, Color, 1.0f, 0.0f );

			pIB[ n*6 + 0 ] = WORD( m ) + 0;
			pIB[ n*6 + 1 ] = WORD( m ) + 1;
			pIB[ n*6 + 2 ] = WORD( m ) + 2;
			pIB[ n*6 + 3 ] = WORD( m ) + 2;
			pIB[ n*6 + 4 ] = WORD( m ) + 1;
			pIB[ n*6 + 5 ] = WORD( m ) + 3;

			pParticle = pParticle->m_Next;
		}
	}
	return Result;
}

void CParticleSystem::UpdateParticle( uint32 CurTime, const CMatrix& matPre, bool bAlloc )
{
	const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;

	float DeltaTime = m_DeltaTime * 0.001f;//1000.0f;

	//for( EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.begin(); 
	//	itParticle != m_ParticleOrderByTexture.end(); ++itParticle )
		
	m_nTotalParticleNum = 0;

	for( EMap< ITexture*, ParticleList >::iterator itParticle = m_ParticleOrderByTexture.begin(); 
			itParticle != m_ParticleOrderByTexture.end(); ++itParticle )
	{
		itParticle->second.nParticleNum = 0;
		Particle* pParticle = itParticle->second.pParticle;
		Particle** ppCurPar = &itParticle->second.pParticle;
		while( pParticle )
		{
			float Percent = 1.0f;
			if ( pParticle->m_LifeSpan - 0.0001f > 0.0f )
				Percent =1.0f - pParticle->m_LeftLife / pParticle->m_LifeSpan;
			
			if( Percent > 1.0f )
			{
				*ppCurPar = pParticle->m_Next;
				pProp->m_pEffectClassDesc->FreeParticle( pParticle );
				pParticle = *ppCurPar;
			}
			else
			{	
				ITexture* pTexture = GetMapState( pProp->m_Texture, Percent*pProp->m_nTextureCircle - (int32)( Percent*pProp->m_nTextureCircle ) );	
				if( pTexture <= itParticle->first )
				{
					pParticle->m_CurAngular += pParticle->m_AngularVelocity*DeltaTime;
					pParticle->m_CurPos		+= pParticle->m_CurSpd*DeltaTime;
					pParticle->m_CurSpd		+= pProp->m_vAcceleration*DeltaTime;
					if( pParticle->m_CurAngular > 6.283f )
						pParticle->m_CurAngular = 0.0f;
					if( pParticle->m_CurAngular < 0 )
						pParticle->m_CurAngular = 6.283f;
				}				
				if( pTexture != itParticle->first )
				{
					*ppCurPar = pParticle->m_Next;
					EMap< ITexture*, ParticleList >::iterator it = m_ParticleOrderByTexture.find( pTexture );
					pParticle->m_Next = it->second.pParticle;
					it->second.pParticle = pParticle;
					if( pTexture < itParticle->first )
					{
						it->second.nParticleNum++;
						++m_nTotalParticleNum;
					}
					pParticle = *ppCurPar;
				}
				else
				{
					ppCurPar = &pParticle->m_Next;
					pParticle = pParticle->m_Next;
					itParticle->second.nParticleNum++;
					++m_nTotalParticleNum;
				}
			}
		}
	}

	if( !bAlloc )
		return;

	if( GetRefCount() && IsInRenderTime() )
	{
		int32 nDeltaTime = int32(m_DeltaTime);
		if( nDeltaTime > 0 )
		{
			m_RemainTime += nDeltaTime;

			uint32 uStartTime = CurTime;
			float fWeight = 0.0f;
			float fDeltaW = pProp->m_nEmissionInterval/float(m_RemainTime);
			int32 ParticlesPerEmission = int32(pProp->m_nParticlesPerEmission * ( CEffect::GetEffectRenderLod() - pProp->m_EffectLod )/MIN_LEVEL );
			limit( ParticlesPerEmission,1,pProp->m_nParticlesPerEmission );
			while( (int32)m_RemainTime >= pProp->m_nEmissionInterval )
			{
				for( int32 i = 0; i < ParticlesPerEmission; ++i )
				{
					Particle* pParticle = pProp->m_pEffectClassDesc->AllocParticle( this );
					if( !pParticle )
						break;
					ParticleList& List = m_ParticleOrderByTexture[ pProp->m_Texture.begin()->second ];
					pParticle->m_Next = List.pParticle;
					List.pParticle = pParticle;
					InitParticle( pParticle, matPre , fWeight );
					++List.nParticleNum;
					++m_nTotalParticleNum;
				}
				m_RemainTime -= pProp->m_nEmissionInterval;
				uStartTime   += pProp->m_nEmissionInterval;
				fWeight		 += fDeltaW;
			}
		}
	}
	else
		m_RemainTime = 0;
}

void CParticleSystem::InitParticle( Particle* pParticle, const CMatrix& matPre , float fWeight )
{
	const CParticleSystemProp* pProp = (CParticleSystemProp*)m_pEffectUnitProp;

	float fPercent = GetUnitPercent();
	float fMinRadius = 0.0f, fMaxRadius = 0.0f;
	pProp->GetCurAdvanceProp(fPercent, fMinRadius, fMaxRadius);

	pParticle->m_CurPos.x = rand() - 16384.0f;
	pParticle->m_CurPos.y = rand() - 16384.0f;
	pParticle->m_CurPos.z = rand() - 16384.0f;
	pParticle->m_CurSpd.x = 0.0f;
	pParticle->m_CurSpd.y = 0.0f;
	pParticle->m_CurSpd.z = 0.0f;

	if( pProp->m_eEmissionType == CParticleSystemProp::PSEMS_eCircle )
		pParticle->m_CurPos -= pProp->m_vEmissionPlan*pParticle->m_CurPos.Dot( pProp->m_vEmissionPlan );
	pParticle->m_CurPos.Normalize();

	// Normalize it
	if( pProp->m_eVelType == CParticleSystemProp::PSVEL_eRANDOM )
	{
		pParticle->m_CurSpd.x = pProp->m_vMinVelocity.x + ( pProp->m_vMaxVelocity.x - pProp->m_vMinVelocity.x )*rand()*invMaxRand;
		pParticle->m_CurSpd.y = pProp->m_vMinVelocity.y + ( pProp->m_vMaxVelocity.y - pProp->m_vMinVelocity.y )*rand()*invMaxRand;
		pParticle->m_CurSpd.z = pProp->m_vMinVelocity.z + ( pProp->m_vMaxVelocity.z - pProp->m_vMinVelocity.z )*rand()*invMaxRand;
	}
	else if( pProp->m_eVelType != CParticleSystemProp::PSVEL_eTOCENTER )
	{
		CVector3f zDir = pParticle->m_CurPos.Cross( pProp->m_vEmissionPlan );
		if( zDir.x == 0.0f && zDir.y == 0.0f && zDir.z == 0.0f )
		{
			zDir = pParticle->m_CurPos;
			zDir.x = 2;
			zDir.Normalize();
		}
		CVector3f xDir = zDir.Cross( pParticle->m_CurPos );
		pParticle->m_CurSpd.x = pProp->m_vMinVelocity.x + ( pProp->m_vMaxVelocity.x - pProp->m_vMinVelocity.x )*rand()*invMaxRand;
		pParticle->m_CurSpd.y = pProp->m_vMinVelocity.y + ( pProp->m_vMaxVelocity.y - pProp->m_vMinVelocity.y )*rand()*invMaxRand;
		pParticle->m_CurSpd.z = pProp->m_vMinVelocity.z + ( pProp->m_vMaxVelocity.z - pProp->m_vMinVelocity.z )*rand()*invMaxRand;
		pParticle->m_CurSpd = pParticle->m_CurPos*pParticle->m_CurSpd.y + zDir*pParticle->m_CurSpd.z + xDir*pParticle->m_CurSpd.x;
	}

	pParticle->m_LifeSpan      = pProp->m_nMinLifeSpan + ( pProp->m_nMaxLifeSpan - pProp->m_nMinLifeSpan )*rand()*invMaxRand;
	pParticle->m_LeftLife			= pParticle->m_LifeSpan;	
	pParticle->m_AngularVelocity = pProp->m_fMinAngularVelocity + ( pProp->m_fMaxAngularVelocity - pProp->m_fMinAngularVelocity )*rand()*invMaxRand;
	pParticle->m_CurAngular		 = 0;

	// Now scale it into our desired range
	if ( pProp->m_eEmissionType == CParticleSystemProp::PSEMS_eCircle || pProp->m_eEmissionType == CParticleSystemProp::PSEMS_eSphere )
	{
		pParticle->m_CurPos *= fMinRadius + ( fMaxRadius - fMinRadius )*rand()*invMaxRand;
	}
	else if ( pProp->m_eEmissionType == CParticleSystemProp::PSEMS_eRectangle )
	{
		CVector3f xDir = pProp->m_vEmissionPlan.Cross(CVector3f(0.001f,1,0));
		xDir.Normalize();
		CVector3f zDir = xDir.Cross( pProp->m_vEmissionPlan );
		zDir.Normalize();

		float eT = rand() * invMaxRand * 2.0f;

		if( eT >= 1.0f )
			eT-=1.0f;

		float tLong = ( fMaxRadius - fMinRadius)* (rand() * invMaxRand );
		if( eT > pProp->m_fLongShortDRadius / ( pProp->m_fLongShortDRadius + pProp->m_fLongShortRadius ) )
		{
			pParticle->m_CurPos = xDir * ( (tLong + fMinRadius) );
			pParticle->m_CurPos+= zDir * ( (fMinRadius * pProp->m_fLongShortRadius + ( fMaxRadius - fMinRadius))* (rand() * invMaxRand *2.0f  - 1.0f));
		}
		else
		{
			pParticle->m_CurPos = xDir * ( fMinRadius  * (rand() * invMaxRand *2.0f  - 1.0f));
			pParticle->m_CurPos+= zDir * ( (fMinRadius * pProp->m_fLongShortRadius + tLong ) );
		}


		if( pProp->m_eVelType == CParticleSystemProp::PSVEL_eTOCENTER )
		{
			CVector3f vV = pParticle->m_CurPos;
			vV.Normalize();
			pParticle->m_CurSpd = vV * ( pProp->m_vMinVelocity.x + ( pProp->m_vMaxVelocity.x - pProp->m_vMinVelocity.x ) * rand() * invMaxRand );
		}
	}
	else if( pProp->m_eEmissionType == CParticleSystemProp::PSEMS_eMulCorner)
	{
		CVector3f xDir;
		xDir.Init( fMinRadius + (fMaxRadius-fMinRadius)*(rand()* invMaxRand), 0, 0);
		CMatrix tR;
		tR.SetRotation( pProp->m_vEmissionPlan, 2.0f * CMath::pi / pProp->m_fCornerDivision *(int)(rand() * invMaxRand * pProp->m_fCornerDivision));
        pParticle->m_CurPos = xDir * tR;		
	}

	if( pProp->m_eFollowType != CParticleSystemProp::PSPC_Follow && pProp->m_eFollowType != CParticleSystemProp::PSPC_Normal )
	{
		pParticle->m_CurSpd.Rotate( m_matWorld );
		if( pProp->m_eFollowType != CParticleSystemProp::PSPC_Static )
		{
			CVector3f vecPre = VEC_FASTMULT( pParticle->m_CurPos, matPre );
			CVector3f vecCur = VEC_FASTMULT( pParticle->m_CurPos, m_matWorld );		
			pParticle->m_CurPos = ( vecCur - vecPre )*fWeight + vecPre;
			if( pProp->m_eFollowType == CParticleSystemProp::PSPC_AddSpeed )
				pParticle->m_CurSpd += m_vecOffsetVelocity;
		}	
	}
}

void CParticleSystem::Free()
{
	if( m_pEffectUnitProp->m_bSlownDisappear )
	{
		((CParticleSystemProp*)m_pEffectUnitProp)->m_pEffectClassDesc->AddLeavingsEffectUnit( this );
	}
	else
		delete this;
}
