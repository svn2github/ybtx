#pragma once 
#include "CStaticObject.h"
#include "TValueAllocator.h"
#include "VariantTypes.h"
#include "CMemData.h"

namespace sqr
{
	class CValueTableData;
	class CVariant;

	class CValueTable
		: public CStaticObject
		, public CValueMallocObject
	{
		friend class CValueTableData;

	public:
		CValueTable();
		~CValueTable();

		void Release();

		uint32 GetRowNum();
		uint32 GetColNum();

		bool IsNull(uint32 uRow, uint32 uCol);
		bool IsNull(uint32 uRow, const char* szName);

		const char* GetString(uint32 uRow, uint32 uCol) ;
		const char* GetString(uint32 uRow, const char* szName) ;

		template<typename ValueType>ValueType GetNumber(uint32 uRow, uint32 uCol);
		template<typename ValueType>ValueType GetNumber(uint32 uRow, const char* szName);

		EVariantType GetType(uint32 uCol);
		EVariantType GetType(const char* szName);

		CVariant* GetValue(CValueTableData* pData, uint32 uRow, uint32 uCol);
		CVariant* GetValue(CValueTableData* pData, uint32 uRow, const char* szName);

		//-----------------------------------------

		
		void SetFieldType(uint32 uCol, EVariantType eType);
		void SetFieldName(uint32 uCol, const char* szName);
		void SetNull(uint32 uRow, uint32 uCol, bool bNull);
		void SetValue(uint32 uRow, uint32 uCol, const void* pData, size_t stDataSize, bool bNull);
		
		void* GetBuffer(uint32 uRow, uint32 uCol)const;
		uint32 GetBufferSize()const;

		void Init(uint32 uHeight, uint32 uWidth);		

		void ReserveTable(size_t stBufferSize);
		void* ExpandBuffer(uint32 uRow, uint32 uCol, size_t stDataSize);
		
		
		//------------------------------------------

		//获取table整个serial的大小，使得可以为serialize分配合适的空间
		size_t GetSerializeBufferSize();
		//将table的数据serialize到buffer里面
		void Serialize(char* szBuffer);

		//将buffer的数据unserialize回table
		void UnSerialize(const char* szBuffer, size_t stBufferSize);

	private:
		uint32 GetColIndex(const char* szName);

		void SerializeData(char* szBuffer, const void* pData, size_t stDataSize, size_t& stOffset);
		void UnSerializeData(void* pData, const char* szBuffer, size_t stDataSize, size_t& stOffset);

		void SerializeName(char* szBuffer, size_t& stOffset);
		void UnSerializeName(const char* szBuffer, size_t& stOffset);

		void GenIndexError(const char* szErrorType, uint32 uRow, uint32 uCol);
		void GenNullError(uint32 uRow, uint32 uCol);
		void GenTypeError(const char* szType, uint32 uCol);

	protected:
		uint32			m_uRowNumber;
		uint32			m_uColNumber;
		typedef vector<EVariantType, TValueAllocator<EVariantType> >	
			VecFieldType_t;
		VecFieldType_t	m_vecFieldType;

		typedef std::basic_string<char,std::char_traits<char>,TValueAllocator<char> >
			FieldNameStr_t;
		typedef map<FieldNameStr_t, uint32, less<FieldNameStr_t>, 
			TValueAllocator<pair<FieldNameStr_t, uint32> > >	
			MapName2Col_t;
		MapName2Col_t	m_mapFieldName2Col;

		CMemData		m_buffNull;
		CMemData		m_buffOffset;
		CMemData		m_buffData;
	};

}
