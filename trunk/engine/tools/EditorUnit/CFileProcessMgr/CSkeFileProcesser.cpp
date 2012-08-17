#include "stdafx.h"
#include "CFileProcessMgr/CSkeFileProcesser.h"
#include "CEditAnimationsRes.h"
#include "CSkeCoder.h"
#include "CodeCvs.h"

CSkeFileProcesser::CSkeFileProcesser()
{
}

CSkeFileProcesser::~CSkeFileProcesser()
{
}

void CSkeFileProcesser::Do( const wchar_t* szfilename )
{
	string sz = utf16_to_gbk(szfilename);
	if(sz.find(".ske")==string::npos)
		return;

	CEditSkeletalFrame tSkeletalFrame(sz);
	tSkeletalFrame.BinLoad();

	CSkeFormat SkeFormat;
	SkeFormat.RegisterCoder(new CSkeCoder);
	SkeFormat.Save( sz, (CSkeletalFrame*)&tSkeletalFrame);
}

CTaskProcess::ExtNameSet 
	CSkeFileProcesser::GetExtNames()
{
	CTaskProcess::ExtNameSet ext_names;
	ext_names.insert(L".ske");

	return ext_names;
}

const string CSkeFileProcesser::GetName() const
{
	return string("ske -> chunk ¸ñÊ½");
}