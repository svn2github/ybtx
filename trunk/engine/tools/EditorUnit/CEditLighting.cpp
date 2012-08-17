#include "stdafx.h"
#include "CEditLighting.h"
#include "TSqrAllocator.inl"


CEditLightningProp::CEditLightningProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CLightningProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditLightningProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditLightningProp::SetUintPropData()
{
	_RegCtrlData(E_VECTOR, NULL, NULL, "旋转轴", NULL, &m_vecRotate);
	_RegCtrlData(E_VECTOR, NULL, NULL, "法线", NULL, &m_vecNormal);
	_RegCtrlData(E_FLOAT, NULL, NULL, "旋转起始角", NULL, &m_fStartAngle);
	_RegCtrlData(E_FLOAT, NULL, NULL, "长宽比", NULL, &m_fWidthRatio);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最小尺寸", NULL, &m_fMinSize);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最大尺寸", NULL, &m_fMaxSize);
	_RegCtrlData(E_VECTOR, NULL, NULL, "起点", NULL, &m_vecSrc);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最小震动幅度", NULL, &m_fDitherMin);
	_RegCtrlData(E_FLOAT, NULL, NULL, "最大震动幅度", NULL, &m_fDitherMax);
	_RegCtrlData(E_INTEGER, NULL, NULL, "分段数目", NULL, &m_nSubNum);
	
	CCombDataHelper* pCombHelper = (CCombDataHelper* )_RegCtrlData(E_COMBO, NULL, NULL, "朝向", NULL, &m_eFaceType);
	pCombHelper->PushItem("法线", SPRF_NORMAL);
	pCombHelper->PushItem("镜头", SPRF_CAMERA);
	pCombHelper->PushItem("场景", SPRF_CAMERANONEX);
	
	pCombHelper = (CCombDataHelper* )_RegCtrlData(E_COMBO, NULL, NULL, "混合模式", NULL, &m_eBlendModel);
	pCombHelper->PushItem("不混合", SPRB_NONE);
	pCombHelper->PushItem("相乘（暗）", SPRB_MULTIPLY);
	pCombHelper->PushItem("相加（亮）", SPRB_ADD);
	pCombHelper->PushItem("光效果", SPRB_LIGHT);

	pCombHelper = (CCombDataHelper* )_RegCtrlData(E_COMBO, NULL, NULL, "Z测试", NULL, &m_eZTest);
	pCombHelper->PushItem("关闭", Z_NONE);
	pCombHelper->PushItem("打开", Z_WRITE);
	pCombHelper->PushItem("只读不写", Z_TESTONLY);
	
	_RegCtrlData(E_BOOLValue, NULL, NULL, "打光", NULL, &m_bLightEnable);
	_RegCtrlData(E_FLOAT, NULL, NULL, "深度偏移", NULL, &m_fZBias);

	// advanced prop 
	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset);

	// EFADV_TEXTURE
	_RegCtrlData(E_EEStrMap, NULL, NULL, "贴图", NULL, &m_TextName);

	// EFADV_COLOR   
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color);

	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale);

	// EFADV_XYZCALE;
	_RegCtrlData(E_EVectorMap, NULL, NULL, "XYZ缩放", NULL, &m_XYZcale);

	// EFADV_XYZCALE;
	_RegCtrlData(E_EVectorMap, NULL, NULL, "纹理旋转中心", NULL, &m_UVRCenter);
	
}

int CEditLightningProp::GetProp( EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "旋转轴",		m_vecRotate ) );
	Prop.push_back( EFFECT_PROP( "法线",		m_vecNormal ) );
	Prop.push_back( EFFECT_PROP( "旋转起始角",	m_fStartAngle ) );
	Prop.push_back( EFFECT_PROP( "长宽比",		m_fWidthRatio ) );
	Prop.push_back( EFFECT_PROP( "最小尺寸",	m_fMinSize ) );
	Prop.push_back( EFFECT_PROP( "最大尺寸",	m_fMaxSize ) );
	Prop.push_back( EFFECT_PROP( "起点",		m_vecSrc ) );
	Prop.push_back( EFFECT_PROP( "最小震动幅度",m_fDitherMin ) );
	Prop.push_back( EFFECT_PROP( "最大震动幅度",m_fDitherMax ) );
	Prop.push_back( EFFECT_PROP( "分段数目",	m_nSubNum ) );
	Prop.push_back( EFFECT_PROP( "朝向",		CComboData( (BYTE)m_eFaceType, "法线,镜头,场景" ) ) );
	Prop.push_back( EFFECT_PROP( "混合模式",	CComboData( (BYTE)m_eBlendModel,  "不混合,相乘（暗）,相加（亮）,光效果" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",		CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "打光",		CComboData( (BYTE)m_bLightEnable, "关闭,打开" ) ) );
	Prop.push_back( EFFECT_PROP( "深度偏移",	m_fZBias ) );

	nBufLen = min( nBufLen,(int) Prop.size() );
	for( int i = 0; i < nBufLen; i++ )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditLightningProp::SetProp( const EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( int i = 0; i < (int)Prop.size(); ++i )
	{
		if( Prop[i].Name() == "旋转轴" )			m_vecRotate = Prop[i].Data().Vec();
		if( Prop[i].Name() == "法线" )				m_vecNormal = Prop[i].Data().Vec();
		if( Prop[i].Name() == "旋转起始角" )		m_fStartAngle  = Prop[i].Data().Float();
		if( Prop[i].Name() == "长宽比" )			m_fWidthRatio  = Prop[i].Data().Float();
		if( Prop[i].Name() == "最小尺寸" )			m_fMinSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "最大尺寸" )			m_fMaxSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "起点" )				m_vecSrc  = Prop[i].Data().Vec();
		if( Prop[i].Name() == "最小震动幅度" )		m_fDitherMin  = Prop[i].Data().Float();
		if( Prop[i].Name() == "最大震动幅度" )		m_fDitherMax  = max(Prop[i].Data().Float(), 1);
		if( Prop[i].Name() == "分段数目" )			m_nSubNum  = max( Prop[i].Data().Int(), 1 );
		if( Prop[i].Name() == "朝向" )				m_eFaceType = (SPR_FACE)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel = (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest = (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "打光" )				m_bLightEnable = Prop[i].Data().Index() != 0;
		if( Prop[i].Name() == "深度偏移" )			m_fZBias = Prop[i].Data().Float();
	}
	_UpdateParams();
}