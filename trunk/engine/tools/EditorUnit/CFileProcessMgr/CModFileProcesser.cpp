#include "stdafx.h"
#include "CFileProcessMgr/CModFileProcesser.h"
#include "CEditPieceGroup.h"
#include "CModCoder.h"
#include "CodeCvs.h"
#include "BaseHelper.h"
CModFileProcesser::CModFileProcesser()
{
}

CModFileProcesser::~CModFileProcesser()
{
}

void CModFileProcesser::Do( const wchar_t* szfilename )
{
	string sz = utf16_to_gbk(szfilename);
	if(sz.find(".mod")==string::npos)
		return;

	CEditPieceGroup *PieceGroup = new CEditPieceGroup;
	PieceGroup->AddRef();

	if( FAILED(PieceGroup->ReLoad( sz )) )
	{
		SafeRelease(PieceGroup);
		PieceGroup = NULL;
	}
	else
	{
		size_t npos = sz.rfind(".mod");
		if( npos != - 1 )
		{
			sz = sz.substr(0, npos);
			sz += ".mod";
		}

		CModFormat ModFormat;
		ModFormat.RegisterCoder(new CPieceGroupCoder);
		ModFormat.Save(sz,(CEditPieceGroup*)PieceGroup);

		SafeRelease(PieceGroup);
	}
}

CTaskProcess::ExtNameSet 
	CModFileProcesser::GetExtNames()
{
	CTaskProcess::ExtNameSet ext_names;
	ext_names.insert(L".mod");

	return ext_names;
}

const string CModFileProcesser::GetName() const
{
	return string("mod -> chunk ¸ñÊ½");
}