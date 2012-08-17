#pragma once

#include "CDynamicObject.h"

namespace sqr
{
class IDbConnection;
class IDatabase;

class IDbConnectionMgr
			:public virtual CDynamicObject
{
public:
	virtual uint8 GetConnectionNum()const=0;
	virtual IDbConnection* GetConnection(uint8 uIndex)const=0;
	virtual void Release(void)=0;
};

IDbConnectionMgr* GetDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szHost,unsigned short uPort,
                                     const char* szUser,const char* szPassword, const char* szDatabase);

IDbConnectionMgr* GetDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szPipe,
                                     const char* szUser, const char* szPassword, const char* szDatabase);

}

;
