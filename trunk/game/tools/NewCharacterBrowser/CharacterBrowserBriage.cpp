#pragma once

#include "StdAfx.h"

//BRIAGE
#include "CharacterBrowserBriage.h"
#include "CEditorConfig.h"
#include "CEditorDevConfig.h"
#include "EditorExpMgr.h"
#include "ScriptMgr.h"
#include "CCoreViewImp.h"
#include "CharacterBrowserContext.h"
#include "BaseHelper.h"
#include "GTRenderMgr.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

namespace sqr
{
	CharacterBrowserBriage::CharacterBrowserBriage(void)
	{
		m_pShowWndHandle = NULL;

		m_pContext		 = new CCharacterBrowserContext;
	}

	CharacterBrowserBriage::~CharacterBrowserBriage(void)
	{
		this->Release();
	}

	void CharacterBrowserBriage::Release()
	{
		if( CGTRenderMgr::HasInst() )
			CGTRenderMgr::Destroy();

		SafeDelete(m_pContext);
	}

	bool CharacterBrowserBriage::InitEnviroment()
	{
		this->InitConfig();
		this->InitLua();

		return true;
	}

	void CharacterBrowserBriage::OnIdle()
	{
	}

	void CharacterBrowserBriage::OnSize()
	{
		//更新界面大小
	}

	void CharacterBrowserBriage::InitConfig()
	{
		//载入配置文件
		CEditorConfig::GetInst()->InitEditorConfig();
 		//设置工作路径
//  		m_PathBrowser.Initialize();
//  		if( !m_PathBrowser.HasSetWorkPath() )
//  		{
//  			m_PathBrowser.DoModal();
//  		}
		m_PathBrowser.SetResPkgFindPath();

		bool bcoreDumpIsEnabled = false;
		bool bErrorMsgShow		= true;
		if( CEditorDevConfig::HasInst() )
		{
			bcoreDumpIsEnabled = CEditorDevConfig::GetInst()->GetCoreDumpIsEnabled();
			bErrorMsgShow      = CEditorDevConfig::GetInst()->GetErrMsgIsShow();
		}

		string homePath = CEditorConfig::GetInst()->GetEditorAppPath();
		homePath += "\\CharacterBrowser";
		InitExpHandler(homePath.c_str(), bcoreDumpIsEnabled, bErrorMsgShow);
		EnableCatchError(true);	
	}

	void CharacterBrowserBriage::InitLua()
	{
		Init();
	}

	void CharacterBrowserBriage::RegisterWinFormHandle( void* handle )
	{
		m_pShowWndHandle = handle;
	}

	void CharacterBrowserBriage::InitRenderMgr()
	{
		//new CGTRenderMgr(m_pMainView->GetRenderGroup());
	}

}
