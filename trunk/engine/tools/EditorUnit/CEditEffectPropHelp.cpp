#include "stdafx.h"
#include "CEditEffectPropHelp.h"


CEditUnitPropHelp::CEditUnitPropHelp()
{

}

CEditUnitPropHelp::~CEditUnitPropHelp()
{
	//m_pEffectUnitProp->Release();
}

int32 CEditUnitPropHelp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	return 0;
}

void CEditUnitPropHelp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	
}

void CEditUnitPropHelp::SetUnitProp(CEffectUnitProp* pProp)
{
	m_pEffectUnitProp = pProp;
}

void CEditUnitPropHelp::GetBaseProp( EVector< EFFECT_PROP >& Prop )
{
	EString str;
	for( ESet<EString>::iterator it = m_pEffectUnitProp->m_szAniName.begin(); it != m_pEffectUnitProp->m_szAniName.end(); ++it )
	{
		if( it != m_pEffectUnitProp->m_szAniName.begin() )
			str += ",";
		str += *it;
	}
	Prop.push_back( EFFECT_PROP( "连接点",			CComboData( (BYTE)m_pEffectUnitProp->m_eUpdatePos, "固定,跟随Socket偏移,跟随骨骼偏移,跟随Socket,跟随骨骼" ) ) );
	Prop.push_back( EFFECT_PROP( "连接对象名字",	CStringData( 0, m_pEffectUnitProp->m_szAttachName.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "关联的动画",		CStringData( 0, str.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "自定义类名",		CStringData( 0, m_pEffectUnitProp->m_EffectClassName.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "跟随加速",		bool( m_pEffectUnitProp->m_isUseVel ) ) );
	Prop.push_back( EFFECT_PROP( "贴图重复次数",	m_pEffectUnitProp->m_nTextureCircle ) );
	Prop.push_back( EFFECT_PROP( "缓慢消失",	bool(m_pEffectUnitProp->m_bSlownDisappear) ) );
}

void CEditUnitPropHelp::SetBaseProp( const EVector< EFFECT_PROP >& Prop )
{
	EString str;
	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "跟随加速" )			m_pEffectUnitProp->m_isUseVel =Prop[i].Data().Bool();
		if( Prop[i].Name() == "连接点" )			m_pEffectUnitProp->m_eUpdatePos = (eUpdatePosType)Prop[i].Data().Index();
		if( Prop[i].Name() == "连接对象名字" )		m_pEffectUnitProp->m_szAttachName = Prop[i].Data().Str();
		if( Prop[i].Name() == "关联的动画" )		str = Prop[i].Data().Str();
		if( Prop[i].Name() == "自定义类名" )		m_pEffectUnitProp->m_EffectClassName = Prop[i].Data().Str();
		if( Prop[i].Name() == "贴图重复次数" )		m_pEffectUnitProp->m_nTextureCircle = max( 1, Prop[i].Data().Int() );
		if( Prop[i].Name() == "缓慢消失" )			m_pEffectUnitProp->m_bSlownDisappear =Prop[i].Data().Bool();
	}

	m_pEffectUnitProp->m_szAniName.clear();
	char* pStr = (char*)str.c_str();
	for( int32 i = strlen( pStr) - 1; i > 0; --i )
	{
		if( pStr[i] == ',' )
		{
			pStr[i] = 0;
			EString Temp = pStr + i + 1;
			if( !Temp.empty() )
				m_pEffectUnitProp->m_szAniName.insert( Temp );
		}
	}
	m_pEffectUnitProp->m_szAniName.insert( pStr );
}

uint32 CEditUnitPropHelp::GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
										ADVANCE_PROP* pTexture, int32& nTextLen,
										ADVANCE_PROP* pColor,   int32& nColorLen,
										ADVANCE_PROP* pScale,   int32& nScaleLen )
{
	int32 i = 0;

	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_vecOffset.begin(); it != m_pEffectUnitProp->m_vecOffset.end() && i < nOffLen; ++it, ++i )
	{
		pOffset[i].fPercent = it->first;
		pOffset[i].Prop = it->second;
	}
	nOffLen = i;

	i = 0;
	for( EMap< float, EString >::iterator it = m_pEffectUnitProp->m_TextName.begin(); it != m_pEffectUnitProp->m_TextName.end() && i < nTextLen; ++it, ++i )
	{
		pTexture[i].fPercent = it->first;
		pTexture[i].Prop = it->second.c_str();
	}
	nTextLen = i;

	i = 0;
	for( EMap< float, CColor >::iterator it = m_pEffectUnitProp->m_Color.begin(); it != m_pEffectUnitProp->m_Color.end() && i < nColorLen; ++it, ++i )
	{
		pColor[i].fPercent = it->first;
		pColor[i].Prop = (uint32)it->second;
	}
	nColorLen = i;

	i = 0;
	for( EMap< float, BYTE >::iterator it = m_pEffectUnitProp->m_Scale.begin(); it != m_pEffectUnitProp->m_Scale.end() && i < nScaleLen; ++it, ++i )
	{
		pScale[i].fPercent = it->first;
		pScale[i].Prop = ( ( uint32) it->second ) << 24;
	}
	nScaleLen = i;

	return EFADV_TEXTURE|EFADV_COLOR|EFADV_SCALE|EFADV_OFFSET;
}

