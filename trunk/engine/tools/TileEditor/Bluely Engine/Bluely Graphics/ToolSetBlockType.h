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

class CToolSetBlockType
{
	HWND m_hResSetView;
	DWORD m_dwShaderHandle;
	std::vector<int> m_vecActiveGrids;
   	
public:
	CToolSetBlockType();
	~CToolSetBlockType();

	void SetBlockType(int nBlockType);
	
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();

	void Create(HWND hResSetView);

	struct SBlockTypeExInfo
	{
		int nModelIndex;
		int nGridIndexIndex;
		int nBlockType;
	};
};

}
}