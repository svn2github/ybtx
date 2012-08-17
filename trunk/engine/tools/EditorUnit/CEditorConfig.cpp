#include "stdafx.h"
#include "CEditorConfig.h"
#include "PkgStream.h"
#include "CDemoCoder.h"
#include "CEditDataSources.h"
#include "CEditSceneConfig.h"
#include "CXmlConfig.h"
#include "CPkgFile.h"
#include "CEditorDevConfig.h"
#include "CodeCvs.h"
#include "PathManager/CPathManager.h"

#define DEFAULT_EDITOR_CONFIG_PATH "/EditorConfig.xml"
#define EDITOR_RES_PATH				"../Tools/Res/"

namespace sqr_tools
{
	const wstring PATH_ALIAS_EDITOR = L"";
}

CEditorConfig::CEditorConfig()
{
	char dir[1024];
	::GetCurrentDirectory(1024,dir);
	m_strAppPath	=	dir;
	m_CfgFileName	=	m_strAppPath + DEFAULT_EDITOR_CONFIG_PATH;
	m_strResPath	=	EDITOR_RES_PATH;

	//打开配置文件,如果没有,则创建一个配置文件
	IsOpen=true;
	m_CfgXml.Clear();
	if( !m_CfgXml.LoadFile(m_CfgFileName) )
	{	
		TiXmlDeclaration	XmlDecl;
		XmlDecl.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
		m_CfgXml.InsertEndChild(XmlDecl);
		m_CfgXml.SaveFile(m_CfgFileName);
		IsOpen=false;
	}	
	
	if( !CPathSetImp::HasInst())
		CPathSetImp::Create();

	//创建图形引擎和数据资源
	new CGraphic(eRCT_DX9);
	CDataSources* pSource = CDataSources::GetInst();
	CDataSources::SetNull();
	new CEditDataSources(pSource);
}

CEditorConfig::~CEditorConfig()
{
	CEditorDevConfig::Destroy();
}

TiXmlNode* CEditorConfig::GetEditorConfig( const string& node_name )
{	
	TiXmlNode* pNode = m_CfgXml.FirstChild(node_name);
	if ( pNode == NULL )
		pNode =m_CfgXml.InsertEndChild(TiXmlElement(node_name.c_str()));
	return pNode;
}

bool CEditorConfig::InitEditorConfig( const char* filename )
{
	if ( m_bInitialized )
		return true;

	if( filename != NULL && (strcmp(m_CfgFileName.c_str(),filename)!=0) )
	{
		IsOpen=false; //新的配置文件名称
		m_CfgFileName = filename;
	}
	if(!IsOpen)
	{
		m_CfgXml.Clear();
		m_CfgXml.LoadFile(m_CfgFileName);
	}

	//解析并初始化SceneConfig配置文件
	TiXmlNode* m_SceneConfigNode=m_CfgXml.FirstChild("scene_config");
	if(m_SceneConfigNode)
	{
		CEditSceneConfig* pConfig = new CEditSceneConfig(m_CfgFileName.c_str(),m_SceneConfigNode->ToElement());
	}
	else
	{
		CEditSceneConfig* pConfig = new CEditSceneConfig(m_CfgFileName.c_str());
		pConfig->AddSceneConfig(&m_CfgXml);
		SaveEditorConfig(m_CfgFileName.c_str());
	}

	m_bInitialized = true;

	// dev config coredump errormsg show
	CEditorDevConfig::Create();
	TiXmlNode* devConfigNode = m_CfgXml.FirstChild("dev_config");
	if( devConfigNode )
	{
		CEditorDevConfig::GetInst()->LoadDevConfig(devConfigNode);
	}
	else
	{
		CEditorDevConfig::GetInst()->SaveDevConfig(&m_CfgXml);
		SaveEditorConfig(m_CfgFileName.c_str());
	}
	return true;
}

bool CEditorConfig::SaveEditorConfig( const char* filename )
{
	if( filename == NULL )
		return m_CfgXml.SaveFile(m_CfgFileName);
	else
		return m_CfgXml.SaveFile(filename);
}

void CEditorConfig::GetEditorConfigValue(const char* Attri0, const char* Attri1, const char* key, string& strValue)
{
	//根据字段名称 获取结点
	TiXmlNode* pConfigXmlNode = this->GetEditorConfig(Attri0);
	TiXmlElement* pElement = pConfigXmlNode->ToElement();
	TiXmlNode* pNode = NULL;

	//根据字段名称 获取结点
	pNode = pConfigXmlNode->FirstChild(Attri1);
	if(pNode!=NULL)
	{ 
		//结点存在 获取属性值
		pElement=pNode->ToElement();
		if( pElement )
			strValue = pElement->Attribute("Path");
	}
	else
	{
		//结点不存在 增加结点
		string pathFileValue = strValue;
		TiXmlElement newNode(Attri1);
		newNode.SetAttribute("Path",pathFileValue);
		pConfigXmlNode->InsertEndChild(newNode);
		this->SaveEditorConfig();
	}
}

TiXmlNode& CXmlHelper::GetNodeChild( TiXmlNode& node, const string& child_name )
{
	return *(node.FirstChild(child_name));
} 

namespace sqr_tools
{
	LPCTSTR GetRegistKeyStr( LPCTSTR Path, LPCTSTR Name )
	{
		HKEY hKey = NULL;
		DWORD Len = 1024;
		DWORD Type = REG_SZ;
		static BYTE  Buf[1024];
		RegCreateKey( HKEY_LOCAL_MACHINE, Path, &hKey );
		if( ERROR_SUCCESS != RegQueryValueEx( hKey, Name, NULL, &Type, Buf, &Len ) )
			Buf[0] = 0;
		RegCloseKey(hKey);

		return (TCHAR*)Buf;
	}
}
