#include "stdafx.h"
#include "TAppConfig.inl"
#include "CAppConfigClient.h"
#include "CSceneConfigClient.h"
#include "CIniFile.h"
#include "basehelper.h"
#include "CodeCvs.h"
#include "basemath.h"
#include "CFontCfg.h"
#include "CLangCfg.h"

template class TAppConfig<CTraitsClient>;

static const char* GetConfigVar(TiXmlNode* pNode, const char* ParentName, const char* SubName)
{
	return pNode->FirstChild(ParentName)
		->FirstChild(SubName)->ToElement()->GetText();
}

static const float fPI=3.14159265f;	

CAppConfigClient::CAppConfigClient(istream& strmConfig, const string& SceneConfig )
:ParentConfig_t(strmConfig)
{
	wstring iniPath		= utf8_to_utf16( Get<string>("system_config","ini_path") );

	m_pIniFile = new CIniFile();
	
	if( !m_pIniFile->Open(L"", iniPath.c_str()) )
		m_pIniFile->Create(iniPath.c_str());

	m_pSceneConfig = new CSceneConfigClient( SceneConfig,m_pIniFile );

	CLangCfg::Create();
	CLangCfg::GetInst()->LoadConfig("./LangConfig.xml");
	CFontCfg::Create();
	CFontCfg::GetInst()->LoadFongConfig(CLangCfg::GetInst()->GetLanFolder(0));

	m_uSkipFrameWaitTime		= Get<uint>( "system_config","skip_frame_waittime" );
	m_uRenderCycInBackground	= Get<uint>( "system_config","bgrendercyc" );
	
	m_strLogPath += utf8_to_utf16( Get<string>("system_config","log_path") );

	m_bTestingClient = Get<bool>( "testing_config","testing_client" );
	m_bErrMsgShow = Get<bool>( "testing_config","err_msg_show" );
	
	m_bAntiDebug = !DftGet<string>( "" , "system_config" , "extrainfo" ).empty();

	m_strAppIconFileName = utf8_to_utf16( Get<string>( "system_config", "app_icon" ));

	m_bReload			= Get<bool>( "Reload" );
	m_pSceneConfig->SetRenderCyc(m_pIniFile->GetValue("Graphic", "RenderCyc", Get<uint>( "system_config","fgrendercyc" )));
}

CAppConfigClient::~CAppConfigClient(void)
{
	SafeDelete(m_pIniFile);

	if ( CLangCfg::HasInst() )
		CLangCfg::Destroy();

	if ( CFontCfg::HasInst() )
		CFontCfg::Destroy();

	delete m_pSceneConfig;
}

//------------------------------------------------------------------------------
//∑µªÿƒ¨»œ≈‰÷√
uint32 CAppConfigClient::GetDefaultRenderCyc() const
{
	return Get<uint>( "system_config","fgrendercyc" );
}

uint32 CAppConfigClient::GetDefaultWndWidth() const
{
	return GetSystemMetrics(SM_CXSCREEN);
}

uint32 CAppConfigClient::GetDefaultWndHeight() const
{
	return GetSystemMetrics(SM_CYSCREEN);
}

uint CAppConfigClient::GetDefaultLanguage() const
{
	return 0;
}

//------------------------------------------------------------------------------
string CAppConfigClient::GetGuiRes()const
{
	return Get<string>( "system_config" , "gui_res" );
}

//------------------------------------------------------------------------------
string CAppConfigClient::GetEmtRes()const
{
	return Get<string>( "system_config" , "emt_res" );
}

//------------------------------------------------------------------------------
string CAppConfigClient::GetGuiFxPath() const
{
	return Get<string>( "system_config" , "gui_fx" );
}

//------------------------------------------------------------------------------
string CAppConfigClient::GetCoreDumpMsg()const
{
	return Get<string>( "system_config" , "core_dump_msg" );
}


//------------------------------------------------------------------------------
wstring CAppConfigClient::GetWindowTitle()const
{
	return utf8_to_utf16( Get<string>( "window_config" , "title" ) );
}

//------------------------------------------------------------------------------
string CAppConfigClient::GetGTPortName()const
{
	return Get<string>( "tg_config" , "port_name" );
}


//------------------------------------------------------------------------------
const string& CAppConfigClient::GetLogUploadFaultMsg() const
{
	return m_bLogUploadFaultMsg;
}

//------------------------------------------------------------------------------
bool CAppConfigClient::IsAntiDebug() const
{
	return m_bAntiDebug;
}

bool CAppConfigClient::GetReload() const
{
	return m_bReload;
}

//------------------------------------------------------------------------------
uint32 CAppConfigClient::GetRenderCycInForground()const
{
	return m_pSceneConfig->GetRenderCyc();
}

void CAppConfigClient::SetRenderCycInForground( uint32 cyc )
{
	m_pSceneConfig->SetRenderCyc(cyc);
}

