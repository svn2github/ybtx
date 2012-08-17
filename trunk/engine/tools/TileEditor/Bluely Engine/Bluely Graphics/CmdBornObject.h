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

class CCmdBornObject : public CCmd
{
	struct SDiffBornInfo
	{
		Ptr<CTerrainMesh::CObject> pObject;
		Ptr<CTerrainMesh::CObject> pPreActiveObject;
		std::vector<std::string> vecModelFileNames;
		SDiffBornInfo() : pObject(NULL),pPreActiveObject(NULL),bSubUnit(false),bBlockType(CModel::EBlock_Type::EBT_NONE) { }
		~SDiffBornInfo() {}
		bool bSubUnit;
		CModel::EBlock_Type bBlockType;
	};

	SDiffBornInfo m_diff;

public:
	CCmdBornObject() { }
	
	~CCmdBornObject() { }
	
	virtual void Execute();
	virtual void UnExecute();
	
	void SetModelFileNames(vector<std::string> const & vec) 
	{
		m_diff.vecModelFileNames = vec;
	}
	
	void SetSubUnit(bool b)
	{
		m_diff.bSubUnit = b;
	}

	void SetBlockType(CModel::EBlock_Type eBlockType)
	{
		m_diff.bBlockType = eBlockType;
	}
};

}
}