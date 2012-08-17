//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Input.h"
#include "TerrainMesh.h"
#include "CTileEditorModel.h"
#include "../../TileEditor/Set.h"

namespace bg
{
namespace sc
{

class CToolSetModel
{
	HWND m_hResSetView;
	DWORD m_dwShaderHandle;
    	
public:
	CToolSetModel();
	~CToolSetModel();
	
	void CreateActiveObject(std::vector<std::string> & veCTileEditorModelFileNames, std::vector< std::vector<sc::CTileEditorModel::SBlockInfo> > & vvBlock, std::vector< std::vector<sc::CTileEditorModel::SLogicHeightInfo> > & vvHeight, bool bSubUnit, CTileEditorModel::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ);
	
	void AdjustActiveObjectCenterType(sc::CTileEditorModel::ECenter_Type e);

	void SetBlockType(int nGridIndex, int nBlockType);
	
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();

	void Create(HWND hResSetView);
};

}
}