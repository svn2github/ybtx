#include "stdafx.h"
#include "CEditModelConsole.h"
#include "CSkeletalsMgr.h"
#include "TSqrAllocator.inl"


CEditModelConsoleProp::CEditModelConsoleProp( uint32 ID, IEffectClassDesc* pEffectClassDesc )
:CModelConsoleProp(ID,pEffectClassDesc)
{
	SetUnitProp(this);
	// 完成数据设置工作
	SetBaseUintData();
	SetUintPropData();
}

void CEditModelConsoleProp::_UIDataUpdate()
{
	_UpdateParams();
	CCombDataHelper* pHelper0 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece0", NULL, &m_nPiece0);
	pHelper0->PushItem("None", 0);
	CCombDataHelper* pHelper1 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece1", NULL, &m_nPiece1);
	pHelper1->PushItem("None", 0);
	CCombDataHelper* pHelper2 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece2", NULL, &m_nPiece2);
	pHelper2->PushItem("None", 0);
	CCombDataHelper* pHelper3 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece3", NULL, &m_nPiece3);
	pHelper3->PushItem("None", 0);
	CCombDataHelper* pHelper4 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece4", NULL, &m_nPiece4);
	pHelper4->PushItem("None", 0);
	CCombDataHelper* pHelper5 = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "piece5", NULL, &m_nPiece5);
	pHelper5->PushItem("None", 0);

	// PieceList
	for( size_t i = 0; m_pPieceGroup && i < m_pPieceGroup->GetPieceClassNum(); ++i )
	{
		pHelper0->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
		pHelper1->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
		pHelper2->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
		pHelper3->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
		pHelper4->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
		pHelper5->PushItem(m_pPieceGroup->GetPieceClassName(i), i+1);
	}

	CCombDataHelper* pHelperAni = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "动作名", NULL, &m_Animation);
	for( int32 j = 0; m_pPieceGroup && j < m_pPieceGroup->GetPieceClassAniNum(); ++j )
	{
		pHelperAni->PushItem(m_pPieceGroup->GetPieceClassAniName(j), j);
	}

	CCombDataHelper* pHelperSke = (CCombDataHelper*)_ResetCtrlData(E_COMBO, NULL, NULL, "子骨骼名字", NULL, &m_nSkIdx);
	pHelperSke->PushItem("None", 0);
	for( int32 i = 0; m_pSkeletalFrame && i < m_pSkeletalFrame->GetSkeletalTotalNum(); ++i )
	{
		pHelperSke->PushItem(m_pSkeletalFrame->GetSkeletalName(i), i+1);
	}
}

void CEditModelConsoleProp::_GetListNames(EString* PieceList, EString* AniList, EString* SKList)
{
	ESet<EString> PieceAniName;
	for( int32 j = 0; m_pPieceGroup && j < m_pPieceGroup->GetPieceClassAniNum(); ++j )
		PieceAniName.insert( m_pPieceGroup->GetPieceClassAniName( j ) );
	// PieceList
	for( size_t i = 0; m_pPieceGroup && i < m_pPieceGroup->GetPieceClassNum(); ++i )
	{
		(*PieceList) += ",";
		(*PieceList) += m_pPieceGroup->GetPieceClassName( i );;
	}
	if( (*PieceList).size() > 120 )	(*PieceList)[120] = 0;

	// AniList
	for( ESet<EString>::iterator it = PieceAniName.begin(); it != PieceAniName.end(); ++it )
	{
		if( it != PieceAniName.begin() )
			(*AniList) += ",";
		(*AniList) += *it;
	}
	for( int32 i = PieceAniName.size(); m_pAnimationGroup 
		&& i < m_pAnimationGroup->GetAnimationNum() + (int32)PieceAniName.size(); ++i )
	{
		if( i )
			(*AniList) += ",";
		(*AniList) += m_pAnimationGroup->GetAnimationName(i);
	}
	if( (*AniList).size() > 120 )		(*AniList)[120] = 0;

	// SkList
	for( int32 i = 0; m_pSkeletalFrame && i < m_pSkeletalFrame->GetSkeletalTotalNum(); ++i )
	{
		(*SKList) += ",";
		(*SKList) += m_pSkeletalFrame->GetSkeletalName(i);
	}
}

