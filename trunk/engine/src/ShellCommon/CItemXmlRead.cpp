#include "stdafx.h"
#include "CItemXmlRead.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "CBaseScriptApp.h"
#include "CXmlConfig.inl"

CItemTableXml::CItemTableXml()
{
	uint32 i=0;
	string setting_path = utf8_to_gbk(CBaseScriptApp::Inst()->GetCfgFilePath(""));
	string ItemBigIdFile = setting_path+"/item/ItemTable.xml";
	ipkgstream ifXml(L"cfg", ItemBigIdFile.c_str());
	if (!ifXml)
	{
		GenErr("ItemTable.xml打开失败，文件可能不存在或已损坏", "没有对应的xml文件");	
	}
	CXmlConfig XmlCfg;
	XmlCfg.LoadConfig("root", ifXml);
	ifXml.close();
	TiXmlNode *FileTeamNode = XmlCfg.GetRootNode()->FirstChild("File_Team");
	for(TiXmlNode * pNode=FileTeamNode->FirstChild("FileName");pNode;pNode=pNode->NextSibling("FileName"))
	{
		m_vAppName.push_back(pNode->ToElement()->Attribute("AppName"));
		m_vLanName.push_back(pNode->ToElement()->Attribute("LanName"));
		m_vBigId.push_back(atoi(pNode->ToElement()->Attribute("Id")));
		i++;
	}
	m_Number=i;
}
CItemTableXml::~CItemTableXml()
{
}

