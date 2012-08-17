//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cmd.h"

namespace bly
{

class CCmdDeleteBlock : public CCmd
{
	struct SDiffPlaceInfo
	{
		// real-time params
		std::vector<int> vecBlockGridIndices;
		std::vector<Ptr<CTerrainMesh::CObject>> pObject;
		std::vector<int> vecSize;
		std::vector<int> vecBlockType;

		SDiffPlaceInfo() { }
		~SDiffPlaceInfo() {}
	};

	SDiffPlaceInfo m_diff;

public:
	CCmdDeleteBlock()
	{
		;
	}

	~CCmdDeleteBlock()
	{
		;
	}

	virtual void Execute();
	virtual void UnExecute();

	void AddGridIndex(int n)
	{
		if ( n == INVALID )
			return;

		m_diff.vecBlockGridIndices.push_back(n);
	}

	//void SetObject(CTerrainMesh::CObject * p)
	//{
	//	m_diff.pObject = p;
	//}

};

}