void CEditModelConsoleProp::_GetListNames(vector<EString>& PieceList, vector<EString>& AniList, vector<EString>& SKList)
{
	// PieceList
	PieceList.push_back("None");
	for( size_t i = 0; m_pPieceGroup && i < m_pPieceGroup->GetPieceClassNum(); ++i )
		PieceList.push_back(m_pPieceGroup->GetPieceClassName(i));

	// AniList
	for( int32 j = 0; m_pPieceGroup && j < m_pPieceGroup->GetPieceClassAniNum(); ++j )
		AniList.push_back(m_pPieceGroup->GetPieceClassAniName(j));

	// SkList
	for( int32 i = 0; m_pSkeletalFrame && i < m_pSkeletalFrame->GetSkeletalTotalNum(); ++i )
		SKList.push_back(m_pSkeletalFrame->GetSkeletalName(i));
}

void CEditModelConsoleProp::SetUintPropData()
{
	vector<EString> PieceList;
	vector<EString> AniList;
	vector<EString> SkList;
	_GetListNames(PieceList, AniList, SkList);
	
	// add end
	_RegCtrlData(E_INTEGER, NULL, NULL, "渐显时间", NULL, &m_BirthTime );
	_RegCtrlData(E_INTEGER, NULL, NULL, "渐隐时间", NULL, &m_DeleteTime );
	_RegCtrlData(E_VECTOR, NULL, NULL, "旋转轴", NULL, &m_vecRotate );
	_RegCtrlData(E_ESTRINGBUTTON, AutoUpdate, this, "MOD文件名", "模型文件(*.mod)|*.mod", &m_szARPName );
	
	CCombDataHelper* pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece0", NULL, &m_nPiece0);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);

	_RegCtrlData(E_INTEGER, NULL, NULL, "piece0渲染类型", NULL, &m_nRS0 );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece1", NULL, &m_nPiece1);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);
	
	_RegCtrlData(E_INTEGER, NULL, NULL, "piece1渲染类型", NULL, &m_nRS1 );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece2", NULL, &m_nPiece2);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);
	
	_RegCtrlData(E_INTEGER, NULL, NULL, "piece2渲染类型", NULL, &m_nRS2 );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece3", NULL, &m_nPiece3);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);
	
	_RegCtrlData(E_INTEGER, NULL, NULL, "piece3渲染类型", NULL, &m_nRS3 );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece4", NULL, &m_nPiece4);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);
	
	_RegCtrlData(E_INTEGER, NULL, NULL, "piece4渲染类型", NULL, &m_nRS4 );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "piece5", NULL, &m_nPiece5);
	for ( size_t i=0; i<PieceList.size(); ++i )
		pCombHelper->PushItem(PieceList[i].c_str(), i);
	
	_RegCtrlData(E_INTEGER, NULL, NULL, "piece5渲染类型", NULL, &m_nRS5 );
	_RegCtrlData(E_ESTRINGBUTTON, AutoUpdate, this, "SKE文件名", "特效模型动作文件(*.agp)|*.agp", &m_szARAName );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "动作名", NULL, &m_Animation);
	for ( size_t i=0; i<AniList.size(); ++i )
		pCombHelper->PushItem(AniList[i].c_str(), i);
	
	_RegCtrlData(E_FLOAT, NULL, NULL, "X最小尺寸", NULL, &m_fMinXSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "X最大尺寸", NULL, &m_fMaxXSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "Y最小尺寸", NULL, &m_fMinYSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "Y最大尺寸", NULL, &m_fMaxYSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "Z最小尺寸", NULL, &m_fMinZSize );
	_RegCtrlData(E_FLOAT, NULL, NULL, "Z最大尺寸", NULL, &m_fMaxZSize );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "朝向", NULL, &m_eFaceType);
	pCombHelper->PushItem("原有", SPRF_NORMAL);
	pCombHelper->PushItem("镜头", SPRF_CAMERA);
	pCombHelper->PushItem("镜头(无X轴)", SPRF_CAMERANONEX);
	
	_RegCtrlData(E_VECTOR2F, NULL, NULL, "贴图平移", NULL, &m_vecTextMoveVel );
	_RegCtrlData(E_FLOAT, NULL, NULL, "贴图旋转", NULL, &m_fTextAngularVel );
	_RegCtrlData(E_FLOAT, NULL, NULL, "贴图最大倍数", NULL, &m_fTextMaxScale );
	_RegCtrlData(E_FLOAT, NULL, NULL, "贴图最小倍数", NULL, &m_fTextMinScale );
	_RegCtrlData(E_ESTRING, NULL, NULL, "父骨骼名字", NULL, &m_szParentSk );
	
	pCombHelper = (CCombDataHelper*)_RegCtrlData(E_COMBO, NULL, NULL, "子骨骼名字", NULL, &m_nSkIdx);
	for ( size_t i=0; i<SkList.size(); ++i )
		pCombHelper->PushItem(SkList[i].c_str(), i);
	
	_RegCtrlData(E_VECTOR, NULL, NULL, "裁剪面法向量", NULL, &(m_ClipPlane.originPlane.m_Normal) );
	_RegCtrlData(E_EFloatMap, NULL, NULL, "裁剪面距离", NULL, &m_ClipDistMap );
	_RegCtrlData(E_BOOLValue, NULL, NULL, "是否有阴影", NULL, &m_bEnableShadow );
	_RegCtrlData(E_EFloatMap, NULL, NULL, "折射率曲线", NULL, &m_mapRefract );


	// advanced prop 
	if( m_nSkIdx > 0 && m_pSkeletalFrame && m_nSkIdx <= m_pSkeletalFrame->GetSkeletalTotalNum() )
		m_szSk = m_pSkeletalFrame->GetSkeletalName(m_nSkIdx-1);

	// EFADV_SCALE   EMap<float, byte>
	_RegCtrlData(E_EBYTEMap, NULL, NULL, "大小", NULL, &m_Scale );

	// EFADV_OFFSET
	_RegCtrlData(E_EVectorMap, NULL, NULL, "轨迹", NULL, &m_vecOffset );

	// EFADV_COLOR
	_RegCtrlData(E_ECOLORMap, NULL, NULL, "颜色", NULL, &m_Color );
	
	// EFADV_XYZCALE;
	_RegCtrlData(E_EVectorMap, NULL, NULL, "XYZ缩放", NULL, &m_XYZcale );
	
}

