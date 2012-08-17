//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Input.h"
#include "TerrainMesh.h"

namespace bg
{
namespace sc
{

class CPicker
{
	D3DXVECTOR3 m_vecRay[2]; // ori and dir 
	Ptr<sc::CTerrainMesh::CObject> m_pLastSelected;

public:
	static CPicker * GetInstance() { if ( !one ) one = new CPicker; return one;} 

private:
	CPicker();
	virtual ~CPicker();
	static CPicker * one;

	std::vector<int> m_vecGridIndices;

	D3DXVECTOR3 GetSpecIntersectedPos(POINT pt);

public:
	int GetPickPos(sc::CTerrainMesh::CObject ** pp, D3DXVECTOR3 * pvHitPoint);
	D3DXVECTOR2 GetGridsCnt();
	void SetLastSelected(sc::CTerrainMesh::CObject * p);
	float GetHeight(float fX, float fZ);
	bool GetIntersected(D3DXVECTOR3 & vHitPoint);
	std::vector<int> & GetGridIndices();
	void Evaluate(D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, DWORD dwSearchCnt, std::vector<DWORD> & vecGridIndices);
};

}
}
