//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"

namespace bg
{
namespace sc
{

class CCmd;

class CCmdMgr
{
public:
	static CCmdMgr * GetInstance() { if ( !one ) one = new CCmdMgr; return one;} 
	
private:
	CCmdMgr();
	~CCmdMgr();
	static CCmdMgr * one;
	
	std::vector<CCmd*> m_vecCmds;
	CCmd * m_pCmdCur;
	
public:
	
	void Do(CCmd * pCmd);
	void ReDo();
	void UnDo();
	void Clear();
};

}
}