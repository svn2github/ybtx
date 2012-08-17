#include "stdafx.h"
#include "DifInfo.h"
#include "common.h"
#include "CSplitInfo.h"
#include "CXmlConfig.inl"

CDifInfo::CDifInfo(const char* szFileName)
{
	int32 nLen = strlen(szFileName);
	m_szFileName = new char[nLen + 1];
	strcpy(m_szFileName, szFileName);
}

CDifInfo::~CDifInfo(void)
{
}

bool CDifInfo::BeMultiRowTable()
{
	string szXmlFileName = GetXmlFileName();
	WIN32_FIND_DATA FindFileData;
	HANDLE hWnd = FindFirstFile(szXmlFileName.c_str(), &FindFileData);
	if (hWnd != INVALID_HANDLE_VALUE)
	{
		TiXmlDocument * pDoc = new TiXmlDocument();
		bool isLink = false;
		string path;
		string strOldXml = szXmlFileName;
		if(pDoc->LoadFile(strOldXml))
		{
			if(pDoc->RootElement()->FirstChild("link"))
			{
				int pos = strOldXml.find_last_of("/");
				path = strOldXml.substr(0,pos);
				string str = pDoc->RootElement()->FirstChild("link")->ToElement()->GetText();
				int spos=-1;
				while(true)
				{
					string temp = str.substr(0,3);
					if(temp=="../")
					{
						str = str.substr(3,str.size());
						pos = path.find_last_of("/");
						path = path.substr(0,pos);
						continue;
					}
					spos=str.find_first_of("/",0);
					if(spos!=-1)
					{
						path=path+"/"+str.substr(0,spos);
						str = str.substr(spos+1,str.size());
						continue;
					}
					path=path+"/"+str;
					break;
				}
				szXmlFileName = path;
				isLink = true;
			}
			
		}
		delete pDoc;
		pDoc = NULL;
		
/////////////////////////////
	CXmlConfig XmlCfg;
	XmlCfg.LoadConfig("config", szXmlFileName.c_str());

	TiXmlNode* pNode = XmlCfg.GetRootNode();
	const char* szType = pNode->FirstChild("TableType")->ToElement()->GetText();
	char cType = szType[0];
	if (cType == 'M' || cType == 'm')
	{
		FindClose(hWnd);
		return true;
	}
	else 
		return false;
////////////////////////////
	}
	else
	{
		return false;
	}
}

string CDifInfo::GetXmlFileName()
{
	string strXmlFileName(m_szFileName);
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strCppCfgExtend = CSplitInfo::GetInst()->GetCppCfgExtend();
	if (strstr(m_szFileName, strLuaCfgExtend.c_str()))
	{
		replace_all_distinct(strXmlFileName, strLuaCfgExtend, ".xml", 0);
	}
	else if (strstr(m_szFileName, strCppCfgExtend.c_str()))
	{
		replace_all_distinct(strXmlFileName, strCppCfgExtend, ".xml", 0);
	}
	return strXmlFileName;
}
