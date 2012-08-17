#include "stdafx.h"
#include "cpolytrail.h"
#include "CMath.h"
#include "TSqrAllocator.inl"
#include "CGraphic.h"
#include "CColor.inl"
DYNAMIC_CREATE_EFFECTUNITBASE( ePolyTrail, IDS_POLYTRAIL, CPolyTrail, CPolyTrailProp )

enum
{
	POLYTRAIL_ORIGIN,
	POLYTRAIL_ADD_RECT_TRAIL,
	POLYTRAIL_ADD_SINGLE_SIDE,
	POLYTRAIL_ADD_LOCALSKELETAL,
	POLYTRAIL_ADD_SMOOTH,
	POLYTRAIL_ADD_DECLTIME,
	POLYTRAIL_ADD_REFRACT,
	POLYTRAIL_ADD_TEXOP,
};


CPolyTrailProp::CPolyTrailProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
	: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_vecRotate				= CVector3f( 1, 0, 0 );
	m_MinTrailWidth			= 0;
	m_Smooth				= 1;	
	m_MaxTrailWidth			= 30;
	m_UnitLifeSpan			= 500;
	m_eBlendModel			= SPRB_ADD;
	m_eTextOp				= SPRO_MODULATE;
	m_eZTest				= Z_TESTONLY;
	m_bLightEnable			= FALSE;
	m_eStrip				= PTT_TRAIL;
	m_bSingleSide			= FALSE;
	m_DeclTime				= 0;		
	m_fRefractIndex			= 0.0f;

	m_vecOffset[0.0f] = CVector3f( 0, 0, 0 );
	m_TextName[0.0f] = "None";
	m_Color[0.0f] = (uint32)0xffccffff;
	m_Color[1.0f] = (uint32)0x00ffffff;
	m_Scale[0.0f] = (BYTE)( ((uint32)0xff000000)>>24 );
	m_Scale[1.0f] = (BYTE)( ((uint32)0x00000000)>>24 );
}

CPolyTrailProp::~CPolyTrailProp(void)
{
}

