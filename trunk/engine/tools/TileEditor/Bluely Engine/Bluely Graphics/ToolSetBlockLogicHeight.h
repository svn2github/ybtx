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

class CToolSetBlockLogicHeight
{
	HWND m_hResSetView;
	DWORD m_dwShaderHandle;
	std::vector<DWORD> m_vecActiveVertexIndices;
	
	std::vector<int> m_vecActiveGrids;

	struct SProcessParams           
	{
		SProcessParams()
		{
			dwBrushSizeInX = 1;
			dwBrushSizeInZ = 1;
		}

		DWORD dwBrushSizeInX;
		DWORD dwBrushSizeInZ;
	};
	
	SProcessParams m_processParams;
	
public:
	CToolSetBlockLogicHeight();
	~CToolSetBlockLogicHeight();
	
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();
	
	void Create(HWND hResSetView);
	
	struct SBlockLogicHeightExInfo
	{
		int nModelIndex;
		int nVertexIndexIndex;
		float fLogicHeight;
	};
	
	DWORD GetBrushSizeInX();
	void SetBrushSizeInX(DWORD dw);
	void IncBrushSizeInX();
	void DecBrushSizeInX();

	DWORD GetBrushSizeInZ();
	void SetBrushSizeInZ(DWORD dw);
	void IncBrushSizeInZ();
	void DecBrushSizeInZ();
	
	std::vector<DWORD> & GetBrushVertexIndices();
};

}
}