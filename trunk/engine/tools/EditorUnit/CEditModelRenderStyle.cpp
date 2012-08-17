#include "stdafx.h"
#include "CEditModelRenderStyle.h"
#include "TSqrAllocator.inl"


CEditModelRenderStyleProp::CEditModelRenderStyleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CModelRenderStyleProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditModelRenderStyleProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditModelRenderStyleProp::SetUintPropData()
{
	EString ApplyClass;
	for( EVector<EString>::iterator it = m_ApplyClass.begin(); it != m_ApplyClass.end(); ++it )
	{
		if( it != m_ApplyClass.begin() )
			ApplyClass += ",";
		ApplyClass += *it;
	}

	_RegCtrlData(E_EVECESTRING, NULL, NULL, "关联的模型部分", NULL, &m_ApplyClass );
	_RegCtrlData(E_COLORSTRING, AutoUpdate, this, "高光颜色", NULL, &m_SpecularColor );
	_RegCtrlData(E_VECTOR2F, NULL, NULL, "贴图平移", NULL, &m_vecTextMoveVel );
	_RegCtrlData(E_FLOAT, NULL, NULL, "贴图旋转", NULL, &m_fTextAngularVel );
	_RegCtrlData(E_VECTOR, NULL, NULL, "裁剪面法向量", NULL, &(m_ClipPlane.originPlane.m_Normal) );
	_RegCtrlData(E_EFloatMap, NULL, NULL, "裁剪面距离", NULL, &m_ClipDistMap );

	// advanced prop 
	// EFADV_TEXTURE
	_RegCtrlData(E_EEStrMap, NULL, NULL, "贴图", NULL, &m_TextName );
	
	// EFADV_COLOR 
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color );

	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale );
	
}

int32 CEditModelRenderStyleProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	EString ApplyClass;
	for( EVector<EString>::iterator it = m_ApplyClass.begin(); it != m_ApplyClass.end(); ++it )
	{
		if( it != m_ApplyClass.begin() )
			ApplyClass += ",";
		ApplyClass += *it;
	}

	char Buf[32];
	Prop.push_back( EFFECT_PROP( "关联的模型部分",	sqr::CStringData( 0, ApplyClass.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "高光颜色",		sqr::CStringData( 0, _ultoa( uint32( m_SpecularColor ), Buf, 16 ) ) ) );
	Prop.push_back( EFFECT_PROP( "贴图平移",		CVector3f( m_vecTextMoveVel.x, m_vecTextMoveVel.y, 0 ) ) );
	Prop.push_back( EFFECT_PROP( "贴图旋转",		m_fTextAngularVel ) );
	Prop.push_back( EFFECT_PROP( "裁剪面法向量", m_ClipPlane.originPlane.m_Normal ) );
	Prop.push_back( EFFECT_PROP( "裁剪面距离", m_ClipDistMap ) ) ;

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )
		pProp[i] = Prop[i];
	return nBufLen;
}

void CEditModelRenderStyleProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	EString ApplyClass;

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "关联的模型部分" )	ApplyClass  = Prop[i].Data().Str();
		if( Prop[i].Name() == "高光颜色" )			m_SpecularColor = strtoul( ( "0x" + EString(Prop[i].Data().Str()) ).c_str(), 0, 0 );
		if( Prop[i].Name() == "贴图平移" )			m_vecTextMoveVel  = Prop[i].Data().Vec();
		if( Prop[i].Name() == "贴图旋转" )			m_fTextAngularVel  = Prop[i].Data().Float();
		if (Prop[i].Name() == "裁剪面法向量")		m_ClipPlane.originPlane.m_Normal = Prop[i].Data().Vec();
		if (Prop[i].Name() == "裁剪面距离")			m_ClipDistMap = *Prop[i].Data().FloatList();
	}

	m_ApplyClass.clear();

	size_t nPos = ApplyClass.find(",");
	while ( nPos != -1 )
	{
		m_ApplyClass.push_back(ApplyClass.substr(0, nPos));

		ApplyClass = ApplyClass.substr(nPos+1, ApplyClass.length()-nPos);
		nPos = ApplyClass.find(",");
	}

	m_ApplyClass.push_back(ApplyClass);

	_UpdateParams();
}

uint32 CEditModelRenderStyleProp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
											  ADVANCE_PROP* pTexture,   int32& nTextLen,
											  ADVANCE_PROP* pColor,     int32& nColorLen,
											  ADVANCE_PROP* pScale,     int32& nScaleLen, 
											  ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
											  ADVANCE_PROP* pUVRCenter, int32& nUVRCenter,
											  ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
											  ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius)//这个地方得重构了
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
	return EFADV_TEXTURE|EFADV_COLOR|EFADV_SCALE;
}

void CEditModelRenderStyleProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
											const ADVANCE_PROP* pTexture,   int32 nTextLen,
											const ADVANCE_PROP* pColor,     int32 nColorLen,
											const ADVANCE_PROP* pScale,     int32 nScaleLen,
											const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
											const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter,
											const ADVANCE_PROP* pEmiMinRad, int32& nEmissionMinRadius,
											const ADVANCE_PROP* pEmiMaxRad,   int32& nEmissionMaxRadius)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter );
	m_bSetTextureMatrix = m_Texture.size() && m_Texture.begin()->second;
}
