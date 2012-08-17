#include "StdAfx.h"
#include "CEditSceneConfig.h"
#include "PkgStream.h"
#include "CXmlConfig.h"
#include "CXmlConfig.inl"
#include "CodeCvs.h"
#include "CMath.h"
#include "CCameraConfig.h"


CEditSceneConfig::CEditSceneConfig(const string& ConfigFile)
:CSceneConfig()
{
	DefaultSceneConfig();
}

CEditSceneConfig::CEditSceneConfig(const string& ConfigFile,TiXmlElement* Node)
:CSceneConfig(ConfigFile,Node)
{
}

CEditSceneConfig::~CEditSceneConfig(void)
{
}

CEditSceneConfig* CEditSceneConfig::Inst()
{
	return (CEditSceneConfig*)CSceneConfig::GetInst();
}

void CEditSceneConfig::DefaultSceneConfig()
{
	m_bLockRatio	= false;
	m_bShowFps		= false;
	m_bBloom		= false;
	m_bRefract		= false;
	m_bVSync		= false;
	m_bFullScreen	= false;
	m_iAntiAlias	= 2;
	m_iShadowType	= 1;
	m_iWaterType	= 0;
	m_iColorDepth	= 32;
	m_uTexQuailty	= 4;

	m_fEffectLevel	= 0.9f;
	m_fGUIZoom		= 1.0f;
	m_TexHMem		= 96;
	m_TexMMem		= 48;
	m_MaxShader		= 4;
	m_iTerrainType	= 0;

	m_strShaderConfigpath	= utf8_to_utf16("./shader").c_str();
	m_strScreenshotPath		= utf8_to_utf16(  "./ScreenShoot/" ).c_str();

	size_t numTextures = 30;
	string lakePath = "./tex/lake_";
	char buffer[32] = {0};
	for (index_t i = 0; i < numTextures; ++i)
	{
		_snprintf(buffer, 32, "%s%02u.dds", lakePath.c_str(), i + 1);
		m_lakeTexNames.push_back(buffer);
	}

	m_volume["MusicVolume"] =	0.3f;
	m_volume["EffectVolume"] =	0.8f;
	m_volume["AmbientVolume"] = 0.8f;
	
	m_strCameraConfigpath="CameraConfig.xml";
	
	//LoadCameraConfig();
}

void CEditSceneConfig::AddSceneConfig(TiXmlNode* Node)
{
	string value;
	TiXmlText tmpTex(value);

	//相机
	TiXmlNode* sceneNode =Node->InsertEndChild( TiXmlElement("scene_config") );
	TiXmlNode* cameraNode=sceneNode->InsertEndChild( TiXmlElement("camera_config") );
	TiXmlNode* childNode=cameraNode->InsertEndChild( TiXmlElement("mode_path") );
	value=m_strCameraConfigpath.c_str();
	tmpTex.Clear();
	tmpTex=value;
	childNode->ToElement()->InsertEndChild(tmpTex);
	//图像
	TiXmlNode* graphicNode=sceneNode->InsertEndChild( TiXmlElement("graph_config") );
	childNode=graphicNode->InsertEndChild( TiXmlElement("lockratio") );
	tmpTex="0.0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("definition") );
	tmpTex="1.0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("showfps") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("shader_map") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("bloom") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("refract") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("baking") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("effectlevel") );
	tmpTex="0.900000";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("vsynchronize") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("antialias") );
	tmpTex="2";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("shadow") );
	tmpTex="1";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("water") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("texquality") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("fullscreen") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("uiscale") );
	tmpTex="1.0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("color_depth") );
	tmpTex="32";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("tex_h_mem") );
	tmpTex="96";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("tex_m_mem") );
	tmpTex="48";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("max_shader") );
	tmpTex="4";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=graphicNode->InsertEndChild( TiXmlElement("max_vid_mem") );
	tmpTex="0";
	childNode->ToElement()->InsertEndChild(tmpTex);
	//音频
	TiXmlNode* audioNode=sceneNode->InsertEndChild( TiXmlElement("audio_config") );
	childNode=audioNode->InsertEndChild( TiXmlElement("music") );
	tmpTex="0.3";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=audioNode->InsertEndChild( TiXmlElement("effect") );
	tmpTex="0.8";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=audioNode->InsertEndChild( TiXmlElement("ambient") );
	tmpTex="0.8";
	childNode->ToElement()->InsertEndChild(tmpTex);
	//系统
	TiXmlNode* systemNode=sceneNode->InsertEndChild( TiXmlElement("system_config") );
	childNode=systemNode->InsertEndChild( TiXmlElement("screenshot") );
	tmpTex=utf16_to_utf8(m_strScreenshotPath.c_str());
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=systemNode->InsertEndChild( TiXmlElement("lake_tex") );
	int num=30;
	childNode->ToElement()->SetAttribute("num",num);
	tmpTex="./tex/lake_";
	childNode->ToElement()->InsertEndChild(tmpTex);
	childNode=systemNode->InsertEndChild( TiXmlElement("shader_path") );
	tmpTex=utf16_to_utf8(m_strShaderConfigpath.c_str());
	childNode->ToElement()->InsertEndChild(tmpTex);
}
