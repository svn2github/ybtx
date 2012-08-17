#include "stdafx.h"
#include "TableFileHelper.h"
#include "CPkgFile.h"
#include "CodeCvs.h"
#include "StringHelper.h"

namespace sqr 
{
	string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value,string::size_type pos)   
	{   
		for( /*pos*/;   pos!=string::npos;   pos+=new_value.length())  
		{   
			if( (pos=str.find(old_value,pos)) != string::npos )   
				str.replace(pos,old_value.length(),new_value);   
			else
				break;   
		}   
		return   str;   
	}


	string g_strOriLangPath("");
	string g_strCurLangPath("");
	bool g_bCurLangIsOriLang = false;

	ETableFileType GetTableFileType(const char* alias, const char* szFileName)
	{
		string str(szFileName);
		str.append(".xml");

		wstring sAliasPath = alias ? utf8_to_utf16(alias) : L"";

		CPkgFile TabFile;
		int nRet = TabFile.Open(sAliasPath.c_str(), str.c_str());
		if (nRet != eFE_SUCCESS)
		{
			if(sAliasPath == L"lan")
			{
				if(!g_bCurLangIsOriLang)
				{
					replace_all_distinct(str, g_strCurLangPath, g_strOriLangPath, 0);
					int nRet = TabFile.Open(sAliasPath.c_str(), str.c_str());
					if (nRet != eFE_SUCCESS)
					{
						return eTFT_None;
					}
				}
				else
				{
					return eTFT_None;
				}
			}
			else
			{
				return eTFT_None;
			}
		}

		//对于配置文件来说，他前面的一个TableType字段就表明了table file的类型
		char buffer[50];
		TabFile.read(buffer, sizeof(buffer));
		buffer[49] = '\0';
		str = tolower(buffer);
		size_t stPos = str.find("</tabletype>");		
		if (stPos != string::npos)
		{
			char cType = str[stPos - 1];
			if (cType == 's')
			{
				return eTFT_Txt;
			}
			else if(cType == 'm')
			{
				return eTFT_Row;
			}
		}

		return eTFT_TypeError;
	}

	void SetOriLangPath(const string& strOriLangPath)
	{
		g_strOriLangPath = strOriLangPath;
		g_bCurLangIsOriLang = bool(g_strOriLangPath == g_strCurLangPath);
	}

	const string& GetOriLangPath()
	{
		return g_strOriLangPath;
	}

	void SetCurLangPath(const string& strCurLangPath)
	{
		g_strCurLangPath = strCurLangPath;
		g_bCurLangIsOriLang = bool(g_strOriLangPath == g_strCurLangPath);
	}

	const string& GetCurLangPath()
	{
		return g_strCurLangPath;
	}

}

