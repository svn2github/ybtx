#include "StdAfx.h"
#include "PathManager/CPathManager.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include "CEditorConfig.h"
#include "CPathMgr.h"
#include "StringHelper.h"

#define DEFAULT_EDITOR_CONFIG_PATH "\\EditorConfig.xml"

string CPathSetImp::st_szCtrlType = "PathBrowser";
DEF_SINGLE_UI_IMP(CPathSetImp);
CPathSetImp::CPathSetImp(void)
{
	Clear();
	TiXmlNode* pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);
	for( TiXmlNode* pNode = pConfig_xml->FirstChild("Path"); pNode; pNode = pNode->NextSibling("Path") )
	{
		TiXmlElement* pElem  = pNode->ToElement();
		if(pElem && pElem->GetText())
		{
			string strText = pElem->GetText();
			AddPath(strText);
		}
	}
}

CPathSetImp::~CPathSetImp(void)
{
}

bool CPathSetImp::GetResFileName(string& d_str,string& r_str)
{
	backslash_to_slash(r_str); //完全路径
	string tmep = "temp";
	for (std::string::iterator i = tmep.begin(); i != tmep.end(); i++)
	{
		if (*i >= 'A' && *i <= 'Z')
			*i = (*i) + ('a' - 'A');
	}
	
	tolower(r_str);
	PathList::iterator it=m_ConfigPath.begin();
	while( it!=m_ConfigPath.end())
	{
		string tmp = (*it);
		backslash_to_slash(tmp);
		if(tmp[tmp.length()-1]!='/')
			tmp.append("/");
		tolower(tmp);
		size_t pos = r_str.find(tmp);
		if(pos != -1)
		{
			r_str = r_str.substr(tmp.length(), r_str.length()-tmp.length() ); //字符串截断
			if( r_str[0] == '/')
				r_str = r_str.substr(1,r_str.length() - 1);
			d_str = tmp;
			return true;
		}
		it++;
	}
	return false;
}

bool CPathSetImp::GetFullFileName(string r_str,string& d_str)
{
	typedef CPkgFile::SearchPathList SearchPathList;
	SearchPathList listSearchPath = CPkgFile::GetSearchList(L"res");
	
	for ( SearchPathList::iterator it = listSearchPath.begin(); it != listSearchPath.end(); ++it )
	{
		wstring path = (*it) + gbk_to_utf16(r_str);
		FILE* fp = _wfopen(path.c_str(), L"rb");
		if ( fp != 0 )
		{	
			fclose(fp);
			d_str=utf16_to_gbk(path);
			return true;
		}
	}
	return false;
}

bool CPathSetImp::SaveConfigPath()
{
	//将设置好的路径,保存到配置文件中
	TiXmlNode* pConfig_xml = CEditorConfig::GetInst()->GetEditorConfig(st_szCtrlType);
	if(!pConfig_xml)
		return false;
	pConfig_xml->Clear();
	PathList::iterator it=m_ConfigPath.begin();
	while(it!=m_ConfigPath.end())
	{
		TiXmlElement PathNode("Path");
		TiXmlText PathText( (*it).c_str() );
		PathNode.InsertEndChild(PathText);
		pConfig_xml->InsertEndChild(PathNode);
		it++;
	}
	CEditorConfig::GetInst()->SaveEditorConfig();
	return true;
}

bool CPathSetImp::HasSetConfigPath()
{
	return (m_ConfigPath.size()>0);
}

unsigned int CPathSetImp::Count()
{
	return m_ConfigPath.size();
}

string CPathSetImp::GetPath(unsigned int index)
{
	string result="";
	if(index<m_ConfigPath.size())
		return m_ConfigPath[index];
	return result;
}

void CPathSetImp::AddPath(const string& work_dir)
{
	for( size_t i = 0; i < m_ConfigPath.size(); ++i )
	{
		if( m_ConfigPath[i] == work_dir )
			return;
	}
	m_ConfigPath.push_back(work_dir);
	CPkgFile::AddLoadPath(0, gbk_to_utf16(work_dir).c_str());
}

void sqr_tools::CPathSetImp::Clear( void )
{
	m_ConfigPath.clear();
	CPkgFile::ClearSearchPath();
	CPkgFile::AddLoadPath(NULL, gbk_to_utf16(CEditorConfig::GetInst()->GetEditorAppPath()).c_str());
}