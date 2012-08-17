#include "stdafx.h"
#include "CEditorConfig.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#define EDITOR_RES_PATH L"../Tools/Res"
#define DEFAULT_EDITOR_CONFIG_PATH L"/EditorConfig.xml"


CEditorConfig::CEditorConfig()
{
	wchar_t szFull[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH,szFull);
	m_strAppPath	=	szFull;
	m_CfgFileName	=	m_strAppPath + DEFAULT_EDITOR_CONFIG_PATH;
	m_strResPath	=	EDITOR_RES_PATH;
	new CGraphic(eRCT_DX9);
	//CEditorCore::Initialize();
}

CEditorConfig::~CEditorConfig()
{
	//CEditorCore::Destroy();
}


TiXmlNode* CEditorConfig::GetEditorConfig( const string& node_name )
{
	TiXmlNode* pNode = m_CfgXml.FirstChild(node_name);
	if ( pNode == NULL )
		pNode = m_CfgXml.InsertEndChild(TiXmlElement(node_name.c_str()));

	return pNode;
}

bool CEditorConfig::InitEditorConfig( const char* filename )
{
	if( filename != NULL )
		m_CfgFileName = gbk_to_utf16(filename);

	m_CfgXml.Clear();
	if( !m_CfgXml.LoadFile(utf16_to_gbk(m_CfgFileName)) )
	{
		TiXmlDeclaration	XmlDecl;
		XmlDecl.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
		m_CfgXml.InsertEndChild(XmlDecl);
		return false;
	}



	return true;
}

bool CEditorConfig::SaveEditorConfig( const char* filename )
{
	if( filename == NULL )
		return m_CfgXml.SaveFile(utf16_to_gbk(m_CfgFileName));
	else
		return m_CfgXml.SaveFile(filename);
}

TiXmlNode& CEditorConfig::GetNodeChild( TiXmlNode& node, const string& child_name )
{
	return *(node.FirstChild(child_name));
} 