int32 CEditModelConsoleProp::GetProp( EFFECT_PROP* pProp, int32 nBufLen )
{
	EString PieceList = "None";
	EString AniList = "";
	EString SkList = "None";
	_GetListNames(&PieceList, &AniList, &SkList);
	// add end

	EVector<EFFECT_PROP> Prop;
	CEditUnitPropHelp::GetBaseProp( Prop );

	Prop.push_back( EFFECT_PROP( "渐显时间",		m_BirthTime ) );
	Prop.push_back( EFFECT_PROP( "渐隐时间",		m_DeleteTime ) );

	Prop.push_back( EFFECT_PROP( "旋转轴",			m_vecRotate ) );
	Prop.push_back( EFFECT_PROP( "MOD文件名",		sqr::CStringData( 1, m_szARPName ) ) );
	Prop.push_back( EFFECT_PROP( "piece0",			CComboData( (BYTE)m_nPiece0, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece0渲染类型",	m_nRS0 ) );
	Prop.push_back( EFFECT_PROP( "piece1",			CComboData( (BYTE)m_nPiece1, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece1渲染类型",	m_nRS1 ) );
	Prop.push_back( EFFECT_PROP( "piece2",			CComboData( (BYTE)m_nPiece2, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece2渲染类型",	m_nRS2 ) );
	Prop.push_back( EFFECT_PROP( "piece3",			CComboData( (BYTE)m_nPiece3, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece3渲染类型",	m_nRS3 ) );
	Prop.push_back( EFFECT_PROP( "piece4",			CComboData( (BYTE)m_nPiece4, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece4渲染类型",	m_nRS4 ) );
	Prop.push_back( EFFECT_PROP( "piece5",			CComboData( (BYTE)m_nPiece5, PieceList.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "piece5渲染类型",	m_nRS5 ) );
	Prop.push_back( EFFECT_PROP( "SKE文件名",		sqr::CStringData( 1, m_szARAName ) ) );
	Prop.push_back( EFFECT_PROP( "动作名",			CComboData( (BYTE)m_Animation, AniList.c_str() ) ) );
	//Prop.push_back( EFFECT_PROP( "同步模型动作",	CComboData( (BYTE)m_bSynch, "不同步,同步" ) ) );

	Prop.push_back( EFFECT_PROP( "X最小尺寸",		m_fMinXSize ) );
	Prop.push_back( EFFECT_PROP( "X最大尺寸",		m_fMaxXSize ) );
	Prop.push_back( EFFECT_PROP( "Y最小尺寸",		m_fMinYSize ) );
	Prop.push_back( EFFECT_PROP( "Y最大尺寸",		m_fMaxYSize ) );
	Prop.push_back( EFFECT_PROP( "Z最小尺寸",		m_fMinZSize ) );
	Prop.push_back( EFFECT_PROP( "Z最大尺寸",		m_fMaxZSize ) );

	Prop.push_back( EFFECT_PROP( "朝向",			CComboData( (BYTE)m_eFaceType, "原有,镜头,镜头(无X轴)" ) ) );

	Prop.push_back( EFFECT_PROP( "贴图平移",		CVector3f( m_vecTextMoveVel.x, m_vecTextMoveVel.y, 0 ) ) );
	Prop.push_back( EFFECT_PROP( "贴图旋转",		m_fTextAngularVel ) );
	Prop.push_back( EFFECT_PROP( "贴图最大倍数",	m_fTextMaxScale ) );
	Prop.push_back( EFFECT_PROP( "贴图最小倍数",	m_fTextMinScale ) );

	Prop.push_back( EFFECT_PROP( "父骨骼名字",		sqr::CStringData( 0, m_szParentSk.c_str() ) ) );
	Prop.push_back( EFFECT_PROP( "子骨骼名字",		CComboData( (BYTE)m_nSkIdx, SkList.c_str() ) ) );

	Prop.push_back( EFFECT_PROP( "裁剪面法向量",	m_ClipPlane.originPlane.m_Normal ) );
	Prop.push_back( EFFECT_PROP( "裁剪面距离",		m_ClipDistMap ) ) ;

	Prop.push_back( EFFECT_PROP( "是否有阴影",		CComboData( (BYTE)m_bEnableShadow, "无, 有" ) ) );

	Prop.push_back(EFFECT_PROP("折射率曲线",		m_mapRefract));

	if( m_nSkIdx > 0 && m_pSkeletalFrame && m_nSkIdx <= m_pSkeletalFrame->GetSkeletalTotalNum() )
		m_szSk = m_pSkeletalFrame->GetSkeletalName(m_nSkIdx-1);

	nBufLen = min( nBufLen,(int32) Prop.size() );
	for( int32 i = 0; i < nBufLen; ++i )
		pProp[i] = Prop[i];

	return nBufLen;
}

void CEditModelConsoleProp::SetProp( const EFFECT_PROP* pProp, int32 nBufLen )
{
	EVector<EFFECT_PROP> Prop( pProp, pProp + nBufLen );
	CEditUnitPropHelp::SetBaseProp( Prop );

	for( size_t i = 0; i < Prop.size(); ++i )
	{
		if( Prop[i].Name() == "渐显时间" )			m_BirthTime = Prop[i].Data().Int();
		if( Prop[i].Name() == "渐隐时间" )			m_DeleteTime = Prop[i].Data().Int();
		if( Prop[i].Name() == "旋转轴" )			m_vecRotate = Prop[i].Data().Vec();
		if( Prop[i].Name() == "MOD文件名" )			m_szARPName	= Prop[i].Data().Str();
		if( Prop[i].Name() == "piece0" )			m_nPiece0 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece0渲染类型" )	m_nRS0 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "piece1" )			m_nPiece1 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece1渲染类型" )	m_nRS1 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "piece2" )			m_nPiece2 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece2渲染类型" )	m_nRS2 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "piece3" )			m_nPiece3 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece3渲染类型" )	m_nRS3 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "piece4" )			m_nPiece4 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece4渲染类型" )	m_nRS4 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "piece5" )			m_nPiece5 = Prop[i].Data().Index();
		if( Prop[i].Name() == "piece5渲染类型" )	m_nRS5 = max( 0, Prop[i].Data().Int() );
		if( Prop[i].Name() == "SKE文件名" )			m_szARAName = Prop[i].Data().Str();
		if( Prop[i].Name() == "动作名" )			m_Animation = Prop[i].Data().Index();
		//if( Prop[i].Name() == "同步模型动作" )		m_bSynch = Prop[i].Data().Index() != 0;

		if( Prop[i].Name() == "X最小尺寸" )			m_fMinXSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "X最大尺寸" )			m_fMaxXSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "Y最小尺寸" )			m_fMinYSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "Y最大尺寸" )			m_fMaxYSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "Z最小尺寸" )			m_fMinZSize  = Prop[i].Data().Float();
		if( Prop[i].Name() == "Z最大尺寸" )			m_fMaxZSize  = Prop[i].Data().Float();

		if( Prop[i].Name() == "朝向" )				m_eFaceType = (SPR_FACE)Prop[i].Data().Index();

		if( Prop[i].Name() == "贴图平移" )			m_vecTextMoveVel  = Prop[i].Data().Vec();
		if( Prop[i].Name() == "贴图旋转" )			m_fTextAngularVel  = Prop[i].Data().Float();
		if( Prop[i].Name() == "贴图最大倍数" )		m_fTextMaxScale = Prop[i].Data().Float();
		if( Prop[i].Name() == "贴图最小倍数" )		m_fTextMinScale = Prop[i].Data().Float();

		if( Prop[i].Name() == "父骨骼名字" )		m_szParentSk = Prop[i].Data().Str();
		if( Prop[i].Name() == "子骨骼名字" )		m_nSkIdx = Prop[i].Data().Index();

		if (Prop[i].Name() == "裁剪面法向量")		m_ClipPlane.originPlane.m_Normal = Prop[i].Data().Vec();
		if (Prop[i].Name() == "裁剪面距离")			m_ClipDistMap = *Prop[i].Data().FloatList();

		if( Prop[i].Name() == "是否有阴影" )		m_bEnableShadow = Prop[i].Data().Index() != 0;

		if (Prop[i].Name() == "折射率曲线")			m_mapRefract = *Prop[i].Data().FloatList();
	}

	_UpdateParams();
}

uint32 CEditModelConsoleProp::GetAdvancedProp( ADVANCE_PROP* pOffset,   int32& nOffLen,
										  ADVANCE_PROP* pTexture,   int32& nTextLen,
										  ADVANCE_PROP* pColor,     int32& nColorLen,
										  ADVANCE_PROP* pScale,     int32& nScaleLen, 
										  ADVANCE_PROP* pXYZcale,   int32& nXYZcaleLen,
										  ADVANCE_PROP* pUVRCenter, int32& nUVRCenter)
{
	CEditUnitPropHelp::GetAdvancedProp( pOffset, nOffLen, pTexture, nTextLen, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter );

	return EFADV_SCALE|EFADV_OFFSET|EFADV_COLOR|EFADV_XYZCALE;
}

void CEditModelConsoleProp::SetAdvancedProp( const ADVANCE_PROP* pOffset,   int32 nOffLen,
										const ADVANCE_PROP* pTexture,   int32 nTextLen,
										const ADVANCE_PROP* pColor,     int32 nColorLen,
										const ADVANCE_PROP* pScale,     int32 nScaleLen,
										const ADVANCE_PROP* pXYZcale,   int32 nXYZcaleLen,
										const ADVANCE_PROP* pUVRCenter, int32 nUVRCenter)
{
	CEditUnitPropHelp::SetAdvancedProp( pOffset, nOffLen, pTexture, 0, pColor, nColorLen, pScale, nScaleLen, pXYZcale, nXYZcaleLen, pUVRCenter, nUVRCenter);
}