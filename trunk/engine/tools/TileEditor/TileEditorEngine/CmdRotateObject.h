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

class CCmdRotateObject : public CCmd
{
	struct SDiffRotateInfo
	{
		Ptr<CTerrainMesh::CObject> pObject;
	};
	
	SDiffRotateInfo m_diff;

public:
	CCmdRotateObject() { }

	~CCmdRotateObject() { }

	virtual void Execute();
	virtual void UnExecute();

	void SetObject(CTerrainMesh::CObject * p) 
	{
		m_diff.pObject = p;
	}
};

}
}