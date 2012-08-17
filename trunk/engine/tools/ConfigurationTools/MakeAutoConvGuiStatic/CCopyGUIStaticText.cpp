#include "stdafx.h"
#include "CCopyGUIStaticText.h"
#include "CSplitInfo.h"
#include "common.h"
#include <set>
#include "ErrLogHelper.h"
#include "conio.h"
#include "CExpLog.h"

void CCopyGUIStaticText::StartProcess()
{
	CSplitInfo* pInst = CSplitInfo::GetInst();
	string strGuiStaticText = pInst->GetGuiStaticText();
	string strFrom = pInst->GetLangPath() + "/" + pInst->GetOriLang() + "/" + pInst->GetGuiFolder() + "/" + strGuiStaticText;
	string strTo = pInst->GetLangSrcPath() + "/" + pInst->GetGuiFolder() + "/" + strGuiStaticText;
	CreateFileDirectory(strTo);
	CopyFile(strFrom.c_str(), strTo.c_str(), false);
}