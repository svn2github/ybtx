#pragma once

#include "BaseTypes.h"
#include "Database.h"
#include "CDynamicObject.h"
#include "CDbMallocObject.h"

namespace sqr
{
	/*
	mysql 获取结果的时候可选择是否缓冲整个结果集，我根据下列分析得出自己的选择结果
	1、查询执行之后，整个结果集肯定被放在mysql内部，这是根据mysql文档:
	(you must retrieve all the rows even if you determine in mid-retrieval
	that you've found the information you were looking for.)
	2、结果集在调用StoreResult或者UseResult+全部FetchRow的前后，要么放在mysql内部，要么放在外部，
	内存没有什么节省,
	3、使用UseResult结果集必须要Fetch全部Row之后才能知道RowCount,这很不方便

	因此无论是Execute还是Stmt_Execute,都选用StoreResult的方案


	另外，提供参数和获取结果应该使用Variant方式还是使用提供多种不同类型的函数也的方式也经过了一番仔细的考虑。
	最后发现两种方式实现方法都差不多，Variant方法比较易用，但是
	功能较弱：
	例如DateTime和Date以及Time这三种类型都只能用operator =(time_t&)来实现，无法区分
	容易让使用者产生疑问：
	例如string和binary都只能用operator=(const char*)或者operator=(const string&);
	因此最后选择用多类型的函数来实现


	目前的解决方案做到了类型id到静态类型的邦定，一定程度提高了易用性，我认为是最好的方法了


	凡是调用mysql api的地方都用了互斥锁来保护。


	2006.1.23日，今日发现对mysql statement的封装在设计上可以进化的地方，bind不应该在每次execute的时候进行，应该bind一次，执行多次，这个设计缺陷令我极其不爽，有时间一定要改
	2006.2.23日，Statement接口的修改已完成，这里补充说明一下
	*/

	enum Isolation_Level 
	{
		READ_UNCOMMITTED = 1,
		READ_COMMITTED,
		REPEATABLE_READ,
		SERIALIZABLE,
	};

	enum EDbFieldType
	{
		eDFT_Int8 = 0,
		eDFT_Uint8,
		eDFT_Int16,
		eDFT_Uint16,
		eDFT_Int32,
		eDFT_Uint32,
		eDFT_Int64,
		eDFT_Uint64,
		eDFT_Float,
		eDFT_Double,
		eDFT_String,        //字符串char/var char
		eDFT_Blob,          //text/blob
	};

	class IDbTextResult
		: public CDbMallocObject
	{
	public:
		virtual size_t GetRowNum() = 0;
		virtual size_t GetColNum() = 0;
		virtual const char * GetColName(size_t uColIndex) const = 0;

		virtual void Locate(size_t uRowIndex) = 0;
		//获取当前行某列的数据，数据以字符串的形式返回
		virtual const char* GetData(size_t uColIndex) = 0;
		virtual unsigned GetDataLength(size_t uColIndex) = 0;

		virtual size_t GetFieldLength(size_t uColIndex) = 0;
		virtual EDbFieldType GetFieldType(size_t uColIndex) = 0;

		virtual void Release()=0;
		virtual void SetTag(void* pTag)=0;
		virtual void* GetTag()const=0;
		virtual ~IDbTextResult(){}
	};

	class IDbConnection;

	class IDbStatement;

	class IDbParamBinder
		: public CDbMallocObject
	{
	public:
		//Text与Binary的区别在于Binary不受collate的影响
		virtual void SetParamNull(const bool* pIsNull,size_t uIndex)=0;

		virtual void SetParamInt8(const int8* pBuffer,size_t uIndex)=0;
		virtual void SetParamUint8(const uint8* pBuffer,size_t uIndex)=0;

		virtual void SetParamInt16(const int16* pBuffer,size_t uIndex)=0;
		virtual void SetParamUint16(const uint16* pBuffer,size_t uIndex)=0;

		virtual void SetParamInt32(const int32* pBuffer,size_t uIndex)=0;
		virtual void SetParamUint32(const uint32* pBuffer,size_t uIndex)=0;

		virtual void SetParamInt64(const int64* pBuffer,size_t uIndex)=0;
		virtual void SetParamUint64(const uint64* pBuffer,size_t uIndex)=0;

		virtual void SetParamFloat(const float* pBuffer,size_t uIndex)=0;
		virtual void SetParamDouble(const double* pBuffer,size_t uIndex)=0;

		virtual void SetParamText(const void* pBuffer,size_t uMaxSize,size_t* pActualSize,size_t uIndex)=0;
		virtual void SetParamBinary(const void* pBuffer,size_t uMaxSize,size_t* pActualSize,size_t uIndex)=0;

		virtual IDbStatement* GetStatement()const=0;

		virtual void Bind()=0;

		virtual void Release()=0;

		virtual ~IDbParamBinder(){}
	};


