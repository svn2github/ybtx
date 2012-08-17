#include "stdafx.h"
#include "CLangCfg.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "CXmlConfig.inl"
#include "CGraphicExp.h"
#include "TableFileHelper.h"

const string CLangCfg::LANG_CONGIG_STR			= "LanguageConfig";
const string CLangCfg::LANG_STR					= "Language";
const string CLangCfg::LANG_NAME_STR			= "name";
const string CLangCfg::LANG_FOLDER_STR			= "pkg_lang_res";
const string CLangCfg::CONFIG_FILE_NAME			= "LanguageConfig.xml";
const string CLangCfg::DEFAULT_LANG_STR			= "DefaultLanguage";
const string CLangCfg::DEFUALT_LANG_ID_STR		= "id";
const string CLangCfg::ORILANG_STR				= "OriginalLanguage";

bool CLangCfg::LoadConfig( const string& cfg_path )
{
	SQR_TRY
	{
		ipkgstream file_strm;
		file_strm.open(L"lan", cfg_path.c_str(), true, eRO_File);
		if (!file_strm)
			GenErr(string("Can't open ") + cfg_path);
		
		m_LangList.clear();
		CXmlConfig *pLangXmlCfg = new CXmlConfig( LANG_CONGIG_STR.c_str() , file_strm );
		file_strm.close();
		
		TiXmlNode *pLanguageNode = pLangXmlCfg->Get<TiXmlElement*>(LANG_STR.c_str());
		while( pLanguageNode )
		{
			string mbcs_str;
			LangNode newLangNode;
 			newLangNode.szLangName	  = pLanguageNode->ToElement()->Attribute(LANG_NAME_STR.c_str());
 			newLangNode.szLangFolder  = pLanguageNode->ToElement()->Attribute(LANG_FOLDER_STR.c_str());

			m_LangList.push_back(newLangNode);

			pLanguageNode = pLanguageNode->NextSibling(LANG_STR.c_str());
		}

		SetCurLangPath(m_LangList.front().szLangFolder);
		pLanguageNode = pLangXmlCfg->Get<TiXmlElement*>(ORILANG_STR.c_str());
		if(pLanguageNode)
		{
			//m_OrgLang.szLangName	  = pLanguageNode->ToElement()->Attribute(LANG_NAME_STR.c_str());
			m_OrgLang.szLangFolder  = pLanguageNode->ToElement()->Attribute(LANG_FOLDER_STR.c_str());
			SetOriLangPath(m_OrgLang.szLangFolder);
		}

		delete pLangXmlCfg;	
		pLangXmlCfg = NULL;
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		return false;
	}
	SQR_TRY_END;

	return true;
}

const char* CLangCfg::GetLanFolder( size_t nLangIndex )
{
	Ast(nLangIndex>=0 && nLangIndex<=m_LangList.size());

	return m_LangList[nLangIndex].szLangFolder.c_str();
}

size_t CLangCfg::GetLanCount()
{
	return m_LangList.size();
}

const char* CLangCfg::GetLanName( size_t nLangIndex ) const
{
	Ast(nLangIndex>=0 && nLangIndex<=m_LangList.size());

	return m_LangList[nLangIndex].szLangName.c_str();
}

void sqr::CLangCfg::UseDefaultConfig()
{
	LangNode newLangNode;
	newLangNode.szLangName	  = "chinese_simple";
	newLangNode.szLangFolder  = "chinese_simple";
}
