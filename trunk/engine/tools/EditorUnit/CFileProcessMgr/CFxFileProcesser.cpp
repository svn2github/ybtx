#include "stdafx.h"
#include "CFxFileProcesser.h"
#include "CEditEffectManager.h"
#include "CGraphic.h"
#include "CEffectCoder.h"
#include "CodeCvs.h"

void CFxFileProcesser::Do( const wchar_t* szfilename )
{
	string sz = utf16_to_gbk(szfilename);
	if(sz.find(".efx")==string::npos)
		return;
	CEditEffectManager* pManager = dynamic_cast<CEditEffectManager*>(CEditEffectManager::GetInst());
	pManager->LoadEffectGroups(sz);
	pManager->SaveEffectGroupsByChunk(sz);
}

CTaskProcess::ExtNameSet 
CFxFileProcesser::GetExtNames()
{
	CTaskProcess::ExtNameSet ext_names;
	ext_names.insert(L".efx");

	return ext_names;
}

const string CFxFileProcesser::GetName() const
{
	return string("efx -> chunk ∏Ò Ω");
}