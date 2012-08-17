#include "stdafx.h"
#include "CSceneConfigClient.h"
#include "CIniFile.h"
#include "BaseMath.h"

CSceneConfigClient::CSceneConfigClient(const string& sSceneConfig,CIniFile* pIniFile)
:CSceneConfig(sSceneConfig)
{
	m_pIniFile = pIniFile;

	m_iWndWidth	 =	GetSystemMetrics(SM_CXSCREEN);
	m_iWndHeight =	GetSystemMetrics(SM_CYSCREEN);

	m_iWndWidth = max(m_iWndWidth,800);
	m_iWndHeight= max(m_iWndHeight,600);
	
	SetGuiZoom(GetDefaultUIScale());	

	LoadUserConfig();
}

CSceneConfigClient::~CSceneConfigClient(void)
{
}


void CSceneConfigClient::LoadUserConfig()
{
	m_iWndWidth		= m_pIniFile->GetValue("Graphic", "ScreenWidth", m_iWndWidth);
	m_iWndHeight	= m_pIniFile->GetValue("Graphic", "ScreenHeight", m_iWndHeight);
	m_iAntiAlias	= limit2(m_pIniFile->GetValue("Graphic", "AntiAlias", m_iAntiAlias),uint(0),uint(4));
	m_iShadowType	= limit2(m_pIniFile->GetValue("Graphic", "Shadow", m_iShadowType),uint(0),uint(2));
	m_iTerrainType	= m_pIniFile->GetValue("Graphic", "ShaderTile", m_iTerrainType);
	m_iWaterType	= m_pIniFile->GetValue("Graphic", "Water", m_iWaterType);
	m_bFullScreen	= m_pIniFile->GetValue("Graphic", "FullScreen", m_bFullScreen);
	m_bVSync		= m_pIniFile->GetValue("Graphic", "VSync", m_bVSync);
	m_bBloom		= m_pIniFile->GetValue("Graphic", "Bloom", m_bBloom);
	m_bRefract		= m_pIniFile->GetValue("Graphic", "Refract", m_bRefract);
	m_fEffectLevel	= m_pIniFile->GetValue("Graphic", "EffectLevel", m_fEffectLevel);
	m_bCameraShake	= m_pIniFile->GetValue("Graphic", "CameraShake", m_bCameraShake);	
	m_uTexQuailty	= m_pIniFile->GetValue("Graphic", "TexQuailty", m_uTexQuailty);
	m_TexHMem		= m_pIniFile->GetValue("Graphic", "TexHQmem", m_TexHMem);
	m_TexMMem		= m_pIniFile->GetValue("Graphic", "TexMQmem", m_TexMMem);
	m_fGUIZoom		= m_pIniFile->GetValue("Graphic", "GuiZoom", m_fGUIZoom);
	m_bIsFreeCamera = true;//m_pIniFile->GetValue("Graphic", "FreeCamera", m_bIsFreeCamera);
	m_volume["MusicVolume"]	= m_pIniFile->GetValue("Audio", "Music", m_volume["MusicVolume"]);
	m_volume["EffectVolume"]= m_pIniFile->GetValue("Audio", "Effect", m_volume["EffectVolume"]);
	m_volume["AmbientVolume"]= m_pIniFile->GetValue("Audio", "Ambient", m_volume["AmbientVolume"]);
	m_bVoiceSwitch	= m_pIniFile->GetValue("Audio", "Voice", m_bVoiceSwitch);
	m_MaxShader		= m_pIniFile->GetValue("System", "ShaderVer", m_MaxShader);
}


void CSceneConfigClient::SaveConfig()
{
	m_pIniFile->SetValue("Graphic", "RenderCyc", m_uRenderCyc);
	m_pIniFile->SetValue("Graphic", "ScreenWidth", m_iWndWidth);
	m_pIniFile->SetValue("Graphic", "ScreenHeight", m_iWndHeight);
	m_pIniFile->SetValue("Graphic", "AntiAlias", m_iAntiAlias);
	m_pIniFile->SetValue("Graphic", "Shadow", m_iShadowType);
	m_pIniFile->SetValue("Graphic", "ShaderTile", m_iTerrainType);
	m_pIniFile->SetValue("Graphic", "Water", m_iWaterType);
	m_pIniFile->SetValue("Graphic", "FullScreen", m_bFullScreen);
	m_pIniFile->SetValue("Graphic", "VSync", m_bVSync);
	m_pIniFile->SetValue("Graphic", "Bloom", m_bBloom);
	m_pIniFile->SetValue("Graphic", "Refract", m_bRefract);
	m_pIniFile->SetValue("Graphic", "EffectLevel", m_fEffectLevel);
	m_pIniFile->SetValue("Graphic", "CameraShake", m_bCameraShake);
	m_pIniFile->SetValue("Graphic", "TexQuailty",  m_uTexQuailty);
	m_pIniFile->SetValue("Graphic", "TexHQmem",  m_TexHMem);
	m_pIniFile->SetValue("Graphic", "TexMQmem",  m_TexMMem);
	m_pIniFile->SetValue("Graphic",	"GuiZoom", m_fGUIZoom );
	//m_pIniFile->SetValue("Graphic", "FreeCamera", m_bIsFreeCamera);
	m_pIniFile->SetValue("Audio", "Music", m_volume["MusicVolume"]);
	m_pIniFile->SetValue("Audio", "Effect", m_volume["EffectVolume"]);
	m_pIniFile->SetValue("Audio", "Ambient", m_volume["AmbientVolume"]);
	m_pIniFile->SetValue("Audio", "Voice", m_bVoiceSwitch);
	m_pIniFile->SetValue("System", "ShaderVer", m_MaxShader);
	m_pIniFile->Save();
}