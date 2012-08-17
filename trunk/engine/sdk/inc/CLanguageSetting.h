#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class CLanguageSetting : public virtual CDynamicObject
	{
	public:
		CLanguageSetting();
		~CLanguageSetting();

		static CLanguageSetting* Inst();
		void					 ChangeLanguage(size_t nLangIndex);
		void					 SetCurLangID(size_t nLangIndex);
		int32					 GetLanguageCount();
		int32					 GetCurLanIndex();
		const char*				 GetLanguageName(int32 nLangIndex);

	private:
		CLanguageSetting(const CLanguageSetting&);
		CLanguageSetting& operator = (const CLanguageSetting&);

		void					saveCfg(size_t nLangIndex);
		void					clearLangInfo();

		string	m_szTmp;
		int32	m_CurLangIndex;
	};
}