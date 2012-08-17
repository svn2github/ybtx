#include "stdafx.h"
#include "CEditPolyTrail.h"
#include "TSqrAllocator.inl"


CEditPolyTrailProp::CEditPolyTrailProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CPolyTrailProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditPolyTrailProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditPolyTrailProp::SetUintPropData()
{
	_RegCtrlData(E_VECTOR, NULL, NULL, "旋转轴", NULL, &m_vecRotate );
	_RegCtrlData(E_FLOAT, NULL, NULL, "轨迹最小宽度", NULL, &m_MinTrailWidth );
	_RegCtrlData(E_FLOAT, NULL, NULL, "轨迹最大宽度", NULL, &m_MaxTrailWidth );
	_RegCtrlData(E_INTEGER, NULL, NULL, "轨迹单元生命周期", NULL, &m_UnitLifeSpan );
	_RegCtrlData(E_INTEGER, NULL, NULL, "轨迹结束衰减速度", NULL, &m_DeclTime );
	_RegCtrlData(E_INTEGER, NULL, NULL, "圆滑度", NULL, &m_Smooth );

	CCombDataHelper* pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合模式", NULL, &m_eBlendModel);
	pCombHelper->PushItem("不混合", SPRB_NONE);
	pCombHelper->PushItem("相乘（暗）", SPRB_MULTIPLY);
	pCombHelper->PushItem("相加（亮）", SPRB_ADD);

	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合OP", NULL, &m_eTextOp);
	pCombHelper->PushItem("纯纹理", SPRO_SELECTARG1);
	pCombHelper->PushItem("纯颜色", SPRO_SELECTARG2);
	pCombHelper->PushItem("1倍乘", SPRO_MODULATE);
	pCombHelper->PushItem("2倍乘", SPRO_MODULATE2X);
	pCombHelper->PushItem("4倍乘", SPRO_MODULATE4X);
	pCombHelper->PushItem("加色", SPRO_ADD);
	pCombHelper->PushItem("加色暗", SPRO_ADDSIGNED);
	pCombHelper->PushItem("2倍加色", SPRO_ADDSIGNED2X);
	pCombHelper->PushItem("减色", SPRO_SUBTRACT);
	pCombHelper->PushItem("平滑加色", SPRO_ADDSMOOTH);
	pCombHelper->PushItem("调色混合", SPRO_BLENDDIFFUSEALPHA);
	pCombHelper->PushItem("贴图混合", SPRO_BLENDTEXTUREALPHA);
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "Z测试", NULL, &m_eZTest);
	pCombHelper->PushItem("关闭", Z_NONE);
	pCombHelper->PushItem("打开", Z_WRITE);
	pCombHelper->PushItem("只读不写", Z_TESTONLY);
	
	_RegCtrlData(E_BOOLValue, NULL, NULL, "打光", NULL, &m_bLightEnable );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "类型", NULL, &m_eStrip);
	pCombHelper->PushItem("单个方格", PTT_BLOCK);
	pCombHelper->PushItem("连续带", PTT_TRAIL);
	pCombHelper->PushItem("贴图拉伸", PTT_STRECTH);
	
	_RegCtrlData(E_BOOLValue, NULL, NULL, "单向扩展", NULL, &m_bSingleSide );
	_RegCtrlData(E_CHARPTR, NULL, NULL, "参考骨骼", NULL, &m_sRefSkelatal );
	_RegCtrlData(E_FLOAT, NULL, NULL, "折射率", NULL, &m_fRefractIndex );

	// advanced prop 
	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset );

	// EFADV_TEXTURE
	_RegCtrlData(E_EEStrMap, NULL, NULL, "贴图", NULL, &m_TextName );

	// EFADV_COLOR   
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color );

	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale );

	// EFADV_XYZCALE;
	_RegCtrlData(E_EVectorMap, NULL, NULL, "XYZ缩放", NULL, &m_XYZcale );


	// EFADV_XYZCALE;
	_RegCtrlData(E_EVectorMap, NULL, NULL, "纹理旋转中心", NULL, &m_UVRCenter );
	
}