int32 CPolyTrailProp::LoadBin( IEffectRead& File )
{
	uint32 Version;
	File.read( (char*)&Version,					sizeof(uint32) );

	m_eStrip										= PTT_TRAIL;
	m_bSingleSide									= FALSE;

	switch( Version )
	{
	case POLYTRAIL_ADD_TEXOP:
		File.read( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP));
	case POLYTRAIL_ADD_REFRACT:
		File.read(&m_fRefractIndex, sizeof(m_fRefractIndex));
	case POLYTRAIL_ADD_DECLTIME:
		File.read( (char*)&m_DeclTime,				sizeof(int32) );
	case POLYTRAIL_ADD_SMOOTH:
		File.read( (char*)&m_Smooth,				sizeof(int32) );
	case POLYTRAIL_ADD_LOCALSKELETAL:
		File.read( (char*)&m_sRefSkelatal,			sizeof(IDNAME) );
	case POLYTRAIL_ADD_SINGLE_SIDE:
		File.read( (char*)&m_bSingleSide,			sizeof(BOOL) ); 
	case POLYTRAIL_ADD_RECT_TRAIL:
		File.read( (char*)&m_eStrip,				sizeof(POLY_TRAIL_TYPE) );
	
	default:
		File.read( (char*)&m_vecRotate,				sizeof(CVector3f) );
		File.read( (char*)&m_MinTrailWidth,			sizeof(float) );
		File.read( (char*)&m_MaxTrailWidth,			sizeof(float) );
		File.read( (char*)&m_UnitLifeSpan,			sizeof(int32) );
		File.read( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
		File.read( (char*)&m_eZTest,				sizeof(Z_TEST) );
		File.read( (char*)&m_bLightEnable,			sizeof(BOOL) );
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;

	return 0;
}

void CPolyTrailProp::_UpdateParams(void)
{
	float n = m_vecRotate.Mag();
	if( n > 0.0001f )
		m_vecRotate = m_vecRotate/n;
	else
		m_vecRotate = CVector3f( 1, 0, 0 );
}

int32 CPolyTrailProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = POLYTRAIL_ADD_TEXOP;
	File.write( (char*)&Version,				sizeof(uint32) );
	File.write( (char*)&m_eTextOp,				sizeof(SPR_BLENDOP));
	File.write( (char*)&m_fRefractIndex,		sizeof(m_fRefractIndex));
	File.write( (char*)&m_DeclTime,			sizeof(int32) );
	File.write( (char*)&m_Smooth,				sizeof(int32) );
	File.write( (char*)&m_sRefSkelatal,		sizeof(IDNAME) );
	File.write( (char*)&m_bSingleSide,			sizeof(BOOL) );
	File.write( (char*)&m_eStrip,				sizeof(POLY_TRAIL_TYPE) );
	File.write( (char*)&m_vecRotate,			sizeof(CVector3f) );
	File.write( (char*)&m_MinTrailWidth,		sizeof(float) );
	File.write( (char*)&m_MaxTrailWidth,		sizeof(float) );
	File.write( (char*)&m_UnitLifeSpan,			sizeof(int32) );
	File.write( (char*)&m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	File.write( (char*)&m_eZTest,				sizeof(Z_TEST) );
	File.write( (char*)&m_bLightEnable,			sizeof(BOOL) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}


//////////////////////////////////////////////////////////////////////////

CPolyTrail::CPolyTrail( IEffectUnitProp* pEffectUnitProp )
	: CEffectUnit( pEffectUnitProp ),m_DeclTime(0)
	,m_TrailUnitNum(0)
	, m_nRefSkeletal(-1)
{
	m_pTrail				= NULL;
	
	const CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;
	m_it = pProp->m_pEffectClassDesc->InsertTrail( this );

	//m_vecScale +=  CVector3f( pProp->m_MaxTrailWidth*2, pProp->m_MaxTrailWidth*2, pProp->m_MaxTrailWidth*2 );
}

CPolyTrail::~CPolyTrail(void)
{
	const CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;

	Trail* pTrail = m_pTrail;
	while( pTrail )
	{
		Trail* Temp = pTrail->m_Next;
		pProp->m_pEffectClassDesc->FreeTrail( pTrail );
		pTrail = Temp;
	}
	pProp->m_pEffectClassDesc->RemoveTrail( m_it );
}

void CPolyTrail::OnLinked( CRenderable* pParentObj  )
{
	const CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;

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

void CPolyTrail::OnUnlinked(void)
{
	m_pParent = NULL;
	m_nRefSkeletal = -1;
	m_matRefence.Identity();
}

const CMatrix& CPolyTrail::GetMatrix()	
{ 
	static CMatrix mat;
	mat = m_matWorld*m_matRefence;
	return mat;
}

const EFFECT_RESULT	CPolyTrail::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	const CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;

	EFFECT_RESULT Result( NULL, true, false );
	CMatrix matRender = matWorld;
	if( GetRefCount() )
	{
		if( m_nRefSkeletal >= 0 && m_pParent )
		{
			( (CModel*)m_pParent )->GetMatrix( m_matRefence, m_nRefSkeletal, uint32(-1) );
			if( pProp->m_eUpdatePos == UP_SOCKETOFFSET || pProp->m_eUpdatePos == UP_NODEOFFSET )
			{
				m_matRefence._11 = m_matRefence._22 = m_matRefence._33 = m_matRefence._44 = 1.0f;
				m_matRefence._12 = m_matRefence._13 = m_matRefence._14 = m_matRefence._21 = 0.0f;
				m_matRefence._23 = m_matRefence._24 = m_matRefence._31 = m_matRefence._32 = 0.0f;
				m_matRefence._34 = 0.0f;
			}
			CMatrix matInvRef = m_matRefence;
			if( pProp->m_eUpdatePos != UP_SOCKETOFFSET && pProp->m_eUpdatePos != UP_NODEOFFSET )
				matInvRef.InvertFast();
			else
			{
				matInvRef._41 = -matInvRef._41;
				matInvRef._42 = -matInvRef._42;
				matInvRef._43 = -matInvRef._43;
			}
			matRender = matRender*matInvRef;
		}
		m_matWorld = matRender;
	}
	//m_matWorld = matWorld*matRender;

	CMatrix rWorld;
	CVector3f vScale;
	
	vScale.x = m_matWorld.GetAxis(0).Mag();
	vScale.y = m_matWorld.GetAxis(1).Mag();
	vScale.z = m_matWorld.GetAxis(2).Mag();
	rWorld.SetWithRow( m_matWorld.GetAxis(0) / vScale.x,
		m_matWorld.GetAxis(1) / vScale.y, m_matWorld.GetAxis(2) / vScale.z );

	UpdateTrail(m_CurTime, true,rWorld,vScale );
	Result.m_UnitbEmpty = !m_TrailUnitNum;

	if( m_TrailUnitNum < 2 )
		return Result;
	if (gIsZero(pProp->m_fRefractIndex))
	{
		UpdateToBuffer(pProp, pRORS);
	}
	else
	{
		UpdateToRefractBuffer(pProp);
	}

	return Result;
}	

void CPolyTrail::UpdateTrail( uint32 CurTime, bool bAlloc,const CMatrix& rMatrix, const CVector3f& vScale)
{
	const CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;

	m_TrailUnitNum = 0;
	Trail* pTrail = m_pTrail;
	Trail** ppTrail = &m_pTrail;

	while( pTrail )
	{
		if( (int32)( pTrail->m_LifeTime ) > (pProp->m_UnitLifeSpan - m_DeclTime) )
		{
			*ppTrail = pTrail->m_Next;
			pProp->m_pEffectClassDesc->FreeTrail( pTrail );
			pTrail = *ppTrail;
		}
		else
		{
			pTrail->m_LifeTime += m_DeltaTime;
			ppTrail = &pTrail->m_Next;
			pTrail = pTrail->m_Next;
			++m_TrailUnitNum;
		}
    }

    if( !bAlloc )
        return;

	if( !GetRefCount() || !IsInRenderTime() || (int32)( m_DeltaTime ) < 0 )
	{
		m_DeclTime = pProp->m_DeclTime *( int32(m_DeltaTime) -  pProp->m_TimeRange ) / 100;
		return;
	}

	CVector3f Offset = GetMapValue( pProp->m_vecOffset, GetUnitPercent() );
	CVector3f Pos =  m_matWorld.GetAxis(3);
	
	CQuaternion tempQuat;	
	tempQuat.FromRotationMatrix(rMatrix);
	

	m_DeclTime = 0;

	pTrail					= pProp->m_pEffectClassDesc->AllocTrail( this );

	if( !pTrail )
		return;
	++m_TrailUnitNum;

	CVector3f RotateVec = pProp->m_vecRotate * vScale;
	pTrail->m_Position1		= m_matWorld * Offset;
	pTrail->m_LifeTime		= 0;
	pTrail->m_Position2		= tempQuat*RotateVec;
	
	//m_matWorld._41 = pTrail->m_Position1.x;
	//m_matWorld._42 = pTrail->m_Position1.y;
	//m_matWorld._43 = pTrail->m_Position1.z;
	Trail* Cur = m_pTrail;
	float Angle = 0.0f;

	float fDeltaOm = 2.0f / pProp->m_nTextureCircle;

	if( pProp->m_Smooth > 1 &&  m_pTrail && m_pTrail->m_LifeTime)
	{	
		float DeltaT = (float)( m_pTrail->m_LifeTime );
		if ( DeltaT>0.0f )
		{
			float tpp = max( DeltaT*fDeltaOm/pProp->m_Smooth, 1.0f );
			CVector3f DeltaV = (Offset-m_LastVec)/DeltaT;
			CVector3f DeltaP = (Pos-m_LastPos)/DeltaT;
			float	InvDeltaT = 1.0f/DeltaT;
			DeltaT -= tpp;
			for( float t = tpp; t < DeltaT; t += tpp )
			{
				Trail* Temp = pProp->m_pEffectClassDesc->AllocTrail(this);
				if( !Temp )
					break;
				CQuaternion temp;
				CQuaternion::Slerp( temp,m_LastQuat,tempQuat,t*InvDeltaT,true);
				//m_LastPos += DeltaP;
				//m_LastVec += DeltaV;
				Temp->m_Position1 	= temp*(m_LastVec + t*DeltaV);
				Temp->m_Position1  *= vScale;
				Temp->m_Position1   += m_LastPos + t*DeltaP;
				Temp->m_Position2	= temp*RotateVec;
				//Temp->m_Position2	*= vScale;
				//Temp->m_StartTime	= CurTime;

				Temp->m_LifeTime	= m_pTrail->m_LifeTime - (int32)t;
				Temp->m_Next		= Cur;
				Cur					= Temp;
				++m_TrailUnitNum		;
			}
		}	
	}
	m_LastQuat = tempQuat;
	m_LastVec = Offset;
	m_LastPos = Pos;
	pTrail->m_Next			= Cur;
	m_pTrail				= pTrail;
}

void CPolyTrail::Free()
{		
	if( m_pEffectUnitProp->m_bSlownDisappear )
	{
		m_pEffectUnitProp->m_pEffectClassDesc->AddLeavingsEffectUnit( this );
	}
	else
		delete this;
}

Trail*	CPolyTrail::GetCurTrail(void)
{
	return m_pTrail;
}

CVector3f CPolyTrail::GetEndPos(Trail* pTrail)
{
	CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;
	float Percent = ( pTrail->m_LifeTime ) / pProp->m_UnitLifeSpan;
	float Scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, Percent )/255.0f );
	return CVector3f(pTrail->m_Position1.x - pTrail->m_Position2.x*Scale, 
		pTrail->m_Position1.y - pTrail->m_Position2.y*Scale,
		pTrail->m_Position1.z - pTrail->m_Position2.z*Scale);
}

