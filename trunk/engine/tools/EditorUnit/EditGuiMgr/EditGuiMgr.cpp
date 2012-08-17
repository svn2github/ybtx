#include "StdAfx.h"
#include "EditGuiMgr\EditGuiMgr.h"
#include "SQRRootWnd.h"
#include "CFontMgr.h"
#include "CLangCfg.h"
#include "CFontCfg.h"
#include "CGraphic.h"


CEditGuiMgr::CEditGuiMgr(void)
{
	m_pGUIRootWnd = new SQRRootWnd();

	InitLanguageSystem();
}

CEditGuiMgr::~CEditGuiMgr(void)
{
	m_pGUIRootWnd->DestroyWnd();
	CLangCfg::Destroy();
}

void CEditGuiMgr::InitLanguageSystem()
{
	SQR_TRY
	{
		// 初始化语言系统
		CLangCfg::Create();
		CLangCfg::GetInst()->LoadConfig("./LangConfig.xml");

		// 设置语言
		int nCurLan = 0;
		CFontCfg::Create();
		CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(nCurLan));
		CFontMgr::GetInst()->SetLangIndex(nCurLan);
		CFontMgr::GetInst()->SetOriLangFolder(CLangCfg::GetInst()->GetOriLanFolder());
		CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(nCurLan));
		CFontMgr::GetInst()->SetDisFontName(CFontCfg::GetInst()->GetDisFontName());
		CFontMgr::GetInst()->ClearFontInfo();
		CFontMgr::GetInst()->UnloadGdiFonts();

		size_t font_count = CFontCfg::GetInst()->GetGDIFontCount();
		for ( size_t font_index = 0; font_index < font_count; ++font_index )
		{
			wstring szFontName = CFontCfg::GetInst()->GetGDIFontName(font_index);
			string  szFontFile = CFontCfg::GetInst()->GetGDIFontFileName(font_index);
			CFontMgr::GetInst()->AddFontInfo(szFontName.c_str(), szFontFile.c_str());
		}
	}
	SQR_CATCH(exp)
	{
		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

		return;
	}
	SQR_TRY_END;
}

void CEditGuiMgr::CreateGuiRootWnd(RECT rect)
{
	wstring m_strGUIPath = L"tgt";
	wstring strResPath = m_strGUIPath + L"\\Layout";
	wstring styPath    = m_strGUIPath + L"\\";

	CFRect rt(rect);
	m_pGUIRootWnd->CreateRootWnd( NULL, rt, CGraphic::GetInst(), 0, NULL, utf16_to_utf8(strResPath).c_str(), utf16_to_utf8(styPath).c_str(), TRUE );

	// 加载GUI静态文本资源
	m_pGUIRootWnd->LoadStaticText();
}

void CEditGuiMgr::CreateDialogFromRes(const char* szWindowNameInResFile)
{
	
}

void CEditGuiMgr::ResizeGuiRootWnd(RECT rect)
{
	if( m_pGUIRootWnd )
	{
		CFRect rt(rect);
		m_pGUIRootWnd->ResizeRect(rt);
	}
}

SQRRootWnd* CEditGuiMgr::GetGuiRootWnd()
{
	return m_pGUIRootWnd;
}

LRESULT CEditGuiMgr::RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam )
{
	return m_pGUIRootWnd->RecieveMsg(message, wParam, lParam);
}

