//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004    
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
	
#pragma once
	
#include "BluelyG.h"
#include "Scene.h"
#include "Input.h"
#include "WireBox.h"


namespace sqr
{
	class IModel;
	class IEffect;
	class IPieceGroup;
	class IEffectGroup;
}

namespace bg
{
namespace sc
{
	
class CModel : public ISceneNode
{
public:
	enum EEdit_State { EES_MOVING = 0, EES_PLACED, EES_DANGLING };
	enum EEdit_State_Placed { EESP_ROTATE, EESP_LIFT, EESP_SHIFTX, EESP_SHIFTZ, EESP_NONE };
	enum ECenter_Type { ECT_CENTER, ECT_CORNER, ECT_XMIDWAY, ECT_ZMIDWAY, ECT_NONE };

	struct SBlockInfo
	{
		SBlockInfo::SBlockInfo()
		{
			nBlockMask = 1;
			nBlockType = 3; // high
		}

		int nBlockMask;
		int nBlockType;
	};

	struct SLogicHeightInfo
	{
		SLogicHeightInfo::SLogicHeightInfo()
		{
			fLogicHeight = 0.0f; 
		}
		float fLogicHeight;
	};

private:

	std::vector<DWORD> m_vecBlockVertexIndices;

	sqr::IModel * m_pModel;

	D3DXVECTOR3 m_vCorners[8];
	D3DXVECTOR3 vCorners[8];

	CWireBox * m_pWireBox;
	
	std::vector<CWireBox *> m_vecpWireBox;
	
	bool m_bLocked;

	bool m_bSubUnit;

	int m_nRotationCnt;
	float m_fRotation;
	
	float m_fLift;
	float m_fShiftX;
	float m_fShiftZ;
	
	D3DXMATRIX m_matSelfOff;
	D3DXMATRIX m_matUserOff;
	D3DXMATRIX m_matWorldOff;
	D3DXMATRIX m_matRotY;
	D3DXMATRIX m_mat;

	IEffect * m_pEffect;
	IEffectGroup * m_pGroup;
	
	EEdit_State m_eEditState;
	EEdit_State_Placed m_eEditStatePlaced;
	ECenter_Type m_eCenterType,m_eCenterTypeOrigin;

	std::string m_strFileName;

	D3DXVECTOR3 m_vScale;
	D3DXVECTOR3 m_vOffset;
	D3DXVECTOR3 m_vOffsetOverall;
	D3DXVECTOR3 m_vCenter;

	// derived from "scale and offset"
	float m_fHeight; 
	float m_fWidth; 
	float m_fDepth; 
	bool m_bSelected;

	// grids heights
	std::vector<float> m_vecGridsHeights;
	int m_nWidthDimInGrids;
	int m_nDepthDimInGrids;

	// logical and physical center grid
	int m_nPhysicalCenterGridIndex;
	int m_nLogicalCenterGridIndex;

	// block grids
	std::vector<int> m_vecBlockGrids; // origin
	std::vector<SBlockInfo> m_vecBlockInfo;

	// vertex heights
	std::vector<SLogicHeightInfo> m_vecLogicHeightInfo;

	// hook
	int m_nHookHandle;


public:
	void UpdateRotation();
	void InitRotation();
	void UpdateRotationDiscrete();
	float GetRotation();
	void RestoreRotation(float f);

	void InitShiftX();
	void UpdateShiftX(int nWay);
	float GetShiftX();

	void InitShiftZ();
	void UpdateShiftZ(int nWay);
	float GetShiftZ();
	
	void UpdateLift(int nWay);
	void InitLift();
	float GetLift();
	void RestoreLift(float f);
	
	D3DXVECTOR3 const & GetOffset();
	
	void SetOffsetOverall(D3DXVECTOR3 vec);
	
	void InitPlaced()
	{
		switch ( m_eEditStatePlaced ) 
		{
		case EESP_ROTATE:
			this->InitRotation();
		break;
		
		case EESP_LIFT:
			this->InitLift();
		break;
		
		case EESP_SHIFTX:
			this->InitShiftX();
		break;
		
		case EESP_SHIFTZ:
			this->InitShiftZ();
		break;
		}
	}
		
	void UpdateBoundingBox();
	void RestoreOffset();
	
	//////////////////////////////////////////////////////////////////////////
	
	int GetWidthInGrids() { return m_nWidthDimInGrids; }
	
	int GetDepthInGrids() { return m_nDepthDimInGrids; }
	
	//////////////////////////////////////////////////////////////////////////
	
	int GetBlockSize() { return m_vecBlockGrids.size(); }
	int GetBlockGridIndex(int i) {	return m_vecBlockGrids[i]; }
	
