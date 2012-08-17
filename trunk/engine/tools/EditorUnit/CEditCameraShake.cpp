#include "stdafx.h"
#include "CEditCameraShake.h"
#include "TSqrAllocator.inl"

CEditCameraShakeProp::CEditCameraShakeProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CCameraShakeProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditCameraShakeProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditCameraShakeProp::SetUintPropData()
{
	_RegCtrlData(E_FLOAT, NULL, NULL, "频率", NULL, &m_fFrequence);
	_RegCtrlData(E_FLOAT, NULL, NULL, "强度", NULL, &m_fStrength);
	_RegCtrlData(E_FLOAT, AutoUpdate, this, "无衰减半径", NULL, &m_fMinRadius);
	_RegCtrlData(E_FLOAT, AutoUpdate, this, "最大作用半径", NULL, &m_fMaxRadius);

	// advanced prop 
	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale);
	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset);
}

int32 CEditCameraShakeProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );
	Prop.push_back( EFFECT_PROP( "频率",			m_fFrequence ) );
	Prop.push_back( EFFECT_PROP( "强度",			m_fStrength ) );
	Prop.push_back( EFFECT_PROP( "无衰减半径",		m_fMinRadius ) );
	Prop.push_back( EFFECT_PROP( "最大作用半径",	m_fMaxRadius ) );

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditCameraShakeProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "频率" )				m_fFrequence	= Prop[i].Data().Float();
		if( Prop[i].Name() == "强度" )				m_fStrength 	= Prop[i].Data().Float();
		if( Prop[i].Name() == "无衰减半径" )		m_fMinRadius	= Prop[i].Data().Float();
		if( Prop[i].Name() == "最大作用半径" )		m_fMaxRadius	= Prop[i].Data().Float();
	}
	_UpdateParams();
}

uint32 CEditCameraShakeProp::GetAdvancedProp( ADVANCE_PROP* pOffset,    int32& nOffLen,
										 ADVANCE_PROP* pTexture,   int32& nTextLen,
										 ADVANCE_PROP* pColor,     int32& nColorLen,
										 ADVANCE_PROP* pScale,     int32& nScaleLen, 
										 ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
										 ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);

	return EFADV_SCALE|EFADV_OFFSET;
}

void CEditCameraShakeProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
									   const ADVANCE_PROP* pTexture,  int32 nTextLen,
									   const ADVANCE_PROP* pColor,    int32 nColorLen,
									   const ADVANCE_PROP* pScale,    int32 nScaleLen,
									   const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
									   const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, 0, pColor, 0, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
}