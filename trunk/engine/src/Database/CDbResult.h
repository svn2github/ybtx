#pragma once

#include "IDatabase.h"

namespace sqr
{
	class CDbConnection;

	class CDbResult
		:public IDbTextResult
	{
	public:
		CDbResult(MYSQL_RES* pRes,CDbConnection* pConn);

		virtual ~CDbResult();

		size_t GetRowNum();
		size_t GetColNum();
		const char * GetColName(size_t uColIndex) const;

		void Locate(size_t uIndex);
		const char* GetData(size_t uColIndex);
		unsigned GetDataLength(size_t uColIndex);

		size_t GetFieldLength(size_t uColIndex);
		EDbFieldType GetFieldType(size_t uColIndex);

		void Release();

		void SetTag(void* pTag);
		void* GetTag()const;

	private:
		MYSQL_RES* m_pRes;
		MYSQL_ROW m_Row;
		unsigned long * m_Lengths;
		void*	m_pTag;
		CDbConnection*	m_pConn;
		size_t				m_stNumFields;
		MYSQL_FIELD*		m_pFields;

		void Lock()const;
		void Unlock()const;
	};

}