	class IDbResultBinder
		: public CDbMallocObject
	{
	public:
		virtual void SetResultInt8(int8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultUint8(uint8* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual void SetResultInt16(int16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultUint16(uint16* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual void SetResultInt32(int32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultUint32(uint32* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual void SetResultInt64(int64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultUint64(uint64* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual void SetResultFloat(float* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultDouble(double* pBuffer,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual void SetResultText(void* pBuffer,size_t uBufferSize,size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex)=0;
		virtual void SetResultBinary(void* pBuffer,size_t uBufferSize,size_t* uDataSize,bool* pIsNull,bool* pError,size_t uColIndex)=0;

		virtual IDbStatement* GetStatement()const=0;

		virtual void Bind()=0;

		virtual void Release()=0;
		virtual void Reset() = 0;

		virtual ~IDbResultBinder(){}
	};


	class IDbStatement
		:public virtual CDynamicObject
		,public CDbMallocObject
	{
	public:
		virtual IDbParamBinder* CreateParamBinder()const=0;
		virtual IDbResultBinder* CreateResultBinder()const=0;

		virtual void FetchResultRow(size_t uRowIndex)=0;

		virtual size_t GetParamNum()const=0;
		virtual size_t GetResultColNum()const=0;
		virtual size_t GetResultRowNum()const=0;
		virtual const char* GetResultColName(size_t uColIndex) const = 0;

		virtual IDbConnection* GetConnection()const=0;

		virtual size_t GetFieldLength(size_t uColIndex) = 0;
		virtual EDbFieldType GetFieldType(size_t uColIndex) = 0;

		virtual void Execute()=0;
		virtual void ClearResult()=0;
		virtual void Release()=0;
		
		virtual void SetTag(void* pTag)=0;
		virtual void* GetTag()const=0;

		virtual string GetSql() const = 0;

		virtual ~IDbStatement(){}
	};

	class CError;
	class IDbConnection
		:public virtual CDynamicObject
		,public CDbMallocObject
	{
	public:
		virtual IDbTextResult* Execute(const char* szSqlBuffer,size_t uSize)=0;
		virtual IDbTextResult* Execute(const char* szSqlBuffer)=0;

		virtual IDbStatement* CreateStatement(const char* szSqlBuffer,size_t uSqlLength)=0;
		virtual IDbStatement* CreateStatement(const char* szSqlBuffer)=0;

		virtual size_t EscapeString(char* szOut,const char* szIn,size_t uInSize)=0;
		virtual uint64 LastInsertId()const=0;
		virtual uint64 LastAffectedRowNum()const=0;
		virtual bool Ping()=0;

		virtual void SetTag(void* pTag)=0;
		virtual void* GetTag()const=0;

		virtual void Release()=0;

		virtual void SetAutoCommit(bool bEnabled) = 0;
		virtual void SetIsolationLevel(Isolation_Level eIsolationLevel) = 0;
		virtual void SetCharacterSet(const char* szCharSet) = 0;
		virtual void CommitTrans() = 0;
		virtual void RollbackTrans() = 0;
		virtual void SetSavepoint(const char* szName) = 0;
		virtual void RollbackTransTo(const char* szName) = 0;
		virtual ~IDbConnection(){}
	};


	class IDatabase : public CDbMallocObject
	{
	public:
		virtual IDbConnection* CreateConnection(const char* szHost,uint16 uPort,
			const char* szUser,const char* szPassword,
			const char* szDatabase)=0;

		virtual IDbConnection* CreateConnection(const char* szPipe,//under unix is this unixsocket
			const char* szUser,const char* szPassword,
			const char* szDatabase)=0;

		virtual void BeginDbThread()=0;
		virtual void EndDbThread()=0;

		virtual void Release()=0;
		virtual ~IDatabase(){}
	};

	DATABASE_API IDatabase* GetDatabase();
}
