#pragma once

#include "IDatabase.h"
#include "CLock.h"

namespace sqr
{

	class CDatabase
		:public IDatabase
	{
	public:

		IDbConnection* CreateConnection(const char* szHost,uint16 uPort,
			const char* szUser,const char* szPassword,
			const char* szDatabase);

		IDbConnection* CreateConnection(const char* szPipe,
			const char* szUser,const char* szPassword,
			const char* szDatabase);

		void Release();

		CDatabase();

		void BeginDbThread();
		void EndDbThread();

		void AddRef();

		static CDatabase*& Inst();
	private:
		friend class CDbConnection;

		~CDatabase();

		uint32	m_uCounter;

		mutable CLock	m_Lock;
	};

}
