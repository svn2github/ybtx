#pragma once
/*---------------------------------------------------------------------------
完成制表符分割文件的读取，制表文件是一个特殊格式的文本文件，将Excel编辑的制表
保存为文本文件即可，制表符文件每行由回车换行符分隔(0x0d 0x0a)，每个单元之间由
制表符（TAB）（0x09）分隔.
//---------------------------------------------------------------------------*/


#include "CommonDefs.h"
#include "CDataFileMallocObject.h"
#include "TableFileHelper.h"
#include "CDynamicObject.h"
#include "TBaseTableFile.h"

namespace sqr
{
	class CTxtTableFileImp;
	class CVariant;
	class CTableFileValue;

	class COMMON_API CTxtTableFile : virtual public CDynamicObject
								   , public TBaseTableFile<CTxtTableFileImp>
	{
	public:
		CTxtTableFile(void);
		~CTxtTableFile(void);
	
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
}

;