	int GetBlockVertexSize() { return m_vecBlockVertexIndices.size(); }
	int GetBlockVertexIndex(int i) {	return m_vecBlockVertexIndices[i]; }
	
	int GetBlockInfoSize() { return m_vecBlockInfo.size(); }
	void FlipBlockMaskByIndex(int nIndex) { m_vecBlockInfo[nIndex].nBlockMask = 1-m_vecBlockInfo[nIndex].nBlockMask; }
	void SetBlockMaskByIndex(int nIndex, int nMask) { m_vecBlockInfo[nIndex].nBlockMask = nMask; }
	int GetBlockMaskByIndex(int i) {	return m_vecBlockInfo[i].nBlockMask; }
	
	int GetBlockTypeByIndex(int i);
	
	void SetBlockMask(std::vector<sc::CModel::SBlockInfo> & vecBlock); 
	void SetBlockType(std::vector<sc::CModel::SBlockInfo> & vecBlock);
//	void SetBlockLogicHeight(std::vector<sc::CModel::SBlockInfo> & vecBlock); 
	
	void SetLogicHeight(std::vector<sc::CModel::SLogicHeightInfo> & vecHeight); 
	
	void AddBlockGridIndex(int i)
	{
		m_vecBlockGrids.push_back(i);
	}
	
	void CleanBlockGridIndices()
	{
		m_vecBlockGrids.clear();
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	float GetGridLogicHeight(int n) 
	{ 		
//		int i = (n/m_nDepthDimInGrids)*m_nWidthDimInGrids + (n%m_nWidthDimInGrids);
		float f = m_vecGridsHeights[n];
		return f;	
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	void SetCenterGridIndex(int n);
	
	void SetLogicCenterGridIndex(int n)
	{
		m_nLogicalCenterGridIndex = n;
	}
	
	int CModel::GetPhysicalCenterGridIndex();

	int CModel::GetLogicalCenterGridIndex();

	int GetOriginAdjustedCenterGridIndex(int n);

	//////////////////////////////////////////////////////////////////////////

	void SetLocked(bool b)
	{
		m_bLocked = b;
		if (b)
		{
			m_pWireBox->SetColor(0xffff0000);
		}
		else
		{
			m_pWireBox->SetColor(0xff0000ff);
		}
	}

	bool GetLocked()
	{
		return m_bLocked;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	std::string & GetName() { return m_strFileName; }
	
	bool IsSubUnit() { return m_bSubUnit; }

	bool IsFliped() { return (m_nRotationCnt%2)!=0; }

	D3DXVECTOR3 GetScale() { return D3DXVECTOR3(m_fWidth,m_fHeight,m_fDepth); }
	
	EEdit_State_Placed GetPlacedEditState() 
	{
		return m_eEditStatePlaced;
	}
	
	void SetPlacedEditState(EEdit_State_Placed e)
	{
		m_eEditStatePlaced = e;
	}
	
	void SetEditState(EEdit_State state)
	{
		m_eEditState = state;

		if ( state == EES_PLACED )
		{
			this->SetVisible(false); // let gird control its visibility
			this->UpdateBoundingBox();
		}
		
		if ( state == EES_MOVING )
		{
			this->SetVisible(true); // replace
		}

		if ( state == EES_DANGLING )
		{
			this->SetVisible(false); 	
		}
	}

	void SetCenterType(ECenter_Type eCenterType);
	ECenter_Type GetCenterType();


	CModel(ISceneNode * pParent);
	
	~CModel();
	
	bool IsActive() { return m_eEditState == EES_MOVING; }
	
	void SetSelected(bool b)
	{ 
		m_bSelected = b; 
	}
	
	bool GetSelected() { return m_bSelected; }
	
	D3DXVECTOR3 const & GetCenter()
	{
		return 	m_vCenter;
	}
	
	ECenter_Type GetCenterTypeOrigin();

	void RestoreCenter(D3DXVECTOR3 & v);
	
	void Create(std::string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ);
	void Render();
	
	bool Intersected(D3DXVECTOR3 vDir,D3DXVECTOR3 vPos);

	void SetCenterTypeOrigin(ECenter_Type eCenterType);

	void SetBlockTypeByIndex(int nIndex, int nBlockType);

	void UpdateHeightBoxes();

	void SetHeightBoxesVisibility(bool bIsVisible);

	void IncreaseBoxHeightByIndex(int n);
	void DecreaseBoxHeightByIndex(int n);
	float GetBoxHeightByIndex(int n);
	void ClearBoxHeightByIndex(int n);

	void SetAnimationByAraFileName(std::string strAraFileName);

	void LinkEffect(IEffect * pEffect, IEffectGroup * pGroup);

	void UnlinkEffect();
};

}
}