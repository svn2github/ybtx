#include "stdafx.h"
#include "Cmd.h"
#include "CmdMgr.h"

CCmd::CCmd() 
{}

CCmd::~CCmd()
{}

void CCmd::Execute(void)
{
	CCmdMgr::GetInst()->Do(this);
}

void CCmd::Destroy( void )
{
	delete this;
}

bool CCmd::IsValid( void )
{
	return true;
}