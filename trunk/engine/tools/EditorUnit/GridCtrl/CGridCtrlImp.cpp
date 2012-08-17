#include "StdAfx.h"
#include "GridCtrl\CGridCtrlImp.h"
#include "Brush\TerrainGridBrush.h"
#include "CEditContext.h"

DEF_UI_IMP(CGridCtrlImp);

CGridCtrlImp::CGridCtrlImp(void* param)
{
	m_OpType=EOP_NONE;
	//画刷状态	
	m_BrushLine=false;
	m_BrushGauss=false;	
	m_BrushSlip=false;
	m_BrushAlign=false;
	m_BrushIncrease=false;
	m_BrushDecrease=false;
	m_BrushDirX=false;
	m_BrushDirZ=false;	
	m_BrushDirB=false;
	m_BurshStrength=0.0f;
	//高度
	m_PhysicHeight=false;
	m_LogicHeight=false;
	m_LogicHValue=0.0f;
	//地表
	m_TerrainLevel=false;
	m_WaterLevel=false;
	m_IsWave=false;
	//MeshTemplate
	m_GaussCut=false;
	m_VolumnCut=false;
	m_Cutplace=1.0f;
	m_PullStrength=1.0f;

	m_pTerrainGridBrush = new CTerrainGridBrush;
}

CGridCtrlImp::~CGridCtrlImp(void)
{
	if( m_pOwnContext )
		m_pOwnContext->UnRegisterDelegate(m_pTerrainGridBrush);

	SafeDelete(m_pTerrainGridBrush);
}

void CGridCtrlImp::SetContext(CEditContext* pContext)
{
	if(m_pOwnContext == pContext)
		return;
	
	m_pOwnContext = pContext;
	//if(m_pOwnContext)
	//{
	//	CWindowTarget* dir=m_pOwnContext->GetWindowTarget();
	//	if(dir)
	//	{
	//		CRenderGroup* group=dir->GetGroup();
	//		if(group)
	//			pCamera=group->GetCamera();
	//	}
	//}
	if(m_pOwnContext)
	{
		m_pOwnContext->RegisterDelegate(m_pTerrainGridBrush);
		m_pTerrainGridBrush->SetBaseBrushType(BRUSH_CIRCLE);
	}
}

//互斥 高斯	线形 平滑 对齐
void CGridCtrlImp::SetBrushSlip(bool enable)
{
	m_BrushSlip=enable;
	if(m_BrushSlip)
	{
		m_BrushGauss=false;
		m_BrushLine=false;
		m_BrushAlign=false;
	}
	UpdateOpState();
}

void CGridCtrlImp::GetBrushAlign(bool enable)
{
	m_BrushAlign=enable;
	if(m_BrushAlign)
	{
		m_BrushGauss=false;
		m_BrushLine=false;
		m_BrushSlip=false;
	}
	UpdateOpState();
}

void CGridCtrlImp::SetBrushLine(bool enable)
{
	m_BrushLine=enable;
	if(m_BrushLine)
	{
		m_BrushGauss=false;
		m_BrushAlign=false;
		m_BrushSlip=false;
	}
	UpdateOpState();
}
void CGridCtrlImp::SetBrushGauss(bool enable)
{
	m_BrushGauss=enable;
	if(m_BrushGauss)
	{
		m_BrushLine=false;
		m_BrushAlign=false;
		m_BrushSlip=false;
	}
	UpdateOpState();
}
//互斥	增加 降低
void CGridCtrlImp::SetBrushIncrease(bool enable)
{
	m_BrushIncrease=enable;
	if(m_BrushIncrease)
		m_BrushDecrease=false;
	UpdateOpState();
}
void CGridCtrlImp::SetBrushDecrease(bool enable)
{
	m_BrushDecrease=enable;
	if(m_BrushDecrease)
		m_BrushIncrease=false;
	UpdateOpState();
}
//互斥 方向X Z B
void CGridCtrlImp::SetBrushDirX(bool enable)
{
	m_BrushDirX=enable;
	if(m_BrushDirX)
	{
		m_BrushDirB=false;
		m_BrushDirZ=false;
	}
	UpdateOpState();
}
void CGridCtrlImp::SetBrushDirZ(bool enable)
{
	m_BrushDirZ=enable;
	if(m_BrushDirZ)
	{
		m_BrushDirX=false;
		m_BrushDirB=false;
	}
	UpdateOpState();
}
void CGridCtrlImp::SetBrushDirB(bool enable)
{
	m_BrushDirB=enable;
	if(m_BrushDirB)
	{
		m_BrushDirX=false;
		m_BrushDirZ=false;
	}
	UpdateOpState();
}

