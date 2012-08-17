#include "stdafx.h"
#include "CEditLeading.h"
#include "TSqrAllocator.inl"


CEditLeadingProp::CEditLeadingProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CLeadingProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditLeadingProp::SetUintPropData()
{
	_RegCtrlData(E_FLOAT, NULL, NULL, "起始角", NULL, &m_fStartAngle);
	_RegCtrlData(E_FLOAT, NULL, NULL, "旋转速度", NULL, &m_fRotSpeed);
	_RegCtrlData(E_FLOAT, NULL, NULL, "UV流速度", NULL, &m_fUVSpeed);
	_RegCtrlData(E_INTEGER, NULL, NULL, "分片数", NULL, &m_nPieceCount);
	_RegCtrlData(E_INTEGER, NULL, NULL, "分段数", NULL, &m_nDouCount);
	_RegCtrlData(E_FLOAT, NULL, NULL, "抖动百分比", NULL, &m_fDouPercent);
	_RegCtrlData(E_FLOAT, NULL, NULL, "抖动间隔", NULL, &m_fDouInterval);
	_RegCtrlData(E_INTEGER, NULL, NULL, "跳跃速度", NULL, &m_JumpSpeed);
	_RegCtrlData(E_INTEGER, NULL, NULL, "UV流速度", NULL, &m_JumpNum);
	
	CCombDataHelper* pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "朝向", NULL, &m_eFaceType);
	pCombHelper->PushItem("法线",SPRF_NORMAL);
	pCombHelper->PushItem("镜头",SPRF_CAMERA);
	pCombHelper->PushItem("场景",SPRF_CAMERANONEX);
	pCombHelper->PushItem("平行法线",SPRF_WORLDNORPAR);
	pCombHelper->PushItem("地表",SPRF_TERRAIN);
	pCombHelper->PushItem("固定法线",SPRF_NORMAL_STATIC);
	pCombHelper->PushItem("地表旋转",SPRF_TERRAIN_ROTATE);
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "连接方式", NULL, &m_eLType);
	pCombHelper->PushItem("立即连接", LPLK_IMM);
	pCombHelper->PushItem("依次连接", LPLK_JUMP);
	pCombHelper->PushItem("跳跃连接", LPLK_JUMPONCE);

	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "初始点", NULL, &m_eSSType);
	pCombHelper->PushItem("主角", LPSS_Player);
	pCombHelper->PushItem("列表", LPSS_LIST);
	pCombHelper->PushItem("光线", LPSS_LINE);

	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合模式",NULL, &m_eBlendModel);
	pCombHelper->PushItem("不混合", SPRB_NONE);
	pCombHelper->PushItem("相乘（暗）", SPRB_MULTIPLY);
	pCombHelper->PushItem("相加（亮）", SPRB_ADD);
	pCombHelper->PushItem("光效果", SPRB_LIGHT);

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

int CEditLeadingProp::GetProp( EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "起始角",			m_fStartAngle	) );
	Prop.push_back( EFFECT_PROP( "旋转速度",		m_fRotSpeed		) );
	Prop.push_back( EFFECT_PROP( "UV流速度",		m_fUVSpeed		) );
	Prop.push_back( EFFECT_PROP( "分片数",			m_nPieceCount	) );
	Prop.push_back( EFFECT_PROP( "分段数",			m_nDouCount		) );
	Prop.push_back( EFFECT_PROP( "抖动百分比",		m_fDouPercent	) );
	Prop.push_back( EFFECT_PROP( "抖动间隔",		m_fDouInterval	) );
	Prop.push_back( EFFECT_PROP( "跳跃速度",		m_JumpSpeed		) );
	Prop.push_back( EFFECT_PROP( "跳跃数量",		m_JumpNum		) );
	Prop.push_back( EFFECT_PROP( "朝向",		CComboData( (BYTE)m_eFaceType, "法线,镜头,场景,平行法线,地表,固定法线,地表旋转" ) ) );
	Prop.push_back( EFFECT_PROP( "连接方式",	CComboData( (BYTE)m_eLType,  "立即连接, 依次连接,跳跃连接" ) ) );
	Prop.push_back( EFFECT_PROP( "初始点",		CComboData( (BYTE)m_eSSType,  "主角,列表,光线" ) ) );

	Prop.push_back( EFFECT_PROP( "混合模式",	CComboData( (BYTE)m_eBlendModel,  "不混合,相乘（暗）,相加（亮）,光效果" ) ) );
	Prop.push_back( EFFECT_PROP( "混合OP",		CComboData( (BYTE)m_eTextOp,	  "纯纹理,纯颜色,1倍乘,2倍乘,4倍乘,加色,加色暗,2倍加色,减色,平滑加色,调色混合,贴图混合" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",		CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "打光",		CComboData( (BYTE)m_bLightEnable, "关闭,打开" ) ) );
	Prop.push_back( EFFECT_PROP( "深度偏移",	m_fZBias ) );

	nBufLen = min( nBufLen,(int) Prop.size() );
	for( int i = 0; i < nBufLen; ++i )
		pProp[i] = Prop[i];

	return nBufLen;
}

void CEditLeadingProp::SetProp( const EFFECT_PROP* pProp, int nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( int i = 0; i < (int)Prop.size(); ++i )
	{
		if( Prop[i].Name() == "起始角" )			m_fStartAngle	= Prop[i].Data().Float();
		if( Prop[i].Name() == "旋转速度" )			m_fRotSpeed		= Prop[i].Data().Float();
		if( Prop[i].Name() == "UV流速度" )			m_fUVSpeed		= Prop[i].Data().Float();
		if( Prop[i].Name() == "分片数" )			m_nPieceCount	= Prop[i].Data().DWord();
		if( Prop[i].Name() == "分段数" )			m_nDouCount		= Prop[i].Data().DWord();
		if( Prop[i].Name() == "抖动百分比" )		m_fDouPercent	= Prop[i].Data().Float();
		if( Prop[i].Name() == "抖动间隔" )			m_fDouInterval	= Prop[i].Data().Float();
		if( Prop[i].Name() == "跳跃速度" )			m_JumpSpeed		= Prop[i].Data().DWord();
		if( Prop[i].Name() == "跳跃数量" )			m_JumpNum		= Prop[i].Data().DWord();
		if( Prop[i].Name() == "朝向" )				m_eFaceType = (SPR_FACE)Prop[i].Data().Index();
		if( Prop[i].Name() == "连接方式" )			m_eLType		= (eLP_LKType)Prop[i].Data().Index();
		if( Prop[i].Name() == "初始点" )			m_eSSType		= (eLP_StartScr)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel	= (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合OP" )			m_eTextOp		= (SPR_BLENDOP)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest		= (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "打光" )				m_bLightEnable	= Prop[i].Data().Index() != 0;
		if( Prop[i].Name() == "深度偏移" )			m_fZBias		= Prop[i].Data().Float();
	}

	_UpdateParams();
}