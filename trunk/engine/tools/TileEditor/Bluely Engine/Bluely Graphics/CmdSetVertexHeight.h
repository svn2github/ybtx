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

class CCmdSetVertexHeight : public CCmd
{
public:
	struct SVertex
	{
		DWORD    dwVertexIndex;
		float    fHeight;
	};

private:
	std::vector<SVertex> m_vecDiffs;
	
	COperator::SProcessParams m_processParams;

	DWORD m_dwVertexCnt;

	class IsEqual
	{
		SVertex v;
	public:
		IsEqual(SVertex s) : v(s) { }
		~IsEqual() { }
		bool operator ()(SVertex s) { return s.dwVertexIndex == v.dwVertexIndex; }
	};

public:
	CCmdSetVertexHeight() : m_dwVertexCnt(0)
	{
		::ZeroMemory(&m_processParams,sizeof(COperator::SProcessParams));
	}
	
	~CCmdSetVertexHeight() { }
	
	virtual void Execute();
	virtual void UnExecute();
	void AddVertex(DWORD dwVertexIndex);
	DWORD GetVertexCnt() { return m_dwVertexCnt; }
};

}
}