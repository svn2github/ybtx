#include "stdafx.h"
#include "CEditDynamicLight.h"
#include "TSqrAllocator.inl"


CEditDynamicLightProp::CEditDynamicLightProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CDynamicLightProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

CEditDynamicLightProp::~CEditDynamicLightProp(void)
{

}

void CEditDynamicLightProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditDynamicLightProp::SetUintPropData()
{
	_RegCtrlData(E_FLOAT,  NULL, NULL, "范围", NULL, &m_fRange);
	_RegCtrlData(E_FLOAT, AutoUpdate, this, "最小强度", NULL, &m_fMinStrong);
	_RegCtrlData(E_FLOAT, AutoUpdate, this, "最大强度", NULL, &m_fMaxStrong);

	// advanced prop 
	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale);
	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset);
	// EFADV_COLOR
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color);
}

int32 CEditDynamicLightProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "范围",				m_fRange ) );
	Prop.push_back( EFFECT_PROP( "最小强度",			m_fMinStrong ) );
	Prop.push_back( EFFECT_PROP( "最大强度",			m_fMaxStrong ) );

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditDynamicLightProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "范围" )				m_fRange = Prop[i].Data().Float();
		if( Prop[i].Name() == "最小强度" )			m_fMinStrong = max( 1.0f, Prop[i].Data().Float() );
		if( Prop[i].Name() == "最大强度" )			m_fMaxStrong = Prop[i].Data().Float();
	}
	_UpdateParams();
}

uint32 CEditDynamicLightProp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
										  ADVANCE_PROP* pTexture,   int32& nTextLen,
										  ADVANCE_PROP* pColor,     int32& nColorLen,
										  ADVANCE_PROP* pScale,     int32& nScaleLen, 
										  ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
										  ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter );

	return EFADV_SCALE|EFADV_OFFSET|EFADV_COLOR;
}

void CEditDynamicLightProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
										const ADVANCE_PROP* pTexture,   int32 nTextLen,
										const ADVANCE_PROP* pColor,     int32 nColorLen,
										const ADVANCE_PROP* pScale,     int32 nScaleLen,
										const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
										const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, 0, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
}