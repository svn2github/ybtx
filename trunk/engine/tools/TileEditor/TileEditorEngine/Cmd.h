//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "CmdMgr.h"

namespace bg
{
namespace sc
{

class CCmd
{
	CCmd * m_pCmdPre;
	CCmd * m_pCmdNxt;

	bool m_bExecuted;
	bool m_bChained;

public:
	CCmd() : m_pCmdPre(NULL), m_pCmdNxt(NULL), m_bExecuted(false), m_bChained(true) { }
	virtual ~CCmd() { }
	
	CCmd * GetCmdPre() { return m_pCmdPre; }
	CCmd * GetCmdNxt() { return m_pCmdNxt; }
	
	void  SetCmdPre(CCmd * pCmdPre) { m_pCmdPre = pCmdPre;}
	void  SetCmdNxt(CCmd * pCmdNxt) { m_pCmdNxt = pCmdNxt;}
	
	virtual void Execute() { m_bExecuted = true; }
	virtual void UnExecute() { m_bExecuted = false; }
	
	bool DoExecuted() const { return m_bExecuted;}
	
	void SetChained(bool b) { m_bChained = b; }
};

}
}