//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "CTileEditorModel.h"

namespace bg
{
namespace sc
{

class CFace : public CRootObject
{
public:
	enum ETrans_Type { ETT_TRANSLUCENT = 0, ETT_SOLID, ETT_COUNT };
	
private:
	std::vector<sc::CTileEditorModel *> m_vecSelfPresentation;
	std::vector<std::string> m_veCTileEditorModelFileNames;
	
	bool m_bSubUnit;
	
	ETrans_Type m_eTransType;
	
	CTileEditorModel::ECenter_Type m_eCenterType;
	
public:	
	CFace();
	~CFace();
	
	void AddContainer(std::vector<sc::CTileEditorModel *> & vec,bool bSubUnit, CTileEditorModel::ECenter_Type eBlockType, int nXDelta, int nZDelta);
	sc::CTileEditorModel * GetSelfPresentation(int n); 
	int GetSelfPresentationCount(); 
	void CleanBlockGridIndices();
	void SetModelFileNames(std::vector<std::string> & vec);
	std::vector<std::string> & GetModelFileNames();
	
	void SetEditState(CTileEditorModel::EEdit_State e);
	
	void SetCenterGridIndex(int n);
	
	void SetLocked(bool);

	void InitPlaced();

	void SetPlacedEditState(CTileEditorModel::EEdit_State_Placed);
	
	void UpdateLift(int n);
	void UpdateShiftX(int n);
	void UpdateShiftZ(int n);
	void UpdateRotation();
	
	void Render();

	bool Intersected(D3DXVECTOR3 vDir,D3DXVECTOR3 vPos);
	bool GetIsSubUnit();
	CTileEditorModel::ECenter_Type GetBlockType();

	void SetCenterType(sc::CTileEditorModel::ECenter_Type e);

	void SetSelected(bool);

	void SetHeightBoxesVisibility(bool bIsVisible);

	void SetTransType(ETrans_Type e);

	void SetAnimationByAraFileName(std::string strAraFileName);

	void LinkEffect(IEffect * pEffect, IEffectGroup * pGroup);

	void UnlinkEffect();
};

}
}
