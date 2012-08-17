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

namespace bg
{
namespace sc
{

class CCmdDeleteObject : public CCmd
{
	struct SDiffDeleteInfo
	{
		// real time params
		Ptr<CTerrainMesh::CObject> pObject;

		// memento
		int nCenterGridIndex;
		std::vector<int> vecBlockGridIndices;
		SDiffDeleteInfo() : nCenterGridIndex(INVALID), pObject(NULL) {}
		~SDiffDeleteInfo() {}
	};

	SDiffDeleteInfo m_diff;

public:
	CCmdDeleteObject() { }
	
	~CCmdDeleteObject() { }

	virtual void Execute();
	virtual void UnExecute();

	void SetObject(CTerrainMesh::CObject * p) 
	{
		m_diff.pObject = p;
	}

	void SetCenterGridIndex(int n) 
	{
		m_diff.nCenterGridIndex = n;
	}

	void AddBlockGridIndex(int n)
	{
		m_diff.vecBlockGridIndices.push_back(n);
	}
};

}
}