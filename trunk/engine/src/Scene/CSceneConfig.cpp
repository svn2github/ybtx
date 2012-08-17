#include "stdafx.h"
#include "CSceneConfig.h"
#include "PkgStream.h"
#include "CXmlConfig.h"
#include "CXmlConfig.inl"
#include "CodeCvs.h"
#include "CMath.h"
#include "CCameraConfig.h"
#include "CLangCfg.h"
#include "CFontCfg.h"
#include "CFontMgr.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CSceneConfig::CSceneConfig()
: m_pXmlConfig(NULL)
, m_uRenderCyc(16)
, m_iWndWidth(0)
, m_iWndHeight(0)
{
}

CSceneConfig::CSceneConfig(const string& ConfigFile)
: m_strConfigFile(ConfigFile.c_str())
, m_pXmlConfig(NULL)
, m_uRenderCyc(16)
, m_iWndWidth(0)
, m_iWndHeight(0)
{
	ipkgstream iConfigStream(L"", m_strConfigFile.c_str());
	if(!iConfigStream)
		return;
	m_pXmlConfig = new CXmlConfig( "scene_config" , iConfigStream );
	LoadDefaultConfig();
	LoadCameraConfig();
}

CSceneConfig::CSceneConfig(const string& ConfigFile,TiXmlElement* Node)
: m_strConfigFile(ConfigFile.c_str())
, m_pXmlConfig(NULL)
, m_uRenderCyc(16)
, m_iWndWidth(0)
, m_iWndHeight(0)
{
	Ast(Node!=NULL);
	
	m_pXmlConfig = new CXmlConfig(Node);
	LoadDefaultConfig();
	LoadCameraConfig();
}

CSceneConfig::~CSceneConfig()
{
	CCameraConfig::Destroy();

	delete m_pXmlConfig;
}

void CSceneConfig::SaveConfig(void)
{
	return;
}

void CSceneConfig::LoadDefaultConfig(void)
{
	//相机
	m_strCameraConfigpath = m_pXmlConfig->Get<const char*>( "camera_config","mode_path" );
	//场景
	m_bLockRatio	= m_pXmlConfig->DftGet<bool>( false,"graph_config","lockratio" );
	m_bShowFps		= m_pXmlConfig->DftGet<bool>( false,"graph_config","showfps" );
	m_bBloom		= m_pXmlConfig->DftGet<bool>( false,"graph_config","bloom" );
	m_bRefract		= m_pXmlConfig->DftGet<bool>( false,"graph_config","refract" );
	m_bVSync		= m_pXmlConfig->DftGet<bool>( false,"graph_config","vsynchronize" );
	m_bFullScreen	= m_pXmlConfig->DftGet<bool>( false,"graph_config","fullscreen" );
	m_iAntiAlias	= m_pXmlConfig->DftGet<uint32>( uint32( 0),"graph_config","antialias" );
	m_iShadowType	= m_pXmlConfig->DftGet<uint32>( uint32( 0),"graph_config","shadow" );
	m_iWaterType	= m_pXmlConfig->DftGet<uint32>( uint32( 0),"graph_config","water" );
	m_iColorDepth	= m_pXmlConfig->DftGet<uint32>( uint32(32),"graph_config","color_depth" );
	m_uTexQuailty	= m_pXmlConfig->DftGet<uint32>( uint32( 0),"graph_config","texquality" );
	m_bCameraShake	= m_pXmlConfig->DftGet<bool>( true,"graph_config","bcamerashake" );

	m_fMinFogDist	= m_pXmlConfig->DftGet<float>( 0.0f,"graph_config","min_fog_dist" );
	m_fMaxFogDist	= m_pXmlConfig->DftGet<float>( 0.0f,"graph_config","max_fog_dist" );
	m_fEffectLevel	= m_pXmlConfig->DftGet<float>( 1.0f,"graph_config","effectlevel" );
	m_fGUIZoom		= m_pXmlConfig->DftGet<float>( 1.0f,"graph_config","uiscale" );
	m_TexHMem		= m_pXmlConfig->DftGet<uint32>( 96 , "graph_config","tex_h_mem" );
	m_TexMMem		= m_pXmlConfig->DftGet<uint32>( 48 , "graph_config","tex_m_mem" );
	m_MaxShader		= m_pXmlConfig->DftGet<uint32>( uint32(5),"graph_config","max_shader");
	m_iTerrainType	= m_pXmlConfig->DftGet<uint32>( uint32(0),"graph_config","terrain_quality");
	
	//Shader
	m_strShaderConfigpath	= utf8_to_utf16(  m_pXmlConfig->DftGet<string>("shader/"	,"system_config","shader_path") ).c_str();
	m_strScreenshotPath		= utf8_to_utf16(  m_pXmlConfig->DftGet<string>("./"		,"system_config","screenshot") ).c_str();

	size_t numTextures = m_pXmlConfig->AttribGet<size_t>( "system_config","lake_tex","num" );
	SString lakePath = m_pXmlConfig->Get<const char*>( "system_config","lake_tex" );
	char buffer[32] = {0};
	for (index_t i = 0; i < numTextures; ++i)
	{
		_snprintf(buffer, 32, "%s%02u.dds", lakePath.c_str(), i + 1);
		m_lakeTexNames.push_back(buffer);
	}
	//音效
	m_volume["MusicVolume"] =	m_pXmlConfig->DftGet<float>( 0.0f,"audio_config","music" );
	m_volume["EffectVolume"] =	m_pXmlConfig->DftGet<float>( 0.0f,"audio_config","effect" );
	m_volume["AmbientVolume"] = m_pXmlConfig->DftGet<float>( 0.0f,"audio_config","ambient" );
	m_bVoiceSwitch = m_pXmlConfig->DftGet<bool>( 0.0f,"audio_config","voice" );

	return;
}