uint32 CEditUnitPropHelp::GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
										ADVANCE_PROP* pTexture, int32& nTextLen,
										ADVANCE_PROP* pColor,   int32& nColorLen,
										ADVANCE_PROP* pScale,   int32& nScaleLen,
										ADVANCE_PROP* pXYZcale,  int32& nXYZcale )
{
	int32 i = 0;

	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_vecOffset.begin(); it != m_pEffectUnitProp->m_vecOffset.end() && i < nOffLen; ++it, ++i )
	{
		pOffset[i].fPercent = it->first;
		pOffset[i].Prop = it->second;
	}
	nOffLen = i;

	i = 0;
	for( EMap< float, EString >::iterator it = m_pEffectUnitProp->m_TextName.begin(); it != m_pEffectUnitProp->m_TextName.end() && i < nTextLen; ++it, ++i )
	{
		pTexture[i].fPercent = it->first;
		pTexture[i].Prop = it->second.c_str();
	}
	nTextLen = i;

	i = 0;
	for( EMap< float, CColor >::iterator it = m_pEffectUnitProp->m_Color.begin(); it != m_pEffectUnitProp->m_Color.end() && i < nColorLen; ++it, ++i )
	{
		pColor[i].fPercent = it->first;
		pColor[i].Prop = (uint32)it->second;
	}
	nColorLen = i;

	i = 0;
	for( EMap< float, BYTE >::iterator it = m_pEffectUnitProp->m_Scale.begin(); it != m_pEffectUnitProp->m_Scale.end() && i < nScaleLen; ++it, ++i )
	{
		pScale[i].fPercent = it->first;
		pScale[i].Prop = ( ( uint32) it->second ) << 24;
	}
	nScaleLen = i;

	i = 0;
	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_XYZcale.begin(); it != m_pEffectUnitProp->m_XYZcale.end() && i < nXYZcale; ++it, ++i )
	{
		pXYZcale[i].fPercent = it->first;
		pXYZcale[i].Prop = it->second;
	}
	nXYZcale = i;

	return EFADV_TEXTURE|EFADV_COLOR|EFADV_SCALE|EFADV_OFFSET|EFADV_XYZCALE;
}

