#pragma once
#include "IDatabase.h"
#include "CDbMallocAllocator.h"

namespace sqr
{

	class CDbConnection;

	class CDbStatement:
		public IDbStatement
	{
	public:
		CDbStatement(CDbConnection* pConn,const char* szSqlBuffer,size_t uSize);
		~CDbStatement(void);


		IDbParamBinder* CreateParamBinder()const;
		IDbResultBinder* CreateResultBinder()const;

		IDbConnection* GetConnection()const;
		void FetchResultRow(size_t uRowIndex);

		size_t GetParamNum()const;
		size_t GetResultColNum()const;
		size_t GetResultRowNum()const;
		
		const char* GetResultColName(size_t uColIndex) const;

		size_t GetFieldLength(size_t uColIndex);
		EDbFieldType GetFieldType(size_t uColIndex);

		void Execute();
		void Release();

		void ClearResult();

		void SetTag(void* pTag);
		void* GetTag()const;

		string GetSql() const;

		CDbConnection*		m_pConn;
		MYSQL_STMT*			m_pStmt;
		MYSQL_RES*			m_pResMetaData;
		void*				m_pTag;
		size_t				m_stNumFields;
		size_t				m_stParamCount;
		MYSQL_FIELD*		m_pFields;

		typedef basic_string<char, char_traits<char>, CDbMallocAllocator<char> >	String_t;
		String_t				m_sSqlBuffer;
			
	private:
		void Lock()const;
		void Unlock()const;
	};

}
