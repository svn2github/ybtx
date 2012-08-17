#include "stdafx.h"
#include "CModelRenderStyle.h"
#include "TSqrAllocator.inl"

enum
{
	ModelRS_Version_0,
	ModelRS_Version_ClipPlane,	///加入裁剪面
	ModelRS_Version_Current,	///< 裁剪面距离动态变化
};

DYNAMIC_CREATE_EFFECTUNITBASE( eModelRenderStyle, IDS_MODELRENDERSTYLE, CModelRenderStyle, CModelRenderStyleProp )

CModelRenderStyleProp::CModelRenderStyleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc ) 
: CEffectUnitProp( ID, pEffectClassDesc )
{
	m_SpecularColor		= CColor( 0, 0, 0, 0 );
	m_vecTextMoveVel	= CVector2f( 0, 0 );
	m_fTextAngularVel	= 0;
}

CModelRenderStyleProp::~CModelRenderStyleProp()
{
	m_ApplyClass.clear();
}

int32 CModelRenderStyleProp::LoadBin( IEffectRead& File )
{
	uint32 Version = 0;
	File.read( (char*)&Version,					sizeof(uint32) );

	switch (Version)
	{
	case ModelRS_Version_Current:
		LoadMapValue(File, m_ClipDistMap);
	case ModelRS_Version_ClipPlane:
		File.read(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
		if (!m_ClipPlane.originPlane.m_Normal.IsZero() && m_ClipDistMap.empty())
		{
			m_ClipDistMap[0.0f] = -m_ClipPlane.originPlane.m_Dist;
			m_ClipDistMap[1.0f] = -m_ClipPlane.originPlane.m_Dist;
		}
	case ModelRS_Version_0:
		int32 n = 0;
		File.read( (char*)&n,						sizeof(int32) );
		m_ApplyClass.resize(n);
		for( int32 i = 0; i < n; ++i )
			LoadStringT( File, m_ApplyClass[i] );
		File.read( (char*)&m_SpecularColor,			sizeof(CColor) );
		File.read( (char*)&m_vecTextMoveVel,		sizeof(CVector2f) );
		File.read( (char*)&m_fTextAngularVel,		sizeof(float) );
		File.read( (char*)&m_bDiffuseAmbient,		sizeof(BOOL) );
		File.read( (char*)&m_bSpecular,				sizeof(BOOL) );
		File.read( (char*)&m_bSetTextureMatrix,		sizeof(BOOL) );
	}

	if( FAILED( CEffectUnitProp::LoadBin( File ) ) )
		return GERROR;
	return 0;
}

void CModelRenderStyleProp::CaculateProp()
{
	if (!m_ClipPlane.originPlane.m_Normal.IsZero() && m_ClipDistMap.empty())
	{
		m_ClipDistMap[0.0f] = -m_ClipPlane.originPlane.m_Dist;
		m_ClipDistMap[1.0f] = -m_ClipPlane.originPlane.m_Dist;
	}
}

void CModelRenderStyleProp::_UpdateParams(void)
{
	m_bSpecular = m_SpecularColor != CColor( 0, 0, 0, 0 );
}

int32 CModelRenderStyleProp::SaveBin( IEffectWrite& File )
{
	uint32 Version = ModelRS_Version_Current;
	File.write( (char*)&Version,				sizeof(uint32) );

	SaveMapValue(File, m_ClipDistMap);
	File.write(&m_ClipPlane.originPlane, sizeof(m_ClipPlane.originPlane));
	int32 n = m_ApplyClass.size();
	File.write( (char*)&n,						sizeof(int32) );
	for( int32 i = 0; i < n; ++i )
		SaveStringT( File, m_ApplyClass[i].c_str() );
	File.write( (char*)&m_SpecularColor,		sizeof(CColor) );
	File.write( (char*)&m_vecTextMoveVel,		sizeof(CVector2f) );
	File.write( (char*)&m_fTextAngularVel,		sizeof(float) );
	File.write( (char*)&m_bDiffuseAmbient,		sizeof(BOOL) );
	File.write( (char*)&m_bSpecular,			sizeof(BOOL) );
	File.write( (char*)&m_bSetTextureMatrix,	sizeof(BOOL) );
	if( FAILED( CEffectUnitProp::SaveBin( File ) ) )
		return GERROR;
	return 0;
}
//////////////////////////////////////////////////////////////////////////

EMap< CRenderable*, CModelRenderStyle* > CModelRenderStyle::s_LinkList;

CModelRenderStyle::CModelRenderStyle( IEffectUnitProp* pEffectUnitProp )
: CEffectUnit( pEffectUnitProp )
{
	m_pPreUnit = NULL;
	m_pNexUnit = NULL;
	m_pParent  = NULL;
	m_bNeedClear	= false;
}

CModelRenderStyle::~CModelRenderStyle(void)
{
	OnUnlinked();
}

//以下的函数过程比较复杂，用于确保只有一个最后一个叠加的CModelRenderStyle作用于pParentObj身上
//"CModelRenderStyle"是CModelRenderStyle的专用连接点
void CModelRenderStyle::OnLinked( CRenderable* pParentObj )
{
	if( pParentObj->GetType() != ROT_MODEL )
		return;

	CEffectUnit::OnLinked(pParentObj);
	CModelRenderStyle*& pLink = s_LinkList[m_pParent];

	if( pLink == NULL )
		pLink = this;
	else
	{
		m_pNexUnit = pLink;
		pLink->m_pPreUnit = this;
		pLink = this;
		m_pPreUnit = NULL;
	}
}

void CModelRenderStyle::OnUnlinked(void)
{
	if(  m_pParent )
	{
		((CModel*)m_pParent)->ClearRsMask(true);
		m_bNeedClear = false;
	}

	if( m_pPreUnit )
	{
		m_pPreUnit->m_pNexUnit = m_pNexUnit;
		if( m_pNexUnit )
			m_pNexUnit->m_pPreUnit = m_pPreUnit;
	}
	else
	{
		CModelRenderStyle*& pLink = s_LinkList[m_pParent];
		if( m_pNexUnit )
		{
			m_pNexUnit->m_pPreUnit = NULL;
			pLink = m_pNexUnit;
		}
		else
		{
			s_LinkList.erase( m_pParent );
		}
	}
	m_pParent = NULL;
	m_pPreUnit = NULL;
	m_pNexUnit = NULL;
}

const EFFECT_RESULT CModelRenderStyle::RenderEx( const CMatrix& matWorld, RenderObjStyle* pRORS )
{
	EFFECT_RESULT Result( NULL,  true, false );

	const CModelRenderStyleProp* pProp = (CModelRenderStyleProp*)m_pEffectUnitProp;
	if( !IsInRenderTime() || m_pPreUnit )
	{
		if(m_bNeedClear && m_pParent )
		{
			((CModel*)m_pParent)->ClearRsMask();
			m_bNeedClear = false;
		}
		return Result;
	}

	SetRenderStyleToParent( m_CurTime, matWorld );
	return Result;
}

void CModelRenderStyle::SetRenderStyleToParent( uint32 CurTime, const CMatrix& matWorld )
{
	CModelRenderStyleProp* pProp = (CModelRenderStyleProp*)m_pEffectUnitProp;

	if( m_pNexUnit )
		m_pNexUnit->SetRenderStyleToParent( CurTime, matWorld );

	CPieceRS rs;
	memset( &rs, 0, sizeof( rs ) );

	CColor Color = 0;
	float Percent = GetUnitPercent();

	if( pProp->m_Color.size() )
	{
		Color = GetMapValue( pProp->m_Color, Percent );
		rs.m_nMask |= PIECERS_DIFFUSE|PIECERS_AMBIENT;
		rs.m_Ambient = rs.m_Diffuse = Color|0xff000000;
	}

	if( pProp->m_bSetTextureMatrix )
	{
		rs.m_nMask |= PIECERS_SECONDTEX;
		CMatrix matTextureRotate,matTextureScale;
		float fCurTime = m_DeltaTime/1000.0f;
		rs.m_MatrixText.SetRotateZ( pProp->m_fTextAngularVel*fCurTime );
		rs.m_MatrixText._31 = pProp->m_vecTextMoveVel.x*fCurTime;
		rs.m_MatrixText._32 = pProp->m_vecTextMoveVel.y*fCurTime;
		if( pProp->m_Texture.size() )
			rs.m_pTexture = GetMapState( pProp->m_Texture, Percent*pProp->m_nTextureCircle - (int32)( Percent*pProp->m_nTextureCircle ) );
		rs.m_BlendAlpha = uint32( Color )&0xff000000;
	}
	else
		rs.m_Ambient = rs.m_Diffuse = Color;

	if( pProp->m_bSpecular )
	{
		rs.m_nMask |= PIECERS_SPECULAR;
		struct BYTE2POW
		{
			float m_Pow[256];
			BYTE2POW()
			{	
				for( int32 i = 0; i < 256; ++i )
					m_Pow[i] = pow( 10.0f, i/64.0f );	
			}
		};
		static BYTE2POW s_Pow;

		rs.m_Specular = pProp->m_SpecularColor;
		if( pProp->m_Scale.size() )
			rs.m_fPower   = s_Pow.m_Pow[ GetMapValue( pProp->m_Scale, Percent ) ];
	}

	if (!pProp->m_ClipDistMap.empty() && !pProp->m_ClipPlane.originPlane.m_Normal.IsZero())
	{
		pProp->m_ClipPlane.originPlane.m_Dist = -GetMapValue(pProp->m_ClipDistMap, Percent);
		rs.m_nMask |= PIECERS_CLIPPLANE;
		pProp->m_ClipPlane.SetTransform(matWorld);
		rs.m_ClipPlane = pProp->m_ClipPlane.transformedPlane;
	}

	if( m_pParent )
	{
		m_bNeedClear = true;
		for( size_t i = 0; i < pProp->m_ApplyClass.size(); ++i )
		{
			((CModel*)m_pParent)->SetPieceClassRS( pProp->m_ApplyClass[i].c_str(), rs );
		}
	}
}

//------------------------------------------------------------------------------
bool
CModelRenderStyle::SetProp( IEffectUnitProp* pEffectUnitProp )
{
	const CModelRenderStyleProp* pNewProp = (CModelRenderStyleProp*)pEffectUnitProp;
	pNewProp->m_ClipPlane.RenderDebug();
	return CEffectUnit::SetProp(pEffectUnitProp);
}

