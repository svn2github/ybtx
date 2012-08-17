#include "stdafx.h"
#include "CLanguageSetting.h"
#include "CFontMgr.h"
#include "CLangCfg.h"
#include "CFontCfg.h"
#include "CAppConfigClient.h"
#include "CodeCvs.h"

CLanguageSetting::CLanguageSetting()
	: m_CurLangIndex(0)
{
}

CLanguageSetting::~CLanguageSetting()
{
}

CLanguageSetting* CLanguageSetting::Inst()
{
	static CLanguageSetting inst;
	return &inst;
}

void CLanguageSetting::SetCurLangID( size_t nLangIndex )
{
	CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(nLangIndex));

	CFontMgr::GetInst()->SetLangIndex(nLangIndex);
	CFontMgr::GetInst()->SetOriLangFolder(CLangCfg::GetInst()->GetOriLanFolder());
	CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(nLangIndex));
	CFontMgr::GetInst()->SetDisFontName(CFontCfg::GetInst()->GetDisFontName());
	CFontMgr::GetInst()->SetGDIDefFontName(CFontCfg::GetInst()->GetGDIDefFontName());

	size_t font_count = CFontCfg::GetInst()->GetGDIFontCount();
	for ( size_t font_index = 0; font_index < font_count; ++font_index )
	{
		wstring szFontName = CFontCfg::GetInst()->GetGDIFontName(font_index);
		string  szFontFile = CFontCfg::GetInst()->GetGDIFontFileName(font_index);
		CFontMgr::GetInst()->AddFontInfo(szFontName.c_str(), szFontFile.c_str());
	}
	CFontMgr::GetInst()->LoadGDIFonts();
}

void CLanguageSetting::ChangeLanguage(size_t nLangIndex)
{
	clearLangInfo();
	SetCurLangID(nLangIndex);
	saveCfg(nLangIndex);
}

int32 CLanguageSetting::GetLanguageCount()
{
	// return CLangCfg::GetInst()->GetLanCount();
	return 1;
}

int32 CLanguageSetting::GetCurLanIndex()
{
	return 0;
}

const char* CLanguageSetting::GetLanguageName(int32 nLangIndex)
{
	m_szTmp = CLangCfg::GetInst()->GetLanName(nLangIndex);
	return m_szTmp.c_str();
}

void CLanguageSetting::saveCfg( size_t nLangIndex )
{
}

void CLanguageSetting::clearLangInfo()
{
	CFontMgr::GetInst()->ClearFontInfo();
	CFontMgr::GetInst()->UnloadGdiFonts();
}