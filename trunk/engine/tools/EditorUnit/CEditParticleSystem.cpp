#include "stdafx.h"
#include "CEditParticleSystem.h"
#include "TSqrAllocator.inl"


CEditParticleSystemProp::CEditParticleSystemProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CParticleSystemProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditParticleSystemProp::_UIDataUpdate()
{
	_UpdateParams();
}

void CEditParticleSystemProp::SetUintPropData()
{
	_RegCtrlData(E_INTEGER, AutoUpdate, this, "发射时间间隔", NULL, &m_nEmissionInterval );
	_RegCtrlData(E_INTEGER, AutoUpdate, this, "每次发射粒子数", NULL, &m_nParticlesPerEmission );
	_RegCtrlData(E_FLOAT, NULL, NULL, "最小尺寸", NULL, &m_fMinSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "最大尺寸", NULL, &m_fMaxSize );
	_RegCtrlData(E_VECTOR, NULL, NULL, "发射平面法线", NULL, &m_vEmissionPlan );
	_RegCtrlData(E_VECTOR, NULL, NULL, "最小速度", NULL, &m_vMinVelocity );
	_RegCtrlData(E_VECTOR, NULL, NULL, "最大速度", NULL, &m_vMaxVelocity );
	_RegCtrlData(E_VECTOR, NULL, NULL, "加速度", NULL, &m_vAcceleration );
	_RegCtrlData(E_FLOAT, NULL, NULL, "最小角速度", NULL, &m_fMinAngularVelocity );
	_RegCtrlData(E_FLOAT, NULL, NULL, "最大角速度", NULL, &m_fMaxAngularVelocity );
	_RegCtrlData(E_INTEGER, AutoUpdate, this, "最小生命周期", NULL, &m_nMinLifeSpan );
	_RegCtrlData(E_INTEGER, AutoUpdate, this, "最大生命周期", NULL, &m_nMaxLifeSpan );

	_RegCtrlData(E_FLOAT, NULL, NULL, "发射器最小半径", NULL, &m_fMinRadius );
	_RegCtrlData(E_FLOAT, NULL, NULL, "发射器最大半径", NULL, &m_fMaxRadius );
	_RegCtrlData(E_FLOAT, NULL, NULL, "矩形长宽比", NULL, &m_fLongShortRadius );
	_RegCtrlData(E_FLOAT, NULL, NULL, "长宽密度比", NULL, &m_fLongShortDRadius );
	_RegCtrlData(E_FLOAT, NULL, NULL, "角点平分", NULL, &m_fCornerDivision );
	_RegCtrlData(E_FLOAT, NULL, NULL, "跟随发射器速度", NULL, &m_fVelocityPercent );


	CCombDataHelper* pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "跟随系统", NULL, &m_eFollowType);
	pCombHelper->PushItem("不跟随", PSPC_Free);
	pCombHelper->PushItem("完全跟随", PSPC_Follow);
	pCombHelper->PushItem("初始速度跟随", PSPC_AddSpeed);
	pCombHelper->PushItem("静态跟随", PSPC_Static);
	pCombHelper->PushItem("跟随法线", PSPC_Normal);

	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "发射器类型", NULL, &m_eEmissionType);
	pCombHelper->PushItem("球状", PSEMS_eSphere);
	pCombHelper->PushItem("圆", PSEMS_eCircle);
	pCombHelper->PushItem("矩形", PSEMS_eRectangle);
	pCombHelper->PushItem("正多边型角点", PSEMS_eMulCorner);
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "速度类型", NULL, &m_eVelType);
	pCombHelper->PushItem("随机", PSVEL_eRANDOM);
	pCombHelper->PushItem("从原点出发", PSVEL_eFROMCENTER);
	pCombHelper->PushItem("外向", PSVEL_eTOCENTER);
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "混合模式", NULL, &m_eBlendModel);
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
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "粒子贴图朝向", NULL, &m_eTextDir);
	pCombHelper->PushItem("镜头", PSTD_eDirCamera);
	pCombHelper->PushItem("沿速度", PSTD_eDirVelocity);
	pCombHelper->PushItem("正对速度固定角", PSTD_eDirFaceToVelocity);
	pCombHelper->PushItem("始终朝上", PSTD_eDirAlwaysUP);
	pCombHelper->PushItem("朝上面积最大化", PSTD_eDirUpUpUp);
	
	_RegCtrlData(E_FLOAT, NULL, NULL, "长宽比", NULL, &m_fWidthRatio );
	_RegCtrlData(E_CHARPTR, NULL, NULL, "参考骨骼", NULL, &m_sRefSkelatal );
	_RegCtrlData(E_FLOAT, NULL, NULL, "深度偏移", NULL, &m_fZBias );
	_RegCtrlData(E_VECTOR, NULL, NULL, "裁剪面法向量", NULL, &(m_ClipPlane.originPlane.m_Normal) );
	_RegCtrlData(E_EFloatMap, NULL, NULL, "裁剪面距离", NULL, &m_ClipDistMap );

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

	// EFADV_EMIMINRAD
	_RegCtrlData(E_EFloatMap, NULL, NULL, "发射器最小半径", NULL, &m_EmissionMinRadius );

	// EFADV_EMIMAXRAD
	_RegCtrlData(E_EFloatMap, NULL, NULL, "发射器最大半径", NULL, &m_EmissionMaxRadius );
	
}