uint32 CEditUnitPropHelp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
										ADVANCE_PROP* pTexture,   int32& nTextLen,
										ADVANCE_PROP* pColor,     int32& nColorLen,
										ADVANCE_PROP* pScale,     int32& nScaleLen,
										ADVANCE_PROP* pXYZcale,   int32& nXYZcale,
										ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	int32 i = 0;

	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_vecOffset.begin(); it != m_pEffectUnitProp->m_vecOffset.end() && i < nOffLen; ++it, ++i )
	{
		pOffset[i].fPercent = it->first;
		pOffset[i].Prop = it->second;
	}
	nOffLen = i;

	i = 0;
	for( EMap< float, EString >::iterator it = m_pEffectUnitProp->m_TextName.begin(); it != m_pEffectUnitProp->m_TextName.end() && i < nTextLen; ++it, ++i )
	{
		pTexture[i].fPercent = it->first;
		pTexture[i].Prop = it->second.c_str();
	}
	nTextLen = i;

	i = 0;
	for( EMap< float, CColor >::iterator it = m_pEffectUnitProp->m_Color.begin(); it != m_pEffectUnitProp->m_Color.end() && i < nColorLen; ++it, ++i )
	{
		pColor[i].fPercent = it->first;
		pColor[i].Prop = (uint32)it->second;
	}
	nColorLen = i;

	i = 0;
	for( EMap< float, BYTE >::iterator it = m_pEffectUnitProp->m_Scale.begin(); it != m_pEffectUnitProp->m_Scale.end() && i < nScaleLen; ++it, ++i )
	{
		pScale[i].fPercent = it->first;
		pScale[i].Prop = ( ( uint32) it->second ) << 24;
	}
	nScaleLen = i;

	i = 0;
	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_XYZcale.begin(); it != m_pEffectUnitProp->m_XYZcale.end() && i < nXYZcale; ++it, ++i )
	{
		pXYZcale[i].fPercent = it->first;
		pXYZcale[i].Prop = it->second;
	}
	nXYZcale = i;

	i = 0;
	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_UVRCenter.begin(); it != m_pEffectUnitProp->m_UVRCenter.end() && i < nXYZcale; ++it, ++i )
	{
		pUVRCenter[i].fPercent = it->first;
		pUVRCenter[i].Prop = it->second;
	}
	nUVRCenter = i;

	return EFADV_TEXTURE|EFADV_COLOR|EFADV_SCALE|EFADV_OFFSET|EFADV_XYZCALE|EFADV_UVRCENTER;
}

