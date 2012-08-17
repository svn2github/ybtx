#pragma once 
#include "ModuleDefs.h"
#include "ErrLogDefs.h"

namespace sqr
{
	class CError;
	class CLogOwner;

	MODULE_API void SetLogCallBackFunc(
		void (*pFunc)(const char* szError, CLogOwner* pOwner));
	MODULE_API void CallLogCallBackFunc(const char* szError, CLogOwner* pOwner);
	MODULE_API void CallLogCallBackFunc(CError& exp, CLogOwner* pOwner);

	MODULE_API void LogDumpExp(CError& exp);
	MODULE_API void LogExp(CError& exp, CLogOwner* pOwner = NULL, bool bErrorCallBack = true);
	MODULE_API void LogErr(const string& strErrorType, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 	
	MODULE_API void LogErr(const string& str, const string& strErrorMsg, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 
	MODULE_API void LogErr(const char* strErrorType, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 	
	MODULE_API void LogErr(const char* str, const char* strErrorMsg, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 

	MODULE_API void LogCategoryErr(const string& strCategory, const string& strErrorType, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 	
	MODULE_API void LogCategoryErr(const string& strCategory, const string& strErrorType, const string& strErrorMsg, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 
	MODULE_API void LogCategoryErr(const char* strCategory, const char* strErrorType, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 	
	MODULE_API void LogCategoryErr(const char* strCategory, const char* strErrorType, const char* strErrorMsg, CLogOwner* pOwner = NULL, bool bErrorCallBack = true); 

	MODULE_API void SetErrorCallBackFunc( void (*pFunc)(CError& exp) );

	MODULE_API void SetErrLogEnabled(bool bEnabled);
	MODULE_API bool ErrLogEnabled();
	MODULE_API void SetErrLogFileName(const wchar_t* szFileName);

#ifdef _WIN32
	MODULE_API void RedirectIOToConsole(bool bEnabled);
#endif

	MODULE_API void SetErrLogEncrypt(bool bEncrypt);
	MODULE_API bool ErrLogEncrypted();

	MODULE_API void FetchCurrentRevision(const wchar_t* szFileName);
	MODULE_API void FetchCurrentVersion(const wchar_t* szFileName);
	MODULE_API uint32 GetCurrentRevision();
	MODULE_API const char * GetCurrentVersion();
	MODULE_API uint32 GetRevisionFromLogFile(const char* szFileName);

	MODULE_API void GetCurLogTime(wchar_t* szTime, size_t count);
	MODULE_API void SetLogPath(const wchar_t* szLogPath);
	MODULE_API const wchar_t* GetLogPath();

#ifndef _WIN32
	MODULE_API const char* GetLogPathU8();
#endif

	MODULE_API void SetProcessGUID(unsigned char guid[16]);
	MODULE_API unsigned char* GetProcessGUID();
	MODULE_API const char* GetProcessStringGUID();

	MODULE_API void SetProcessName(const char* szErrLog);
	MODULE_API const char* GetProcessName();

	MODULE_API void SetProcessInitTime();
	MODULE_API void GetProcessInitTime(struct tm* pInitTime);

	MODULE_API void CfgLogErr(const std::string& sErrorType);
	MODULE_API void CfgLogErr(const std::string& sErrorType, const std::string& sErrorMsg);
	MODULE_API void CfgLogErr(const char* sErrorType);
	MODULE_API void CfgLogErr(const char* sErrorType, const char* sErrorMsg);

	MODULE_API void ErrLogMsg(const char* key, const char* value);

	//设置重置log的时间间隔，每超过这个间隔时间，我们会重新生成一个新的log文件用来记录log
	MODULE_API void SetLogFileInterval(uint32 uInterval);
	MODULE_API uint32 GetLogFileInterval();

	//因为现在log会隔断时间就产生新的，而一些基本头信息需要每个log都有
	MODULE_API void ErrLogHeadMsg(const char* key, const char* value);

	MODULE_API void SetCompressFun(CompressFun pFun);	

	MODULE_API void SetDeCompressFun(DeCompressFun pFun);

	MODULE_API bool Compress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen);
	MODULE_API bool DeCompress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen);

	MODULE_API void Encode(char* buf, uint32 uLen);
	MODULE_API void Decode(char* buf, uint32 uLen);

	MODULE_API uint32 GetErrIndexCode(CError& exp);

}
