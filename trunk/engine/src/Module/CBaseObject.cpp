#include "stdafx.h"
#include "CBaseObject.h"
#include "ExpHelper.h"

CBaseObject::CBaseObject(void)
{	
}

CBaseObject::~CBaseObject(void)
{	
}

void CBaseObject::SetObjectFlag(uint8 uFlagBit)
{
	GenErr("should not call this");
}

bool CBaseObject::VisibleToScript()const
{
	GenErr("should not call this");
	return true;
}

bool CBaseObject::CreatedByScript()const
{
	GenErr("should not call this");
	return true;
}

void CBaseObject::SetScriptRefNum(int32 num)
{
	GenErr("should not call this");
}

bool CBaseObject::RefByScript() const
{
	GenErr("should not call this");
	return true;
}

int32 CBaseObject::GetScriptRefNum() const
{
	GenErr("should not call this");
	return 0;
}

uint32 CBaseObject::GetBaseObjectID() const
{
	GenErr("should not call this");
	return 0;
}
