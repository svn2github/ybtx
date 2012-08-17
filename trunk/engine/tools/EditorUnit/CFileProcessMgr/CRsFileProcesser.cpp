#include "stdafx.h"
#include "CFileProcessMgr/CRsFileProcesser.h"

CRsFileProcesser::CRsFileProcesser()
{
}

CRsFileProcesser::~CRsFileProcesser()
{
}

void CRsFileProcesser::Do( const wchar_t* szfilename )
{
	
}

CTaskProcess::ExtNameSet 
	CRsFileProcesser::GetExtNames()
{
	CTaskProcess::ExtNameSet ext_names;
	ext_names.insert(L".marl");

	return ext_names;
}

const string CRsFileProcesser::GetName() const
{
	return string("rs -> chunk ¸ñÊ½");
}