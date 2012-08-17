//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"

namespace bg
{
namespace sc
{

class CCmdPlaceObject : public CCmd
{
	struct SDiffPlaceInfo
	{
		// real-time params
		int nHotGridIndex;
		Ptr<CTerrainMesh::CObject> pObject;
		
		// memento params
		std::vector<int> vecBlockGridIndices;
		SDiffPlaceInfo() : nHotGridIndex(INVALID) { }
		~SDiffPlaceInfo() {}
	};
	
	SDiffPlaceInfo m_diff;
	
public:
	CCmdPlaceObject() { }
	
	~CCmdPlaceObject() {	}

	virtual void Execute();
	virtual void UnExecute();
	
	void SetObject(CTerrainMesh::CObject * p) 
	{
		m_diff.pObject = p;
	}
	
	void SetHotGridIndex(int n) 
	{
		m_diff.nHotGridIndex = n;
	}
	
	void AddGridIndex(int n)
	{
		m_diff.vecBlockGridIndices.push_back(n);
	}
};

}
}