uint32 CEditUnitPropHelp::GetAdvancedProp( ADVANCE_PROP* pOffset,  int32& nOffLen,
										ADVANCE_PROP* pTexture,   int32& nTextLen,
										ADVANCE_PROP* pColor,     int32& nColorLen,
										ADVANCE_PROP* pScale,     int32& nScaleLen, 
										ADVANCE_PROP* pXYZcale,   int32& nXYZcale,
										ADVANCE_PROP* pUVRCenter, int32& nUVRCenter,
										ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
										ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius)
{
	int32 i = 0;

	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_vecOffset.begin(); it != m_pEffectUnitProp->m_vecOffset.end() && i < nOffLen; ++it, ++i )
	{
		pOffset[i].fPercent = it->first;
		pOffset[i].Prop = it->second;
	}
	nOffLen = i;

	i = 0;
	for( EMap< float, EString >::iterator it = m_pEffectUnitProp->m_TextName.begin(); it != m_pEffectUnitProp->m_TextName.end() && i < nTextLen; ++it, ++i )
	{
		pTexture[i].fPercent = it->first;
		pTexture[i].Prop = it->second.c_str();
	}
	nTextLen = i;

	i = 0;
	for( EMap< float, CColor >::iterator it = m_pEffectUnitProp->m_Color.begin(); it != m_pEffectUnitProp->m_Color.end() && i < nColorLen; ++it, ++i )
	{
		pColor[i].fPercent = it->first;
		pColor[i].Prop = (uint32)it->second;
	}
	nColorLen = i;

	i = 0;
	for( EMap< float, BYTE >::iterator it = m_pEffectUnitProp->m_Scale.begin(); it != m_pEffectUnitProp->m_Scale.end() && i < nScaleLen; ++it, ++i )
	{
		pScale[i].fPercent = it->first;
		pScale[i].Prop = ( ( uint32) it->second ) << 24;
	}
	nScaleLen = i;

	i = 0;
	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_XYZcale.begin(); it != m_pEffectUnitProp->m_XYZcale.end() && i < nXYZcale; ++it, ++i )
	{
		pXYZcale[i].fPercent = it->first;
		pXYZcale[i].Prop = it->second;
	}
	nXYZcale = i;

	i = 0;
	for( EMap< float, CVector3f >::iterator it =	m_pEffectUnitProp->m_UVRCenter.begin(); it != m_pEffectUnitProp->m_UVRCenter.end() && i < nUVRCenter; ++it, ++i )
	{
		pUVRCenter[i].fPercent = it->first;
		pUVRCenter[i].Prop = it->second;
	}
	nUVRCenter = i;

	i = 0;
	for( EMap< float, float >::iterator it =	m_pEffectUnitProp->m_EmissionMinRadius.begin(); it != m_pEffectUnitProp->m_EmissionMinRadius.end() && i < nEmissionMinRadius; ++it, ++i )
	{
		pEmiMinRad[i].fPercent = it->first;
		pEmiMinRad[i].Prop = it->second;
	}
	nEmissionMinRadius = i;

	i = 0;
	for( EMap< float, float >::iterator it =	m_pEffectUnitProp->m_EmissionMaxRadius.begin(); it != m_pEffectUnitProp->m_EmissionMaxRadius.end() && i < nEmissionMaxRadius; ++it, ++i )
	{
		pEmiMaxRad[i].fPercent = it->first;
		pEmiMaxRad[i].Prop = it->second;
	}
	nEmissionMaxRadius = i;

	return EFADV_TEXTURE|EFADV_COLOR|EFADV_SCALE|EFADV_OFFSET|EFADV_XYZCALE|EFADV_UVRCENTER|EFADV_EMIMINRAD|EFADV_EMIMAXRAD;
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									  const ADVANCE_PROP* pTexture, int32 nTextLen,
									  const ADVANCE_PROP* pColor,   int32 nColorLen,
									  const ADVANCE_PROP* pScale,   int32 nScaleLen )
{
#ifdef _DEBUG
	EMap< float, EString > Name = m_pEffectUnitProp->m_TextName;
#endif

	m_pEffectUnitProp->m_vecOffset.clear();
	m_pEffectUnitProp->m_TextName.clear();
	m_pEffectUnitProp->m_Color.clear();
	m_pEffectUnitProp->m_Scale.clear();

	for( int32 i = 0; i < nOffLen; ++i )
		m_pEffectUnitProp->m_vecOffset[pOffset[i].fPercent] = pOffset[i].Prop.Vec();
	for( int32 i = 0; i < nTextLen; ++i )
		m_pEffectUnitProp->m_TextName[pTexture[i].fPercent] = pTexture[i].Prop.CStr();
	for( int32 i = 0; i < nColorLen; ++i )
		m_pEffectUnitProp->m_Color[pColor[i].fPercent] = pColor[i].Prop.DWord();
	for( int32 i = 0; i < nScaleLen; ++i )
		m_pEffectUnitProp->m_Scale[pScale[i].fPercent] = (BYTE)( pScale[i].Prop.DWord()>>24 );
	m_pEffectUnitProp->LoadFullRes();
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									  const ADVANCE_PROP* pTexture, int32 nTextLen,
									  const ADVANCE_PROP* pColor,   int32 nColorLen,
									  const ADVANCE_PROP* pScale,   int32 nScaleLen,
									  const ADVANCE_PROP* pXYZcale,  int32 nXYZcale)
{
#ifdef _DEBUG
	EMap< float, EString > Name = m_pEffectUnitProp->m_TextName;
#endif

	m_pEffectUnitProp->m_vecOffset.clear();
	m_pEffectUnitProp->m_TextName.clear();
	m_pEffectUnitProp->m_Color.clear();
	m_pEffectUnitProp->m_Scale.clear();
	m_pEffectUnitProp->m_XYZcale.clear();

	for( int32 i = 0; i < nOffLen; ++i )
		m_pEffectUnitProp->m_vecOffset[pOffset[i].fPercent] = pOffset[i].Prop.Vec();
	for( int32 i = 0; i < nTextLen; ++i )
		m_pEffectUnitProp->m_TextName[pTexture[i].fPercent] = pTexture[i].Prop.CStr();
	for( int32 i = 0; i < nColorLen; ++i )
		m_pEffectUnitProp->m_Color[pColor[i].fPercent] = pColor[i].Prop.DWord();
	for( int32 i = 0; i < nScaleLen; ++i )
		m_pEffectUnitProp->m_Scale[pScale[i].fPercent] = (BYTE)( pScale[i].Prop.DWord()>>24 );
	for( int32 i = 0; i < nXYZcale; ++i )
		m_pEffectUnitProp->m_XYZcale[pXYZcale[i].fPercent] = pXYZcale[i].Prop.Vec();

	m_pEffectUnitProp->LoadFullRes();
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									  const ADVANCE_PROP* pTexture,   int32 nTextLen,
									  const ADVANCE_PROP* pColor,     int32 nColorLen,
									  const ADVANCE_PROP* pScale,     int32 nScaleLen,
									  const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
									  const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
#ifdef _DEBUG
	EMap< float, EString > Name = m_pEffectUnitProp->m_TextName;
#endif

	m_pEffectUnitProp->m_vecOffset.clear();
	m_pEffectUnitProp->m_TextName.clear();
	m_pEffectUnitProp->m_Color.clear();
	m_pEffectUnitProp->m_Scale.clear();
	m_pEffectUnitProp->m_XYZcale.clear();
	m_pEffectUnitProp->m_UVRCenter.clear();

	for( int32 i = 0; i < nOffLen; ++i )
		m_pEffectUnitProp->m_vecOffset[pOffset[i].fPercent] = pOffset[i].Prop.Vec();
	for( int32 i = 0; i < nTextLen; ++i )
		m_pEffectUnitProp->m_TextName[pTexture[i].fPercent] = pTexture[i].Prop.CStr();
	for( int32 i = 0; i < nColorLen; ++i )
		m_pEffectUnitProp->m_Color[pColor[i].fPercent] = pColor[i].Prop.DWord();
	for( int32 i = 0; i < nScaleLen; ++i )
		m_pEffectUnitProp->m_Scale[pScale[i].fPercent] = (BYTE)( pScale[i].Prop.DWord()>>24 );
	for( int32 i = 0; i < nXYZcale; ++i )
		m_pEffectUnitProp->m_XYZcale[pXYZcale[i].fPercent] = pXYZcale[i].Prop.Vec();
	for( int32 i = 0; i < nUVRCenter; ++i )
		m_pEffectUnitProp->m_UVRCenter[pXYZcale[i].fPercent] = pUVRCenter[i].Prop.Vec();

	m_pEffectUnitProp->LoadFullRes();
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									  const ADVANCE_PROP* pTexture,   int32 nTextLen,
									  const ADVANCE_PROP* pColor,     int32 nColorLen,
									  const ADVANCE_PROP* pScale,     int32 nScaleLen,
									  const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
									  const ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
									  const ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius)
{
#ifdef _DEBUG
	EMap< float, EString > Name = m_pEffectUnitProp->m_TextName;
#endif

	m_pEffectUnitProp->m_vecOffset.clear();
	m_pEffectUnitProp->m_TextName.clear();
	m_pEffectUnitProp->m_Color.clear();
	m_pEffectUnitProp->m_Scale.clear();
	m_pEffectUnitProp->m_XYZcale.clear();
	m_pEffectUnitProp->m_UVRCenter.clear();
	m_pEffectUnitProp->m_EmissionMinRadius.clear();
	m_pEffectUnitProp->m_EmissionMaxRadius.clear();

	for( int32 i = 0; i < nOffLen; ++i )
		m_pEffectUnitProp->m_vecOffset[pOffset[i].fPercent] = pOffset[i].Prop.Vec();
	for( int32 i = 0; i < nTextLen; ++i )
		m_pEffectUnitProp->m_TextName[pTexture[i].fPercent] = pTexture[i].Prop.CStr();
	for( int32 i = 0; i < nColorLen; ++i )
		m_pEffectUnitProp->m_Color[pColor[i].fPercent] = pColor[i].Prop.DWord();
	for( int32 i = 0; i < nScaleLen; ++i )
		m_pEffectUnitProp->m_Scale[pScale[i].fPercent] = (BYTE)( pScale[i].Prop.DWord()>>24 );
	for( int32 i = 0; i < nXYZcale; ++i )
		m_pEffectUnitProp->m_XYZcale[pXYZcale[i].fPercent] = pXYZcale[i].Prop.Vec();
	for( int32 i = 0; i < nEmissionMinRadius; ++i )
		m_pEffectUnitProp->m_EmissionMinRadius[pEmiMinRad[i].fPercent] = pEmiMinRad[i].Prop.Float();
	for( int32 i = 0; i < nEmissionMaxRadius; ++i )
		m_pEffectUnitProp->m_EmissionMaxRadius[pEmiMaxRad[i].fPercent] = pEmiMaxRad[i].Prop.Float();

	m_pEffectUnitProp->LoadFullRes();
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									  const ADVANCE_PROP* pTexture,   int32 nTextLen,
									  const ADVANCE_PROP* pColor,     int32 nColorLen,
									  const ADVANCE_PROP* pScale,     int32 nScaleLen,
									  const ADVANCE_PROP* pXYZcale,   int32 nXYZcale,
									  const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter,
									  const ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
									  const ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius)
{
#ifdef _DEBUG
	EMap< float, EString > Name = m_pEffectUnitProp->m_TextName;
#endif

	m_pEffectUnitProp->m_vecOffset.clear();
	m_pEffectUnitProp->m_TextName.clear();
	m_pEffectUnitProp->m_Color.clear();
	m_pEffectUnitProp->m_Scale.clear();
	m_pEffectUnitProp->m_XYZcale.clear();
	m_pEffectUnitProp->m_UVRCenter.clear();
	m_pEffectUnitProp->m_EmissionMinRadius.clear();
	m_pEffectUnitProp->m_EmissionMaxRadius.clear();

	for( int32 i = 0; i < nOffLen; ++i )
		m_pEffectUnitProp->m_vecOffset[pOffset[i].fPercent] = pOffset[i].Prop.Vec();
	for( int32 i = 0; i < nTextLen; ++i )
		m_pEffectUnitProp->m_TextName[pTexture[i].fPercent] = pTexture[i].Prop.CStr();
	for( int32 i = 0; i < nColorLen; ++i )
		m_pEffectUnitProp->m_Color[pColor[i].fPercent] = pColor[i].Prop.DWord();
	for( int32 i = 0; i < nScaleLen; ++i )
		m_pEffectUnitProp->m_Scale[pScale[i].fPercent] = (BYTE)( pScale[i].Prop.DWord()>>24 );
	for( int32 i = 0; i < nXYZcale; ++i )
		m_pEffectUnitProp->m_XYZcale[pXYZcale[i].fPercent] = pXYZcale[i].Prop.Vec();
	for( int32 i = 0; i < nUVRCenter; ++i )
		m_pEffectUnitProp->m_UVRCenter[pXYZcale[i].fPercent] = pUVRCenter[i].Prop.Vec();
	for( int32 i = 0; i < nEmissionMinRadius; ++i )
		m_pEffectUnitProp->m_EmissionMinRadius[pEmiMinRad[i].fPercent] = pEmiMinRad[i].Prop.Float();
	for( int32 i = 0; i < nEmissionMaxRadius; ++i )
		m_pEffectUnitProp->m_EmissionMaxRadius[pEmiMaxRad[i].fPercent] = pEmiMaxRad[i].Prop.Float();

	m_pEffectUnitProp->LoadFullRes();
}

void CEditUnitPropHelp::SetAdvancedProp( const ADVANCE_PROP* pEmiMinRad, int32 nEmissionMinRadius,
									  const ADVANCE_PROP* pEmiMaxRad,   int32 nEmissionMaxRadius)
{
	m_pEffectUnitProp->m_EmissionMinRadius.clear();
	m_pEffectUnitProp->m_EmissionMaxRadius.clear();

	for( int32 i = 0; i < nEmissionMinRadius; ++i )
		m_pEffectUnitProp->m_EmissionMinRadius[pEmiMinRad[i].fPercent] = pEmiMinRad[i].Prop.Float();
	for( int32 i = 0; i < nEmissionMaxRadius; ++i )
		m_pEffectUnitProp->m_EmissionMaxRadius[pEmiMaxRad[i].fPercent] = pEmiMaxRad[i].Prop.Float();
}
