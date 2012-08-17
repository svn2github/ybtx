#pragma once
#include "TSingleton.h"
#include "CFontCfg.h"

namespace sqr
{
	struct LangNode
	{
		typedef vector<GDIFontCfgNode>	FontList;

		string		szLangName;
		string		szLangFolder;
	};

	class CLangCfg
		: public Singleton<CLangCfg>
	{
	public:
		bool			LoadConfig(const string& cfg_path);
		const char*		GetLanFolder(size_t nLangIndex);
		size_t			GetLanCount();
		const char*		GetLanName(size_t nLangIndex)			const;
		const char*		GetOriLanFolder()		{return m_OrgLang.szLangFolder.c_str();}

		void			UseDefaultConfig();

	private:
		typedef	vector<LangNode> LanNodeList;

		LanNodeList		m_LangList;
		LangNode		m_OrgLang;

		static const string LANG_CONGIG_STR;
		static const string LANG_STR;
		static const string LANG_NAME_STR;
		static const string LANG_FOLDER_STR;
		static const string CONFIG_FILE_NAME;
		static const string DEFAULT_LANG_STR;
		static const string DEFUALT_LANG_ID_STR;
		static const string ORILANG_STR;
	};
}
