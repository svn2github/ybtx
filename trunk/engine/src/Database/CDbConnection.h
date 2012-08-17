#pragma once

#include "IDatabase.h"
#include "CLock.h"

namespace sqr
{
	class CDatabase;
	class CDbResult;
	class CDbStatement;
	class CDbParamBinder;
	class CDbResultBinder;

	class CDbConnection
		:public IDbConnection
	{
	public:
		MYSQL* m_pMySql;

		CDbConnection(CDatabase* pDatabase,const char* szHost,uint16 uPort
			,const char* szUser,const char* szPassword,const char* szDatabase);
		CDbConnection(CDatabase* pDatabase,const char* szPipe
			,const char* szUser,const char* szPassword,const char* szDatabase);

		virtual ~CDbConnection();

		uint64 LastInsertId()const;

		uint64 LastAffectedRowNum()const;

		size_t EscapeString(char* szOut,const char* szIn,size_t uInSize);

		bool Ping();

		IDbTextResult* Execute(const char* szSqlBuffer);
		IDbTextResult* Execute(const char* szSqlBuffer,size_t uSize);
		IDbStatement* CreateStatement(const char* szSqlBuffer,size_t uSize);
		IDbStatement* CreateStatement(const char* szSqlBuffer);

		void SetTag(void* pTag);
		void* GetTag()const;

		void Release();

		void SetAutoCommit(bool bEnabled);
		void SetIsolationLevel(Isolation_Level eIsolationLevel);
		void SetCharacterSet(const char* szCharSet);

		void CommitTrans();
		void RollbackTrans();

		void SetSavepoint(const char* szName);
		void RollbackTransTo(const char* szName);
	private:
		CDatabase*	m_pDatabase;
		void*		m_pTag;

		CDbResult* GetResult(int nQueryReturnCode);

		friend class CDbStatement;
		friend class CDbResult;
		friend class CDbParamBinder;
		friend class CDbResultBinder;

		mutable CLock	m_Lock;
	};
}
