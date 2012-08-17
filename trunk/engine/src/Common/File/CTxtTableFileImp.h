#pragma once
#include "TBaseTableFileImp.h"
#include "TableFileHelper.h"

namespace sqr
{
	class CTxtTableFileImp : public TBaseTableFileImp<CTxtTableFileImp>
	{
	public:
		typedef TBaseTableFileImp<CTxtTableFileImp> Parent_t;
		friend class TBaseTableFileImp<CTxtTableFileImp>;

		CTxtTableFileImp();
		~CTxtTableFileImp();

		ETableFileFieldType GetType(int32 nCol);
		ETableFileFieldType GetType(string szColName);
		void Map2Vec();

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
	};
};