void CGridCtrlImp::UpdateOpState()
{
	if(m_BrushSlip)
	{	//平滑
		m_OpType=EOP_SMOOTH; 
	}
	else if(m_BrushAlign)
	{	//对齐
		m_OpType=EOP_ALIGN;
	}
	else if(m_BrushGauss)
	{	//高斯
		if(m_BrushIncrease) m_OpType=EOP_GAUS_INC;
		if(m_BrushDecrease)	m_OpType=EOP_GAUS_DEC; 
		if( (!m_BrushIncrease) && (!m_BrushDecrease) ) m_OpType=EOP_NONE;
	}
	else if(m_BrushLine)
	{	//线形
		if(m_BrushIncrease) 
		{
			if(m_BrushDirX)	 m_OpType=EOP_LINE_INCX;
			if(m_BrushDirZ)	 m_OpType=EOP_LINE_INCZ;
			if(m_BrushDirB)	 m_OpType=EOP_LINE_INCB; 
			if( (!m_BrushDirX) && (!m_BrushDirZ)  && (!m_BrushDirB) ) m_OpType=EOP_NONE;
		}
		if(m_BrushDecrease)
		{
			if(m_BrushDirX)	 m_OpType=EOP_LINE_DECX;
			if(m_BrushDirZ)	 m_OpType=EOP_LINE_DECZ;
			if(m_BrushDirB)	 m_OpType=EOP_LINE_DECB;
			if( (!m_BrushDirX) && (!m_BrushDirZ)  && (!m_BrushDirB) ) m_OpType=EOP_NONE;
		}
		if( (!m_BrushIncrease) && (!m_BrushDecrease) ) m_OpType=EOP_NONE;
	}
	else
	{	//无
		m_OpType=EOP_NONE;
	}

	if( m_pTerrainGridBrush )
		m_pTerrainGridBrush->SetTerrainOpType(m_OpType);
}

//互斥 物理高 逻辑高
void CGridCtrlImp::SetPhysicalH(bool enable)
{
	m_PhysicHeight=enable;
	if(m_PhysicHeight)
		m_LogicHeight=false;
}

void CGridCtrlImp::SetLogicH(bool enable)
{
	m_LogicHeight=enable;
	if(m_LogicHeight)
		m_PhysicHeight=false;
}
//互斥 线形 高斯
void CGridCtrlImp::SetGaussCut(bool enable)
{
	m_GaussCut=enable;
	if(m_GaussCut)
		m_VolumnCut=false;
}
void CGridCtrlImp::SetVolumnCut(bool enable)
{
	m_VolumnCut=enable;
	if(m_VolumnCut)
		m_GaussCut=false;
}
//互斥 地表 水面
void CGridCtrlImp::SetTerrainLevel(bool enable)
{
	m_TerrainLevel=enable;
	if(m_TerrainLevel)
		m_WaterLevel=false;
}
void CGridCtrlImp::SetWaterLevel(bool enable)
{
	m_WaterLevel=enable;
	if(m_WaterLevel)
		m_TerrainLevel=false;
}
void CGridCtrlImp::SetWaterWave(bool enable)
{
	m_IsWave=enable;
}

void CGridCtrlImp::SetBrushStrength(float strength)
{
	m_BurshStrength=strength;
}

void CGridCtrlImp::SetLogicHeight(float height)
{
	m_LogicHValue=height;
}

void CGridCtrlImp::SetCutPlace(float place)
{
	m_Cutplace=place;
}

void CGridCtrlImp::SetPullStrength(float pullstrength)
{
	m_PullStrength=pullstrength;
}

void CGridCtrlImp::SetTemplateType( const int nType )
{
	if( m_pTerrainGridBrush )
		m_pTerrainGridBrush->SetTemplateType(nType);
}

void CGridCtrlImp::ActiveBrush()
{
	if( m_pTerrainGridBrush )
		m_pTerrainGridBrush->Active();
}

void CGridCtrlImp::SetBrushHeightStrength( const float f )
{
	m_BurshStrength=f;
	if( m_pTerrainGridBrush )
		m_pTerrainGridBrush->SetHeightStrength(f);
}

float CGridCtrlImp::GetBrushHHeightStrength() const
{
	if( m_pTerrainGridBrush )
		return ((CTerrainGridBrush*)m_pTerrainGridBrush)->GetHeightStrength();
	return m_BurshStrength;
}