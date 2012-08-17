#pragma once
#include "CommonDefs.h"
#include "CDataFileMallocObject.h"
#include "TableFileHelper.h"
#include "TBaseTableFile.h"
#include "CDynamicObject.h"

namespace sqr
{
	class CCurLangTxtTableFileImp;
	class CMultiLangTxtTableFileImp;
	class CTableFileValue;

	class COMMON_API CMultiLangTxtTableFile
		: virtual public CDynamicObject
		, public TBaseTableFile<CMultiLangTxtTableFileImp>
	{
		friend class CMultiLangTxtTableFileImp;
		friend class TBaseTableFile<CMultiLangTxtTableFileImp>;
	public:
		typedef TBaseTableFile<CCurLangTxtTableFileImp>		CurLangFile_t;
		typedef TBaseTableFile<CMultiLangTxtTableFileImp>	Parent_t;

		CMultiLangTxtTableFile(void);
		~CMultiLangTxtTableFile(void);

		bool Load( const char* alias, vector<CVariant*>* pArgList );
		bool Load( const wchar_t* szAliasPath, vector<CVariant*>* pArgList );

	protected:
		TBaseTableFile<CCurLangTxtTableFileImp>*			m_pCurLangFile;

	public:
		void Release();

		int32		GetHeight();
		int32		GetWidth();

		ETableFileFieldType GetType( int32 nColumn );
		ETableFileFieldType GetType( const char* szColumnName );

		const char* GetString( int32 nRow, int32 nColumn );
		const char* GetString( int32 nRow, const char* szColumnName );

		int32		GetInteger( int32 nRow, int32 nColumn, int32 defaultvalue );
		int32		GetInteger( int32 nRow, const char* szColumnName, int32 defaultvalue );

		float		GetFloat( int32 nRow, int32 nColumn, float defaultvalue );
		float		GetFloat( int32 nRow, const char* szColumnName, float defaultvalue );

		template<typename T>
		T			GetNumber( int32 nRow, int32 nColumn, T defaultvalue);

		template<typename T>
		T			GetNumber( int32 nRow, const char* szColumnName, T defaultvalue );

		CVariant*	GetValue(CTableFileValue* pValue, int32 nRow, int32 nColumn);
		CVariant*	GetValue(CTableFileValue* pValue, int32 nRow, const char* szColumnName);

	};
};

