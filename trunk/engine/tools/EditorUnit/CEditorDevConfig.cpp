#include "StdAfx.h"
#include "CEditorDevConfig.h"
#include "CXmlConfig.h"
#include "CXmlConfig.inl"
#include "StringHelper.h"

namespace sqr
{

CEditorDevConfig::CEditorDevConfig()
{
	m_pXmlConfig = NULL;
	DefaultDevConfig();
}

CEditorDevConfig::~CEditorDevConfig(void)
{
	delete m_pXmlConfig;
	m_pXmlConfig = NULL;
}

void CEditorDevConfig::DefaultDevConfig()
{
	m_coreDumpIsEnabled = false;
	m_errMsgIsShow	    = true;
}

void CEditorDevConfig::LoadDevConfig(TiXmlNode* Node)
{
	m_pXmlConfig = new CXmlConfig(Node->ToElement());
	if( m_pXmlConfig == NULL )
		return;

	m_coreDumpIsEnabled	= m_pXmlConfig->DftGet<bool>( false,"core_dump" );
	m_errMsgIsShow		= m_pXmlConfig->DftGet<bool>( false,"err_msg_show" );
}

void CEditorDevConfig::SaveDevConfig(TiXmlNode* Node)
{
 	string value;
 	TiXmlText tmpTex(value);


 	// coredump
 	TiXmlNode* coredumpNode = Node->InsertEndChild( TiXmlElement("dev_config") );

	TiXmlNode* childNode = coredumpNode->InsertEndChild( TiXmlElement("core_dump") );
 	value = format("%d", m_coreDumpIsEnabled);
	tmpTex = value.c_str();
 	childNode->ToElement()->InsertEndChild(tmpTex);

	tmpTex.Clear();

	childNode = coredumpNode->InsertEndChild( TiXmlElement("err_msg_show") );
	value = format("%d", m_errMsgIsShow);
	tmpTex = value.c_str();
	childNode->ToElement()->InsertEndChild(tmpTex);
}

}