int32 CEditParticleSystemProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "发射时间间隔",	m_nEmissionInterval ) );
	Prop.push_back( EFFECT_PROP( "每次发射粒子数",	m_nParticlesPerEmission ) );
	Prop.push_back( EFFECT_PROP( "最小尺寸",		m_fMinSize ) );
	Prop.push_back( EFFECT_PROP( "最大尺寸",		m_fMaxSize ) );
	Prop.push_back( EFFECT_PROP( "发射平面法线",	m_vEmissionPlan ) );
	Prop.push_back( EFFECT_PROP( "最小速度",		m_vMinVelocity ) );
	Prop.push_back( EFFECT_PROP( "最大速度",		m_vMaxVelocity ) );
	Prop.push_back( EFFECT_PROP( "加速度",			m_vAcceleration ) );
	Prop.push_back( EFFECT_PROP( "最小角速度",		m_fMinAngularVelocity ) );
	Prop.push_back( EFFECT_PROP( "最大角速度",		m_fMaxAngularVelocity ) );
	Prop.push_back( EFFECT_PROP( "最小生命周期",	m_nMinLifeSpan ) );
	Prop.push_back( EFFECT_PROP( "最大生命周期",	m_nMaxLifeSpan ) );
	Prop.push_back( EFFECT_PROP( "发射器最小半径",	m_fMinRadius ) );
	Prop.push_back( EFFECT_PROP( "发射器最大半径",	m_fMaxRadius ) );
	Prop.push_back( EFFECT_PROP( "矩形长宽比",		m_fLongShortRadius ) );
	Prop.push_back( EFFECT_PROP( "长宽密度比",		m_fLongShortDRadius ) );
	Prop.push_back( EFFECT_PROP( "角点平分",		m_fCornerDivision ) );
	Prop.push_back( EFFECT_PROP( "跟随发射器速度",	m_fVelocityPercent) );
	Prop.push_back( EFFECT_PROP( "跟随系统",		CComboData( (BYTE)m_eFollowType, "不跟随,完全跟随,初始速度跟随,静态跟随,跟随法线" ) ) );
	Prop.push_back( EFFECT_PROP( "发射器类型",		CComboData( (BYTE)m_eEmissionType, "球状,圆,矩形,正多边型角点" ) ) );
	Prop.push_back( EFFECT_PROP( "速度类型",		CComboData( (BYTE)m_eVelType, "随机,从原点出发,外向" ) ) );
	Prop.push_back( EFFECT_PROP( "混合模式",		CComboData( (BYTE)m_eBlendModel, "不混合,相乘（暗）,相加（亮）" ) ) );
	Prop.push_back( EFFECT_PROP( "混合OP",		CComboData( (BYTE)m_eTextOp,	  "纯纹理,纯颜色,1倍乘,2倍乘,4倍乘,加色,加色暗,2倍加色,减色,平滑加色,调色混合,贴图混合" ) ) );
	Prop.push_back( EFFECT_PROP( "Z测试",			CComboData( (BYTE)m_eZTest, "关闭,打开,只读不写" ) ) );
	Prop.push_back( EFFECT_PROP( "打光",			CComboData( (BYTE)m_bLightEnable, "关闭,打开" ) ) );
	Prop.push_back( EFFECT_PROP( "粒子贴图朝向",	CComboData( (BYTE)m_eTextDir, "镜头,沿速度,正对速度固定角,始终朝上,朝上面积最大化" ) ) );
	Prop.push_back( EFFECT_PROP( "长宽比",			m_fWidthRatio ) );
	Prop.push_back( EFFECT_PROP( "参考骨骼",		sqr::CStringData( 0, (const char*)m_sRefSkelatal ) ) );
	Prop.push_back( EFFECT_PROP( "深度偏移",		m_fZBias ) );
	Prop.push_back( EFFECT_PROP( "裁剪面法向量", m_ClipPlane.originPlane.m_Normal ) );
	Prop.push_back( EFFECT_PROP( "裁剪面距离", m_ClipDistMap ) ) ;

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )pProp[i] = Prop[i];
	return nBufLen;
}

void CEditParticleSystemProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "发射时间间隔" )		m_nEmissionInterval = max( Prop[i].Data().Int(), 1 );
		if( Prop[i].Name() == "每次发射粒子数" )	m_nParticlesPerEmission = Prop[i].Data().Int();
		if( Prop[i].Name() == "最小尺寸" )			m_fMinSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "最大尺寸" )			m_fMaxSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "发射平面法线" )		m_vEmissionPlan = Prop[i].Data().Vec();
		if( Prop[i].Name() == "最小速度" )			m_vMinVelocity = Prop[i].Data().Vec();
		if( Prop[i].Name() == "最大速度" )			m_vMaxVelocity = Prop[i].Data().Vec();
		if( Prop[i].Name() == "加速度" )			m_vAcceleration = Prop[i].Data().Vec();
		if( Prop[i].Name() == "最小角速度" )		m_fMinAngularVelocity = Prop[i].Data().Float();
		if( Prop[i].Name() == "最大角速度" )		m_fMaxAngularVelocity = Prop[i].Data().Float();
		if( Prop[i].Name() == "最小生命周期" )		m_nMinLifeSpan = Prop[i].Data().Int();
		if( Prop[i].Name() == "最大生命周期" )		m_nMaxLifeSpan = Prop[i].Data().Int();
		if( Prop[i].Name() == "发射器最小半径" )	m_fMinRadius = Prop[i].Data().Float();
		if( Prop[i].Name() == "发射器最大半径" )	m_fMaxRadius = Prop[i].Data().Float();
		if( Prop[i].Name() == "矩形长宽比" )		m_fLongShortRadius = Prop[i].Data().Float();
		if( Prop[i].Name() == "长宽密度比" )		m_fLongShortDRadius = Prop[i].Data().Float();
		if( Prop[i].Name() == "角点平分" )			m_fCornerDivision = Prop[i].Data().Float();
		if( Prop[i].Name() == "跟随发射器速度" )	m_fVelocityPercent = Prop[i].Data().Float();
		if( Prop[i].Name() == "跟随系统" )			m_eFollowType = (ePS_ParticlCoord)Prop[i].Data().Index();
		if( Prop[i].Name() == "发射器类型" )		m_eEmissionType = (ePS_EMSType)Prop[i].Data().Index();
		if( Prop[i].Name() == "速度类型" )			m_eVelType = (ePS_VELType)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合模式" )			m_eBlendModel = (SPR_BLENDMODE)Prop[i].Data().Index();
		if( Prop[i].Name() == "混合OP" )			m_eTextOp = (SPR_BLENDOP)Prop[i].Data().Index();
		if( Prop[i].Name() == "Z测试" )				m_eZTest = (Z_TEST)Prop[i].Data().Index();
		if( Prop[i].Name() == "打光" )				m_bLightEnable = (0!=Prop[i].Data().Index());
		if( Prop[i].Name() == "粒子贴图朝向" )		m_eTextDir = (ePS_TextureDir)Prop[i].Data().Index();
		if( Prop[i].Name() == "长宽比" )			m_fWidthRatio  = Prop[i].Data().Float();
		if( Prop[i].Name() == "参考骨骼" )			m_sRefSkelatal = Prop[i].Data().Str();
		if( Prop[i].Name() == "深度偏移" )			m_fZBias = Prop[i].Data().Float();
		if (Prop[i].Name() == "裁剪面法向量")		m_ClipPlane.originPlane.m_Normal = Prop[i].Data().Vec();
		if (Prop[i].Name() == "裁剪面距离")			m_ClipDistMap = *Prop[i].Data().FloatList();
	}
	_UpdateParams();
}

void CEditParticleSystemProp::SetAdvancedProp( const ADVANCE_PROP* pOffset, int32 nOffLen,
										  const ADVANCE_PROP* pTexture,	int32 nTextLen,
										  const ADVANCE_PROP* pColor,		int32 nColorLen,
										  const ADVANCE_PROP* pScale,		int32 nScaleLen,
										  const ADVANCE_PROP* pXYZcale,	int32 nXYZcale,
										  const ADVANCE_PROP* pEmiMinRad,	int32 nEmissionMinRadius,
										  const ADVANCE_PROP* pEmiMaxRad,  int32 nEmissionMaxRadius)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcale, pEmiMinRad, nEmissionMinRadius, pEmiMaxRad, nEmissionMaxRadius );
}