CVector3f CPolyTrail::GetBeginPos(Trail* pTrail)
{
	CPolyTrailProp* pProp = (CPolyTrailProp*)m_pEffectUnitProp;
	float Percent = ( (float)pTrail->m_LifeTime ) / pProp->m_UnitLifeSpan;
	float Scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, Percent )/255.0f );
	if( pProp->m_bSingleSide )
		return CVector3f( pTrail->m_Position1.x, pTrail->m_Position1.y,	pTrail->m_Position1.z );
	else
		return CVector3f( pTrail->m_Position1.x + pTrail->m_Position2.x*Scale, 
		pTrail->m_Position1.y + pTrail->m_Position2.y*Scale,
		pTrail->m_Position1.z + pTrail->m_Position2.z*Scale );
}

//------------------------------------------------------------------------------
void
CPolyTrail::UpdateToBuffer( const CPolyTrailProp* pProp, RenderObjStyle* pRORS )
{
	WORD* pIB = NULL;
	VerColorTex* pVB = NULL;
	RenderState* pRS = NULL;
	ITexture* pCurText = pProp->m_Texture.size() ? pProp->m_Texture.begin()->second : NULL;
	CMatrix* pMatrix = NULL;
	switch( pProp->m_eStrip )
	{
	case PTT_BLOCK:
		{
			pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLELIST, 
				4*( m_TrailUnitNum - 1 ), 2*( m_TrailUnitNum - 1 ), 
				VerColorTex::Format, sizeof(VerColorTex), (void**)&pVB, (void**)&pIB, (void**)&pRS );
		}
		break;
	case PTT_TRAIL:
	case PTT_STRECTH:
		{
			pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT,pRORS->m_eType, pCurText, NULL, PT_TRIANGLESTRIP, 
				2*m_TrailUnitNum, 2*( m_TrailUnitNum - 1 ), 
				VerColorTex::Format, sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS );
		}
		break;
	}

	*pMatrix = m_matRefence;

	pRS->m_TexOP_S0			= pProp->m_eTextOp + 2;
	pRS->m_Cull				= 1;				//D3DCULL_NONE;
	pRS->m_AlpOP_S0			= 4;				//D3DTOP_MODULATE;
	pRS->m_LightEnable		= pProp->m_bLightEnable;
	pRS->m_ZTestEnable		= pProp->m_eZTest != Z_NONE;
	pRS->m_ZWrite			= pProp->m_eZTest != Z_TESTONLY;
	pRS->m_ZTestFun			= 2;				//D3DCMP_LESS;
	pRS->m_UAddr_S0			= 3;				//D3DTADDRESS_BORDER; 
	//pRS->m_FillMode			= 2;

	if( pProp->m_eBlendModel == SPRB_NONE )
		pRS->m_AlphaBlendEnable = FALSE;
	else
	{
		pRS->m_AlphaBlendEnable = TRUE;
		if( pProp->m_eBlendModel != SPRB_MULTIPLY )
			pRS->m_DestBlend = 2;				//D3DBLEND_ONE;
	}

	float fTexturPercent = pCurText ? float( pCurText->GetHeight() )/pCurText->GetWidth() : 1.0f;
	Trail* pTrail = m_pTrail;
	float fv = 0.0001f;
	WORD nVBBase = 0;

	// 加入贴图拉伸功能
	switch( pProp->m_eStrip )
	{
	case PTT_BLOCK:
		{
			while( pTrail->m_Next )
			{
				float Percent = ( (float)pTrail->m_LifeTime ) / pProp->m_UnitLifeSpan;
				uint32 Color = GetMapValue( pProp->m_Color, Percent );
				float Scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, Percent )/255.0f );

				CVector3f Delta = pTrail->m_Next->m_Position1 - pTrail->m_Position1;
				Delta.Normalize();
				Delta *= fTexturPercent*Scale;

				pVB[0].p = pTrail->m_Position1 - Delta - pTrail->m_Position2*Scale;
				pVB[0].diffuse = Color;
				pVB[0].tu = 0.0f;
				pVB[0].tv = 0.0f;

				if( pProp->m_bSingleSide )
					pVB[0].p = pTrail->m_Position1 - Delta;
				else
					pVB[1].p = pTrail->m_Position1 - Delta + pTrail->m_Position2*Scale;
				pVB[1].diffuse = Color;
				pVB[1].tu = 1.0f;
				pVB[1].tv = 0.0f;

				pVB[2].p = pTrail->m_Position1 + Delta - pTrail->m_Position2*Scale;
				pVB[2].diffuse = Color;
				pVB[2].tu = 0.0f;
				pVB[2].tv = 1.0f;

				if( pProp->m_bSingleSide )
					pVB[2].p = pTrail->m_Position1 + Delta;
				else
					pVB[3].p = pTrail->m_Position1 + Delta + pTrail->m_Position2*Scale;
				pVB[3].diffuse = Color;
				pVB[3].tu = 1.0f;
				pVB[3].tv = 1.0f;

				pIB[0] = nVBBase + 0;
				pIB[1] = nVBBase + 1;
				pIB[2] = nVBBase + 2;
				pIB[3] = nVBBase + 1;
				pIB[4] = nVBBase + 3;
				pIB[5] = nVBBase + 2;

				pVB += 4;
				pIB += 6;
				nVBBase += 4;

				pTrail = pTrail->m_Next;
			}
		}
		break;
	case PTT_TRAIL:
		{
			while( pTrail )
			{
				float Percent = ( (float)pTrail->m_LifeTime )/pProp->m_UnitLifeSpan;

				uint32 Color = GetMapValue( pProp->m_Color, Percent );
				float Scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, Percent )/255.0f );

				if( pProp->m_bSingleSide )
					pVB->Set( pTrail->m_Position1.x, pTrail->m_Position1.y,	pTrail->m_Position1.z,
					Color, 0, fv );
				else
					pVB->Set( pTrail->m_Position1.x + pTrail->m_Position2.x*Scale, 
					pTrail->m_Position1.y + pTrail->m_Position2.y*Scale,
					pTrail->m_Position1.z + pTrail->m_Position2.z*Scale,
					Color, 0, fv );
				pVB++;
				pVB->Set( pTrail->m_Position1.x - pTrail->m_Position2.x*Scale, 
					pTrail->m_Position1.y - pTrail->m_Position2.y*Scale,
					pTrail->m_Position1.z - pTrail->m_Position2.z*Scale,
					Color, 1, fv );
				pVB++;
				fv += 1.0f;

				pTrail = pTrail->m_Next;
			}
		}
		break;
	case PTT_STRECTH:
		{
			float dv = 1.0f/(m_TrailUnitNum - 1);
			while( pTrail )
			{
				float Percent = ( (float)pTrail->m_LifeTime )/pProp->m_UnitLifeSpan;
				uint32 Color = GetMapValue( pProp->m_Color, Percent );
				float Scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, Percent )/255.0f );

				if( pProp->m_bSingleSide )
					pVB->Set( pTrail->m_Position1.x, pTrail->m_Position1.y,	pTrail->m_Position1.z,
					Color, 0, fv );
				else
					pVB->Set( pTrail->m_Position1.x + pTrail->m_Position2.x*Scale, 
					pTrail->m_Position1.y + pTrail->m_Position2.y*Scale,
					pTrail->m_Position1.z + pTrail->m_Position2.z*Scale,
					Color, 0, fv );
				pVB++;
				pVB->Set( pTrail->m_Position1.x - pTrail->m_Position2.x*Scale, 
					pTrail->m_Position1.y - pTrail->m_Position2.y*Scale,
					pTrail->m_Position1.z - pTrail->m_Position2.z*Scale,
					Color, 1, fv );
				pVB++;
				fv += dv;

				pTrail = pTrail->m_Next;
			}
		}
		break;
	}
}

