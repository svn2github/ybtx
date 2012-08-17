#include "stdafx.h"
#include "CFileProcessMgr/CAgpFileProcesser.h"
#include "CEditAnimationsRes.h"
#include "CAgpCoder.h"
#include "CAniCoder.h"
#include "CodeCvs.h"

CAgpFileProcesser::CAgpFileProcesser()
{
}

CAgpFileProcesser::~CAgpFileProcesser()
{
}

void CAgpFileProcesser::Do( const wchar_t* szfilename )
{
	string sz = utf16_to_gbk(szfilename);
	if(sz.find(".agp")==string::npos)
		return;

	CEditAnimationGroup* pAnitmateGroup = new CEditAnimationGroup(sz);
	pAnitmateGroup->AddRef();
	
	if(pAnitmateGroup->BinLoad()<0)
	{
		//
	}
	else
	{
		CAgpFormat AgpFormat;
		AgpFormat.RegisterCoder(new CAgpCoder);
		AgpFormat.Save( sz, (CAnimationGroup*)pAnitmateGroup);
	}

	pAnitmateGroup->Release();
}

CTaskProcess::ExtNameSet 
	CAgpFileProcesser::GetExtNames()
{
	CTaskProcess::ExtNameSet ext_names;
	ext_names.insert(L".agp");

	return ext_names;
}

const string CAgpFileProcesser::GetName() const
{
	return string("agp -> chunk ∏Ò Ω");
}