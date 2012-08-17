#include "stdafx.h"
#include "CLanguageSetting.h"
#include "CFontMgr.h"
#include "CLangCfg.h"
#include "CSceneConfig.h"
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
	CFontMgr::GetInst()->SetLangIndex(nLangIndex);
	CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(nLangIndex));
	CFontMgr::GetInst()->SetDisFontName(CLangCfg::GetInst()->GetDisFontName(nLangIndex));
	CFontMgr::GetInst()->SetGDIDefFontName(CLangCfg::GetInst()->GetGDIDefFontName(nLangIndex));

	size_t font_count = CLangCfg::GetInst()->GetGDIFontCount(nLangIndex);
	for ( size_t font_index = 0; font_index < font_count; ++font_index )
	{
		wstring szFontName = CLangCfg::GetInst()->GetGDIFontName(nLangIndex, font_index);
		string  szFontFile = CLangCfg::GetInst()->GetGDIFontFileName(nLangIndex, font_index);
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
	// zjtemp
	return 1;
}

int32 CLanguageSetting::GetCurLanIndex()
{
	return CAppConfigClient::Inst()->GetLangID();
}

const char* CLanguageSetting::GetLanguageName(int32 nLangIndex)
{
	m_szTmp = CLangCfg::GetInst()->GetLanName(nLangIndex);
	return m_szTmp.c_str();
}

void CLanguageSetting::saveCfg( size_t nLangIndex )
{
	CAppConfigClient::Inst()->SetLangID(nLangIndex);
	CAppConfigClient::Inst()->SaveIniConfig();
}

void CLanguageSetting::clearLangInfo()
{
	CFontMgr::GetInst()->ClearFontInfo();
	CFontMgr::GetInst()->UnloadGdiFonts();
}