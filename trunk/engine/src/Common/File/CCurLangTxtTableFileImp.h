#pragma once
#include "TBaseTableFileImp.h"
#include "TableFileHelper.h"

namespace sqr
{
	class CCurLangTxtTableFileImp : public TBaseTableFileImp<CCurLangTxtTableFileImp>
	{
	public:
		typedef TBaseTableFileImp<CCurLangTxtTableFileImp> Parent_t;
		friend class TBaseTableFileImp<CCurLangTxtTableFileImp>;

		CCurLangTxtTableFileImp();
		~CCurLangTxtTableFileImp();

		void					BuildKeyMap();
		int32					GetRowNo(const char* szKeyName);
		int32					GetColNo(const char* szTitleName);
		void					ClearMap();

	protected:
		typedef MapName2Col_t	MapKey2Row_t;
		MapKey2Row_t			m_mapKeyToRowNo;
		bool					m_bBuildKeyMapFinished;
		
	};
};

