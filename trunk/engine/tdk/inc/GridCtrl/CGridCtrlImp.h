#pragma once
#include "CCtrlBase.h"
#include "CVector3.h"
#include <string>
#include <vector>

#include "Brush\BaseBrushEnum.h"

namespace sqr_tools
{
class CTerrainGridBrush;

class CGridCtrlImp :public CCtrlBase
{
	REG_UI_IMP(CGridCtrlImp);
public:
	CGridCtrlImp(void* param);
	~CGridCtrlImp(void);

protected:
	//画刷状态
	bool	m_BrushLine;
	bool	m_BrushGauss;
	bool	m_BrushSlip;
	bool	m_BrushAlign;
	bool	m_BrushIncrease;
	bool	m_BrushDecrease;
	bool	m_BrushDirX;
	bool	m_BrushDirZ;
	bool	m_BrushDirB;
	//高度
	bool	m_PhysicHeight;
	bool	m_LogicHeight;
	//地表
	bool	m_TerrainLevel;
	bool	m_WaterLevel;
	bool	m_IsWave;
	//MeshTemplate
	bool	m_GaussCut;
	bool	m_VolumnCut;
	//Value
	float	m_BurshStrength;
	float	m_LogicHValue;
	float	m_Cutplace;
	float	m_PullStrength;

	ETERRAIN_OP_TYPE m_OpType;

	CTerrainGridBrush*	m_pTerrainGridBrush;
public:
	virtual void SetBrushLine(bool enable);
	virtual void SetBrushGauss(bool enable);
	virtual void SetBrushSlip(bool enable);
	virtual void GetBrushAlign(bool enable);
	virtual void SetBrushIncrease(bool enable);
	virtual void SetBrushDecrease(bool enable);
	virtual void SetBrushDirX(bool enable);
	virtual void SetBrushDirZ(bool enable);
	virtual void SetBrushDirB(bool enable);
	virtual void SetPhysicalH(bool enable);
	virtual void SetLogicH(bool enable);
	virtual void SetGaussCut(bool enable);
	virtual void SetVolumnCut(bool enable);
	virtual void SetTerrainLevel(bool enable);
	virtual void SetWaterLevel(bool enable);
	virtual void SetWaterWave(bool enable);
	
	virtual bool GetBrushSlip();
	virtual bool GetBrushAlign();
	virtual bool GetBrushLine();
	virtual bool GetBrushGauss();
	virtual bool GetBrushIncrease();
	virtual bool GetBrushDecrease();
	virtual bool GetBrushDirX();
	virtual bool GetBrushDirZ();
	virtual bool GetBrushDirB();
	virtual bool GetPhysicalH();
	virtual bool GetLogicH();
	virtual bool GetGaussCut();
	virtual bool GetVolumnCut();
	virtual bool GetTerrainLevel();
	virtual bool GetWaterLevel();
	virtual bool GetWaterWave();

	virtual void SetBrushStrength(float strength);
	virtual void SetLogicHeight(float height);
	virtual void SetCutPlace(float place);
	virtual void SetPullStrength(float pullstrength);

	virtual float GetBrushStrength();
	virtual float GetLogicHeight();
	virtual float GetCutPlace();
	virtual float GetPullStrength();

	virtual void  SetTemplateType(const int nType);
	virtual void  SetBrushHeightStrength(const float f);
	virtual float GetBrushHHeightStrength() const;

	virtual void ActiveBrush();

	virtual void UpdateOpState();
	virtual ETERRAIN_OP_TYPE GetOpState();
public:
	//重载
	virtual void  SetContext(CEditContext* pContext);
};

//!
}

inline ETERRAIN_OP_TYPE CGridCtrlImp::GetOpState(){	return m_OpType;}
inline bool CGridCtrlImp::GetBrushLine(){	return	m_BrushLine;	}
inline bool CGridCtrlImp::GetBrushGauss(){	return	m_BrushGauss;	}
inline bool CGridCtrlImp::GetBrushSlip(){	return	m_BrushSlip;	}
inline bool CGridCtrlImp::GetBrushAlign(){	return	m_BrushAlign;	}
inline bool CGridCtrlImp::GetBrushIncrease(){	return	m_BrushIncrease;	}
inline bool CGridCtrlImp::GetBrushDecrease(){	return	m_BrushDecrease;	}
inline bool CGridCtrlImp::GetBrushDirX(){	return	m_BrushDirX;	}
inline bool CGridCtrlImp::GetBrushDirB(){	return	m_BrushDirB;	}
inline bool CGridCtrlImp::GetBrushDirZ(){	return	m_BrushDirZ;	}
inline bool CGridCtrlImp::GetPhysicalH(){	return	m_PhysicHeight;	}
inline bool CGridCtrlImp::GetLogicH(){	return	m_LogicHeight;	}
inline bool CGridCtrlImp::GetGaussCut(){	return	m_GaussCut;	}
inline bool CGridCtrlImp::GetVolumnCut(){	return	m_VolumnCut;	}
inline bool CGridCtrlImp::GetTerrainLevel(){	return	m_TerrainLevel;	}
inline bool CGridCtrlImp::GetWaterLevel(){	return	m_WaterLevel;	}
inline bool CGridCtrlImp::GetWaterWave(){	return	m_IsWave;	}
inline float CGridCtrlImp::GetBrushStrength(){	return m_BurshStrength;	};
inline float CGridCtrlImp::GetLogicHeight(){	return m_LogicHValue;	};
inline float CGridCtrlImp::GetCutPlace(){	return m_Cutplace;	};
inline float CGridCtrlImp::GetPullStrength(){	return m_PullStrength;	};