bool CSceneConfig::LoadCameraConfig()
{
	ipkgstream strmCamConfig( L"", m_strCameraConfigpath.c_str());
	if(!strmCamConfig)
		GenErr("Can't open Camera Config file.");
	new CCameraConfig( strmCamConfig );
	if(strmCamConfig.isopen())
		strmCamConfig.close();
	return true;
}

float CSceneConfig::GetAudioVolume( const char* category )
{
	if( category == NULL || category[0] == 0  )
		return 0.0f;

	SString strCategory = category;
	SMap<SString, float>::iterator iter = m_volume.find(strCategory);
	if( iter != m_volume.end() )
		return iter->second;
	else
		return 0.0f;
}


uint32 CSceneConfig::GetDefaultAntialias() const
{
	return m_pXmlConfig->Get<uint>( "graph_config","antialias" );
}

uint32 CSceneConfig::GetDefaultShadowType() const
{
	return m_pXmlConfig->Get<uint>( "graph_config","shadow" );
}

uint32 CSceneConfig::GetDefaultShaderTile() const
{
	return m_pXmlConfig->Get<uint32>( "graph_config","shader_map" );
}

uint32 CSceneConfig::GetDefaultWaterType() const
{
	return m_pXmlConfig->Get<uint>( "graph_config","water" );
}

bool CSceneConfig::GetDefaultFullScreen() const
{
	return m_pXmlConfig->Get<bool>( "graph_config","fullscreen" );
}

bool CSceneConfig::GetDefaultVSync() const
{
	return m_pXmlConfig->Get<bool>( "graph_config","vsynchronize" );
}

bool CSceneConfig::GetDefaultBloom() const
{
	return m_pXmlConfig->Get<bool>( "graph_config","bloom" );
}

bool CSceneConfig::GetDefaultRefract() const
{
	return m_pXmlConfig->Get<bool>( "graph_config","refract" );
}

float CSceneConfig::GetDefaultMinSpeed() const
{
	return m_pXmlConfig->DftGet( 0.05f , "camera_config","minspeed" );
}

float CSceneConfig::GetDefaultEffectLevel() const
{
	return m_pXmlConfig->Get<float>( "graph_config","effectlevel" );
}

uint32 CSceneConfig::GetDefaultTexQuality() const
{
	return m_pXmlConfig->Get<uint>( "graph_config","texquality" );
}

uint32 CSceneConfig::GetDefaultTexHMem() const
{
	return m_pXmlConfig->DftGet( 96 , "graph_config","tex_h_mem" );
}

uint32 CSceneConfig::GetDefaultTexMMen() const
{
	return m_pXmlConfig->DftGet( 48 , "graph_config","tex_m_mem" );
}

float CSceneConfig::GetDefaultUIScale() const
{
	return m_pXmlConfig->Get<float>( "graph_config","uiscale" );
}

uint32 CSceneConfig::GetDefaultMaxShader() const
{
	return m_pXmlConfig->DftGet<uint32>( 5,"graph_config","max_shader");

}



bool CSceneConfig::GetDefaultFreeCamera() const //--摄像机缓冲----
{
	return m_pXmlConfig->Get<bool>( "camera_config","freecamera" );
}

float CSceneConfig::GetDefaultAudioVolume(const char* category)
{
	return m_pXmlConfig->Get<float>( "audio_config",category );
}

bool CSceneConfig::GetDefaultVoiceSwitch() const
{
	return m_pXmlConfig->Get<bool>( "audio_config", "voice");
}

bool CSceneConfig::GetDefaultCameraShake() const
{
	return m_pXmlConfig->Get<bool>( "graph_config","bcamerashake" );
}
