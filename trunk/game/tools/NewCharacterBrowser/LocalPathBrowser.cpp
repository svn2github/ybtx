#include "StdAfx.h"
#include "LocalPathBrowser.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include "CEditorConfig.h"

const string st_szCtrlType = "PathBrowser";

CLocalPathBrowser::CLocalPathBrowser(void)
{
}

CLocalPathBrowser::~CLocalPathBrowser()
{

}

void CLocalPathBrowser::SetResPkgFindPath()
{
	char* aliasArray[] = {"cfg","gui","res","lan","shd"};
	int num = sizeof(aliasArray)/sizeof(char*);

	list<string> lstPath;

	//载入配置文件
	CEditorConfig::GetInst()->InitEditorConfig();

	//加入PathBrowser里面设置的路径
	TiXmlNode* pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);
	for( TiXmlNode* pNode = pConfig_xml->FirstChild("Path"); pNode; pNode = pNode->NextSibling("Path") )
	{
		TiXmlElement* pElem  = pNode->ToElement();
		if(pElem && pElem->GetText())
		{
			string strText = pElem->GetText();
			lstPath.push_back(strText);
		}
	}

	//lstPath.push_back("F:/company/ybtx/artist/resbin");
	for ( list<string>::iterator iter = lstPath.begin(); iter != lstPath.end(); ++iter )
	{
		for (int i=0;i<num;++i)
		{
			string path = *iter;
			string pathLan = *iter + "/lang";
			string pathRes = *iter + "/res";
			string strTemp = aliasArray[i];
			wstring wstrTemp = gbk_to_utf16(strTemp);
			CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(path).c_str());
			CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(pathLan).c_str());
			CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(pathRes).c_str());
		}
	}
}