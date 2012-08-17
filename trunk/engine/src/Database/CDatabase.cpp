#include "stdafx.h"
#include "CDatabase.h"
#include "CDbConnection.h"
#include "mysql/mysql.h"
#include "CTimeCheckPoint.h"
#include "ExpHelper.h"
#include "DebugHelper.h"

namespace sqr
{
	IDatabase* GetDatabase()
	{
		if( !CDatabase::Inst() )
		{
			new CDatabase;
		}
		else
		{
			CDatabase::Inst()->AddRef();
		}

		return CDatabase::Inst();
	}

	EDbFieldType GetDbFieldType(MYSQL_FIELD& field)
	{
		/*
		基本数据类型，返回相应的类型
		对于blob和text类型的，我们返回blob，
		对于其他类型，譬如varchar，datetime，我们则返回string
		*/
		enum_field_types type = field.type;
		uint32 UnsignedFlag = field.flags & UNSIGNED_FLAG;

		switch(type)
		{
		case MYSQL_TYPE_TINY:
			return UnsignedFlag ? eDFT_Uint8 : eDFT_Int8;
			break;
		case MYSQL_TYPE_SHORT:
			return UnsignedFlag ? eDFT_Uint16 : eDFT_Int16;
			break;
		case MYSQL_TYPE_LONG:
			return UnsignedFlag ? eDFT_Uint32 : eDFT_Int32;
			break;
		case MYSQL_TYPE_FLOAT:
			return eDFT_Float;
			break;
		case MYSQL_TYPE_DOUBLE:
			return eDFT_Double;
			break;
		case MYSQL_TYPE_NULL:
			return eDFT_Int8;
			break;
		case MYSQL_TYPE_TIMESTAMP:
			return eDFT_Uint32;
			break;
		case MYSQL_TYPE_LONGLONG:
			return UnsignedFlag ? eDFT_Uint64 : eDFT_Int64;
			break;
		case MYSQL_TYPE_INT24:
			return UnsignedFlag ? eDFT_Uint32 : eDFT_Int32;
			break;
		case MYSQL_TYPE_YEAR:
			return eDFT_Int8;
			break;
		case MYSQL_TYPE_NEWDECIMAL:
			return eDFT_Double;
			break;
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			return eDFT_Blob;
			break;
		default:
			return eDFT_String;
			break;
		}
	}
}

CDatabase*& CDatabase::Inst()
{
	static CDatabase*	ls_pInst=NULL;
	return ls_pInst;
}


IDbConnection* CDatabase::CreateConnection(const char* szHost,uint16 uPort,
										   const char* szUser,const char* szPassword,
										   const char* szDatabase)
{
	return new CDbConnection(this,szHost,uPort,szUser,szPassword,szDatabase);
}

IDbConnection* CDatabase::CreateConnection(const char* szPipe,
										   const char* szUser,const char* szPassword,
										   const char* szDatabase)
{
	return new CDbConnection(this,szPipe,szUser,szPassword,szDatabase);
}

void CDatabase::Release()
{
	if( m_uCounter == 0 )
	{
		delete this;
		return;
	}

	--m_uCounter;
}

CDatabase::CDatabase()
{
	m_uCounter = 0;

	mysql_library_init(0,NULL,NULL);
	CDatabase::Inst() = this;
}

CDatabase::~CDatabase()
{
	cout << "清理mysql花费";
	
	CTimeCheckPoint tcp;
	
	mysql_library_end();

	cout << tcp.GetElapse() << "毫秒\n";

	CDatabase::Inst() = NULL;
}

void CDatabase::AddRef()
{
	m_uCounter = 0;
}

#include "ThreadHelper.h"

void CDatabase::BeginDbThread()
{
	mysql_thread_init();
}

void CDatabase::EndDbThread()
{
	mysql_thread_end();
}

