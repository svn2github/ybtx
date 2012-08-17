#include "stdafx.h"
#include "CEditCameraFilter.h"
#include "TSqrAllocator.inl"

CEditCameraFilterProp::CEditCameraFilterProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CCameraFilterProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

CEditCameraFilterProp::~CEditCameraFilterProp(void)
{
}

void CEditCameraFilterProp::SetUintPropData()
{
	_RegCtrlData(E_INTEGER, NULL, NULL, "渐显时间", NULL, &m_BirthTime);
	_RegCtrlData(E_INTEGER, NULL, NULL, "渐隐时间", NULL, &m_DeleteTime);
	_RegCtrlData(E_BOOLValue, NULL, NULL, "不过滤模型", NULL, &m_bFilterParent);

	CCombDataHelper* pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "比例设置", NULL, &m_eCFilterTp );
	pHelper->PushItem("平铺", CAMERAFILTER_RATIO_NONE);
	pHelper->PushItem("保持比例", CAMERAFILTER_RATIO_CLAMP);
	
	pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合模式", NULL, &m_eBlendModel);
	pHelper->PushItem("不混合", SPRB_NONE);
	pHelper->PushItem("相乘（暗）", SPRB_MULTIPLY);
	pHelper->PushItem("相加（亮）", SPRB_ADD);

	pHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "Z测试", NULL, &m_eZTest);
	pHelper->PushItem("关闭", Z_NONE);
	pHelper->PushItem("打开", Z_WRITE);
	pHelper->PushItem("只读不写", Z_TESTONLY);


	_RegCtrlData(E_FLOAT, NULL, NULL, "影响范围宽度x", NULL, &m_xScale);
	_RegCtrlData(E_FLOAT, NULL, NULL, "影响范围高度y", NULL, &m_yScale);
	_RegCtrlData(E_FLOAT, NULL, NULL, "影响范围深度z", NULL, &m_zScale);
	// advanced prop 
	// EFADV_TEXTURE
	_RegCtrlData(E_EEStrMap, NULL, NULL, "贴图", NULL, &m_TextName);
	// EFADV_COLOR
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color);
}

int32 CEditCameraFilterProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "渐显时间",		m_BirthTime ) );
	Prop.push_back( EFFECT_PROP( "渐隐时间",		m_DeleteTime ) );
	Prop.push_back( EFFECT_PROP( "不过滤模型",		m_bFilterParent ) );
	Prop.push_back( EFFECT_PROP( "比例设置",		CComboData( (BYTE)m_eCFilterTp, "平铺,保持比例" ) ) );
	Prop.push_back( EFFECT_PROP( "混合模式",		CComboData( (BYTE)m_eBlendModel,  "不混合,相乘（暗）,相加（亮）" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",			CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "影响范围宽度x",	m_xScale ) );
	Prop.push_back( EFFECT_PROP( "影响范围高度y",	m_yScale ) );
	Prop.push_back( EFFECT_PROP( "影响范围深度z",	m_zScale ) );

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; i++ )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditCameraFilterProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "渐显时间" )			m_BirthTime		= Prop[i].Data().Int();
		if( Prop[i].Name() == "渐隐时间" )			m_DeleteTime	= Prop[i].Data().Int();
		if( Prop[i].Name() == "不过滤模型" )		m_bFilterParent	= Prop[i].Data().Bool();
		if( Prop[i].Name() == "比例设置" )			m_eCFilterTp	= (CAMERAFILTER_RATIO)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel	= (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest		= (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "影响范围宽度x" )		m_xScale		= fabs( Prop[i].Data().Float() );
		if( Prop[i].Name() == "影响范围高度y" )		m_yScale		= fabs( Prop[i].Data().Float() );
		if( Prop[i].Name() == "影响范围深度z" )		m_zScale		= fabs( Prop[i].Data().Float() );
	}
	_UpdateParams();
}

uint32 CEditCameraFilterProp::GetAdvancedProp( ADVANCE_PROP* pOffset,      int32& nOffLen,
										  ADVANCE_PROP* pTexture,   int32& nTextLen,
										  ADVANCE_PROP* pColor,     int32& nColorLen,
										  ADVANCE_PROP* pScale,     int32& nScaleLen,
										  ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
										  ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen ,pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);

	return EFADV_TEXTURE|EFADV_COLOR;
}