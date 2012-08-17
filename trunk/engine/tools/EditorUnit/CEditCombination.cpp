#include "stdafx.h"
#include "CEditCombination.h"
#include "IEffectGroup.h"
#include "TSqrAllocator.inl"

CEditCombinationProp::CEditCombinationProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CCombinationProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditCombinationProp::_UIDataUpdate()
{
	_UpdateParams();
	CCombDataHelper* pHelper = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "特效名", NULL, &m_nEffectIndex);
	pHelper->PushItem("None", 0);
	if( m_pEffectGroup )
	{
		uint32 nEffectNum = m_pEffectGroup->GetEffectNum();
		for ( uint32 i = 0; i < nEffectNum; ++i )
		{
			EString strEffectName = m_pEffectGroup->GetEffectName(i);
			pHelper->PushItem(strEffectName.c_str(), i+1);
		}
	}
}

void CEditCombinationProp::SetUintPropData()
{
	_RegCtrlData(E_ESTRINGBUTTON, AutoUpdate, this, "特效文件", "特效文件(*.efx)|*.efx", &m_strEffectFileName);
	CCombDataHelper* pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "特效名", NULL, &m_nEffectIndex);
	pHelper->PushItem("None", 0);
	if( m_pEffectGroup )
	{
		uint32 nEffectNum = m_pEffectGroup->GetEffectNum();
		for ( uint32 i = 0; i < nEffectNum; ++i )
		{
			EString strEffectName = m_pEffectGroup->GetEffectName(i);
			pHelper->PushItem(strEffectName.c_str(), i+1);
		}
	}

	// advanced prop 
	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale);
	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset);
}

int32 CEditCombinationProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	EString effectNameList = "None";
	if( m_pEffectGroup )
	{
		uint32 nEffectNum = m_pEffectGroup->GetEffectNum();
		for ( uint32 i = 0; i < nEffectNum; ++i )
		{
			effectNameList += ",";
			EString strEffectName = m_pEffectGroup->GetEffectName(i);
			effectNameList += strEffectName;
		}
	}
	if( effectNameList.size() > 120 )
		effectNameList[120] = 0;

	Prop.push_back( EFFECT_PROP( "特效文件", sqr::CStringData( 3, m_strEffectFileName ) ) );
	Prop.push_back( EFFECT_PROP( "特效名",	 CComboData( (BYTE)m_nEffectIndex, effectNameList .c_str() ) ) );

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for ( int32 i = 0; i < nBufLen; ++i )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditCombinationProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for ( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "特效文件" )	m_strEffectFileName	= Prop[i].Data().Str();
		if( Prop[i].Name() == "特效名" )	m_nEffectIndex = Prop[i].Data().Index();
	}

	_UpdateParams();
}

uint32 CEditCombinationProp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
									   ADVANCE_PROP* pTexture,   int32& nTextLen,
									   ADVANCE_PROP* pColor,     int32& nColorLen,
									   ADVANCE_PROP* pScale,     int32& nScaleLen,
									   ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
									   ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);

	return EFADV_SCALE|EFADV_OFFSET;
}

void CEditCombinationProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
									 const ADVANCE_PROP* pTexture,   int32 nTextLen,
									 const ADVANCE_PROP* pColor,     int32 nColorLen,
									 const ADVANCE_PROP* pScale,     int32 nScaleLen,
									 const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
									 const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, 0, pColor, 0, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
}