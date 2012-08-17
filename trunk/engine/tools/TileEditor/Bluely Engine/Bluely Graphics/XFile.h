//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "Operator.h"

namespace bg
{
namespace sc
{

class CXFile
{
public:
	static CXFile * GetInstance() { if ( !one ) one = new CXFile; return one;} 

private:
	CXFile() : m_pOperator(NULL),m_pTerrainMesh(NULL) {}
	virtual ~CXFile() {}
	static CXFile * one;

	COperator * m_pOperator;
	CTerrainMesh * m_pTerrainMesh;

public:
	void SetTerrainMesh(CTerrainMesh * pTerrainMesh); 
	void SetOperator(COperator * pOperator);
	CTerrainMesh * GetTerrainMesh() { return m_pTerrainMesh; }
	COperator * GetOperator() { return m_pOperator; }
};

}
}