//------------------------------------------------------------------------------
void
CPolyTrail::UpdateToRefractBuffer( const CPolyTrailProp* pProp )
{
	if (m_TrailUnitNum <= 2)
		return;
	float percent = GetUnitPercent();

	VerNorDif* vb = NULL;
	STATIC_RS* rs = NULL;
	CMatrix* matrix = CGraphic::GetInst()->GetRenderStack(
		RS_DEFAULT, ROS_REFRACT, NULL, NULL, PT_TRIANGLESTRIP, 2 * (m_TrailUnitNum - 1),
		2 * (m_TrailUnitNum - 2), VerNorDif::Format, sizeof(VerNorDif),
		(void**)&vb, NULL, (void**)&rs, 0);
	if (NULL == matrix)
		return;
	*matrix = m_matRefence;
	rs->m_fRefractIndex = pProp->m_fRefractIndex;
	rs->m_Material.Diffuse = CColor4::White;
	// write vertex buffer
	Trail* trail = m_pTrail;
	while (NULL != trail->m_Next)
	{
		percent = float(trail->m_LifeTime) / pProp->m_UnitLifeSpan;
		uint32 color = GetMapValue(pProp->m_Color, percent);
		float scale = ( pProp->m_MinTrailWidth + ( pProp->m_MaxTrailWidth - pProp->m_MinTrailWidth )*GetMapValue( pProp->m_Scale, percent )/255.0f );
		CVector3f pos1 = trail->m_Position1;
		CVector3f pos2 = trail->m_Position2 * scale;
		CVector3f normal = (pos2 - pos1).Cross(trail->m_Next->m_Position1 - pos1);
		normal.Normalize();
		vb->p = pos1, vb->np = normal, vb->diffuse = color;
		if (!pProp->m_bSingleSide)
			vb->p += pos2;
		++vb;
		vb->p = pos1 - pos2, vb->np = normal, vb->diffuse = color;
		++vb;

		trail = trail->m_Next;
	}
}

void CPolyTrail::RenderChild(IEffectUnit* pEffectUnit, uint32 uCurTime, RenderObjStyle* pRORS)
{
	CMatrix		matrix;
	CVector3f	Offset = m_matWorld * m_LastVec;

	matrix = m_matWorld;
	matrix._41 = Offset.x;
	matrix._42 = Offset.y;
	matrix._43 = Offset.z;

	pEffectUnit->Render( matrix, uCurTime, pRORS );
}