int32 CEditPolyTrailProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "旋转轴",				m_vecRotate ) );
	Prop.push_back( EFFECT_PROP( "轨迹最小宽度",		m_MinTrailWidth ) );
	Prop.push_back( EFFECT_PROP( "轨迹最大宽度",		m_MaxTrailWidth ) );
	Prop.push_back( EFFECT_PROP( "轨迹单元生命周期",	m_UnitLifeSpan ) );
	Prop.push_back( EFFECT_PROP( "轨迹结束衰减速度",	m_DeclTime ) );
	Prop.push_back( EFFECT_PROP( "圆滑度",				m_Smooth) );
	Prop.push_back( EFFECT_PROP( "混合模式",			CComboData( (BYTE)m_eBlendModel,  "不混合,相乘（暗）,相加（亮）" ) ) );
	Prop.push_back( EFFECT_PROP( "混合OP",				CComboData( (BYTE)m_eTextOp,	  "纯纹理,纯颜色,1倍乘,2倍乘,4倍乘,加色,加色暗,2倍加色,减色,平滑加色,调色混合,贴图混合" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",				CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "打光",				CComboData( (BYTE)m_bLightEnable, "关闭,打开" ) ) );
	Prop.push_back( EFFECT_PROP( "类型",				CComboData( (BYTE)m_eStrip , "单个方格,连续带,贴图拉伸" ) ) );
	Prop.push_back( EFFECT_PROP( "扩展",				CComboData( (BYTE)m_bSingleSide , "双向,单向" ) ) );
	Prop.push_back( EFFECT_PROP( "参考骨骼",			sqr::CStringData( 0, (const char*)m_sRefSkelatal ) ) );
	Prop.push_back(EFFECT_PROP("折射率",		m_fRefractIndex));

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditPolyTrailProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "旋转轴" )			m_vecRotate = Prop[i].Data().Vec();
		if( Prop[i].Name() == "轨迹最小宽度" )		m_MinTrailWidth = Prop[i].Data().Float();
		if( Prop[i].Name() == "轨迹最大宽度" )		m_MaxTrailWidth = Prop[i].Data().Float();
		if( Prop[i].Name() == "轨迹单元生命周期" )	m_UnitLifeSpan = Prop[i].Data().Int();
		if( Prop[i].Name() == "轨迹结束衰减速度" )m_DeclTime = max( Prop[i].Data().Int() , 1 );
		if( Prop[i].Name() == "圆滑度" )			m_Smooth = max( Prop[i].Data().Int() , 1 );
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel = (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合OP" )			m_eTextOp = (SPR_BLENDOP)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest = (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "打光" )				m_bLightEnable = (0!=Prop[i].Data().Index());
		if( Prop[i].Name() == "类型" )				m_eStrip = (POLY_TRAIL_TYPE)Prop[i].Data().Index();
		if( Prop[i].Name() == "扩展" )				m_bSingleSide = (0!=Prop[i].Data().Index());
		if( Prop[i].Name() == "参考骨骼" )			m_sRefSkelatal = Prop[i].Data().Str();
		if (Prop[i].Name() == "折射率")			m_fRefractIndex = Prop[i].Data().Float();
	}

	_UpdateParams();
}

void CEditPolyTrailProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,  int32 nOffLen,
									 const ADVANCE_PROP* pTexture, int32 nTextLen,
									 const ADVANCE_PROP* pColor,   int32 nColorLen,
									 const ADVANCE_PROP* pScale,   int32 nScaleLen )
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen );
	m_OrgScale = m_MinTrailWidth + ( m_MaxTrailWidth - m_MinTrailWidth )*GetMapValue( m_Scale, 0.0f )/255.0f;
}
