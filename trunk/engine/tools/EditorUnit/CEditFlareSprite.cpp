#include "stdafx.h"
#include "CEditFlareSprite.h"
#include "TSqrAllocator.inl"


CEditFlareSpriteProp::CEditFlareSpriteProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CFlareSpriteProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditFlareSpriteProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditFlareSpriteProp::SetUintPropData()
{
	_RegCtrlData(E_VECTOR, NULL, NULL, "旋转轴", NULL, &m_vecRotate);
	_RegCtrlData(E_VECTOR, NULL, NULL, "法线", NULL, &m_vecNormal);
	_RegCtrlData(E_FLOAT, AutoUpdate, this, "最大可见角", NULL, &m_fMaxAngle);
	_RegCtrlData(E_BYTE, AutoUpdate, this, "最小Alpha", NULL, &m_byMinAlpha);
	_RegCtrlData(E_BYTE, AutoUpdate, this, "最大Alpha", NULL, &m_byMaxAlpha);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最小尺寸", NULL, &m_fMinSize);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最大尺寸", NULL, &m_fMaxSize);
	
	CCombDataHelper* pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "朝向", NULL, &m_eFaceType);
	pHelper->PushItem("法线", SPRF_NORMAL );
	pHelper->PushItem("镜头", SPRF_CAMERA );
	pHelper->PushItem("场景", SPRF_CAMERANONEX );
	
	pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合模式", NULL, &m_eBlendModel );
	pHelper->PushItem("不混合", SPRB_NONE);
	pHelper->PushItem("相乘（暗）", SPRB_MULTIPLY);
	pHelper->PushItem("相加（亮）", SPRB_ADD);
	pHelper->PushItem("光效果", SPRB_LIGHT);

	pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "Z测试", NULL, &m_eZTest);
	pHelper->PushItem("关闭", Z_NONE);
	pHelper->PushItem("打开", Z_WRITE);;
	pHelper->PushItem("只读不写", Z_TESTONLY);;

	
	_RegCtrlData(E_BOOLValue, NULL, NULL, "打光", NULL, &m_bLightEnable);
	
	// advanced prop 
	// EFADV_COLOR
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color);

	// EFADV_TEXTURE
	_RegCtrlData(E_EEStrMap, NULL, NULL, "贴图", NULL, &m_TextName);

	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset);
}

int CEditFlareSpriteProp::GetProp( EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "旋转轴",		m_vecRotate ) );
	Prop.push_back( EFFECT_PROP( "法线",		m_vecNormal ) );
	Prop.push_back( EFFECT_PROP( "最大可见角",	m_fMaxAngle*180.0f/3.1415f ) );
	Prop.push_back( EFFECT_PROP( "最小Alpha",	(int)m_byMinAlpha ) );
	Prop.push_back( EFFECT_PROP( "最大Alpha",	(int)m_byMaxAlpha ) );
	Prop.push_back( EFFECT_PROP( "最小尺寸",	m_fMinSize ) );
	Prop.push_back( EFFECT_PROP( "最大尺寸",	m_fMaxSize ) );
	Prop.push_back( EFFECT_PROP( "朝向",		CComboData( (BYTE)m_eFaceType, "法线,镜头,场景" ) ) );
	Prop.push_back( EFFECT_PROP( "混合模式",	CComboData( (BYTE)m_eBlendModel,  "不混合,相乘（暗）,相加（亮）,光效果" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",		CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "打光",		CComboData( (BYTE)m_bLightEnable, "关闭,打开" ) ) );

	nBufLen = min( nBufLen,(int) Prop.size() );
	for( int i = 0; i < nBufLen; ++i )
		pProp[i] = Prop[i];

	return nBufLen;
}

void CEditFlareSpriteProp::SetProp( const EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( int i = 0; i < (int)Prop.size(); ++i )
	{
		if( Prop[i].Name() == "旋转轴" )			m_vecRotate = Prop[i].Data().Vec();
		if( Prop[i].Name() == "法线" )				m_vecNormal = Prop[i].Data().Vec();
		if( Prop[i].Name() == "最大可见角" )		m_fMaxAngle = Prop[i].Data().Float()*3.1415f/180.0f;
		if( Prop[i].Name() == "最小Alpha" )			m_byMinAlpha = (BYTE)Prop[i].Data().Int();
		if( Prop[i].Name() == "最大Alpha" )			m_byMaxAlpha = (BYTE)Prop[i].Data().Int();
		if( Prop[i].Name() == "最小尺寸" )			m_fMinSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "最大尺寸" )			m_fMaxSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "朝向" )				m_eFaceType = (SPR_FACE)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel = (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest = (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "打光" )				m_bLightEnable = !!Prop[i].Data().Index();
	}
	_UpdateParams();
}

uint32 CEditFlareSpriteProp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
										 ADVANCE_PROP* pTexture,   int32& nTextLen,
										 ADVANCE_PROP* pColor,     int32& nColorLen,
										 ADVANCE_PROP* pScale,     int32& nScaleLen, 
										 ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
										 ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);

	return EFADV_COLOR|EFADV_TEXTURE|EFADV_OFFSET;
}

void CEditFlareSpriteProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
									   const ADVANCE_PROP* pTexture,   int32 nTextLen,
									   const ADVANCE_PROP* pColor,     int32 nColorLen,
									   const ADVANCE_PROP* pScale,     int32 nScaleLen,
									   const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
									   const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, 0, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
}