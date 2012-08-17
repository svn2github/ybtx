#pragma once
#include "CTxtTableFileImp.h"


namespace sqr
{
	class CCurLangTxtTableFileImp;

	class CMultiLangTxtTableFileImp
		: public TBaseTableFileImp<CMultiLangTxtTableFileImp>
	{
	public:
		typedef TBaseTableFileImp<CMultiLangTxtTableFileImp> Parent_t;
		friend class TBaseTableFileImp<CMultiLangTxtTableFileImp>;
		friend class CMultiLangTxtTableFile;

		CMultiLangTxtTableFileImp();
		~CMultiLangTxtTableFileImp();
		bool						GetUseMultiLang();
		void						SetUseMultiLang(bool bUseMultiLang);
		void						RedirectOffset(CCurLangTxtTableFileImp* pCurLangImp);

		const char* GetString(int32 nRow, int32 nCol);
		const char* GetString(int32 nRow, const char* szColName);
		const char* GetString(int32 nOffsetIndex);

		ETableFileFieldType GetType(int32 nCol);
		ETableFileFieldType GetType(string szColName);
		void Map2Vec();

	protected:
		bool						m_bUseMultiLang;
		vector<bool>				m_vecOffsetToCurLang;
		CCurLangTxtTableFileImp*	m_pCurLangImp;

	private:
		bool LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName);
		bool LoadDerived();

	private:
			struct str_less : public binary_function<string, string, bool>
		{
			bool operator()(string szLeft, string szRight) const 
			{
				return (strcmp(szLeft.c_str(), szRight.c_str()) < 0) ? true : false;
			}
		};
		typedef vector<ETableFileFieldType, CDataFileAllocator<ETableFileFieldType> >	VecType_t;
		typedef map<string, ETableFileFieldType,str_less, CDataFileAllocator<pair<string, ETableFileFieldType> > >		MapName2Type;
		VecType_t					m_vecType;
		MapName2Type				m_mName2Type;
		bool						m_bRedirectOffsetFinished;
	};
};

