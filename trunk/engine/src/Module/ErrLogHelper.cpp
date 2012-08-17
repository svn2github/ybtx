#include "stdafx.h"
#include "ErrLogHelper.h"
#include "CErrLogMgr.h"
#include "ExpHelper.h"
#include "UtfCvs.h"
#include <time.h>
#include "CLogOwner.h"
#include <time.h>
#include <stdlib.h>
#include "TimeHelper.h"
#include "ThreadHelper.h"
#include "GuidHelper.h"
#include "MemoryHelper.h"
#include "CErrLogCompress.h"
#include "CErrLogEncrypt.h"
#include "CError.h"
#include "jenkins.h"

#ifdef _WIN32
#include <iostream>
#include <io.h>
#include <fcntl.h>
#endif

#ifndef _WIN32
#	define _countof(array)	(sizeof(array)/sizeof(array[0]))
#else
#	pragma warning(disable:4996)
#endif

namespace sqr
{
	void WriteAndTellErrLog(CError& exp, CLogOwner* pOwner)
	{
		if(pOwner)
		{
			typedef basic_string<char,std::char_traits<char>,
				TMallocAllocator<char> >	LogString_t;

			LogString_t str;
			pOwner->GetLog(str);
			exp.AppendMsg(str.c_str(), false);
		}

		CErrLogMgr::Inst().WriteErrLog(exp);
	
		CallLogCallBackFunc(exp, pOwner);
	}
	
	//----------------------------------------------

	void NoneErrorCallBack( CError& ) {  }

	typedef void (*ErrorCallBackFunc)(CError& exp);
	static ErrorCallBackFunc gs_ErrorCallBackFunc = &NoneErrorCallBack;

	void SetErrorCallBackFunc( void (*pFunc)(CError& exp) )
	{
		gs_ErrorCallBackFunc = pFunc ? pFunc : NoneErrorCallBack;
	}

	static inline void CallErrorCallBackFunc(CError& exp)
	{
		gs_ErrorCallBackFunc(exp);
	}

	void LogDumpExp(CError& exp)
	{
		CallErrorCallBackFunc(exp);
		CErrLogMgr::Inst().WriteDumpErrLog(exp);
		CallLogCallBackFunc(exp, NULL);
	}

	void LogExp(CError& exp, CLogOwner* pOwner, bool bErrorCallBack)
	{
		if (bErrorCallBack)
		{
			CallErrorCallBackFunc(exp);
		}
		WriteAndTellErrLog(exp, pOwner);
	}


	void LogErr(const string& sType, CLogOwner* pOwner, bool bErrorCallBack)
	{
		LogErr(sType.c_str(), pOwner, bErrorCallBack);
	}

	void LogErr(const string& strType, const string& strMsg, CLogOwner* pOwner, bool bErrorCallBack) 
	{
		LogErr(strType.c_str(), strMsg.c_str(), pOwner, bErrorCallBack);
	}

	void LogErr(const char* sType, CLogOwner* pOwner, bool bErrorCallBack)
	{
		CError error(sType);
		LogExp(error, pOwner, bErrorCallBack);
	}

	void LogErr(const char* strType, const char* strMsg, CLogOwner* pOwner, bool bErrorCallBack) 
	{
		CError error(strType);
		error.AppendMsg(strMsg);
		LogExp(error, pOwner, bErrorCallBack);
	}

	void LogCategoryErr(const string& strCategory, const string& strErrorType, CLogOwner* pOwner, bool bErrorCallBack )
	{
		LogCategoryErr(strCategory.c_str(), strErrorType.c_str(), pOwner, bErrorCallBack);
	}

	void LogCategoryErr(const string& strCategory, const string& strErrorType, const string& strErrorMsg, CLogOwner* pOwner, bool bErrorCallBack)
	{
		LogCategoryErr(strCategory.c_str(), strErrorType.c_str(), strErrorMsg.c_str(), pOwner, bErrorCallBack);
	}

	void LogCategoryErr(const char* strCategory, const char* strErrorType, CLogOwner* pOwner, bool bErrorCallBack )
	{
		CError error(strErrorType);
		error.SetCategory(strCategory);
		LogExp(error, pOwner, bErrorCallBack);
	}

	void LogCategoryErr(const char* strCategory, const char* strErrorType, const char* strErrorMsg, CLogOwner* pOwner, bool bErrorCallBack)
	{
		CError error(strErrorType);
		error.AppendMsg(strErrorMsg);
		error.SetCategory(strCategory);
		LogExp(error, pOwner, bErrorCallBack);
	}

	void CfgLogErr(const std::string& sErrorType)
	{
		return CfgLogErr(sErrorType.c_str());
	}

	void CfgLogErr(const std::string& sErrorType, const std::string& sErrorMsg)
	{
		return CfgLogErr(sErrorType.c_str(), sErrorMsg.c_str());
	}

	void CfgLogErr(const char* sErrorType)
	{
		CError error(sErrorType);
		error.SetCategory(CFG_ERR_MSG);
		LogExp(error, NULL);
	}

	void CfgLogErr(const char* sErrorType, const char* sErrorMsg)
	{
		CError error(sErrorType);
		error.AppendMsg(sErrorMsg);
		error.SetCategory(CFG_ERR_MSG);
		LogExp(error, NULL);
	}

	//---------------------------------------------------------


	class CErrLogCallBackFunc
	{
	public:
		CErrLogCallBackFunc();
		~CErrLogCallBackFunc();

		typedef void (*CallBackFunc)(const char* szError, CLogOwner* pOwner);
		void SetFunc(CallBackFunc pFunc);
		void CallFunc(const char* szError, CLogOwner* pOwner);

#ifdef _WIN32
		CallBackFunc	m_pFunc;
#else
		HTLSKEY			m_tlsFuncKey;
#endif
	};

	CErrLogCallBackFunc::CErrLogCallBackFunc()
	{
#ifdef _WIN32
		m_pFunc = NULL;
#else
		TLS_CreateKey(&m_tlsFuncKey);
#endif 
	}

	CErrLogCallBackFunc::~CErrLogCallBackFunc()
	{
#ifndef _WIN32
		TLS_DestroyKey(m_tlsFuncKey);
#endif
	}

	void CErrLogCallBackFunc::SetFunc(CErrLogCallBackFunc::CallBackFunc pFunc)
	{
#ifdef _WIN32
		m_pFunc = pFunc;
#else
		TLS_SetValue(m_tlsFuncKey, (void*)pFunc);
#endif
	}

	void CErrLogCallBackFunc::CallFunc(const char* szError, CLogOwner* pOwner)
	{
#ifdef _WIN32
		if (m_pFunc)
		{
			m_pFunc(szError, pOwner);
		}
#else
		CallBackFunc pFunc = (CallBackFunc)(TLS_GetValue(m_tlsFuncKey));

		if(!pFunc)
			return;

		pFunc(szError, pOwner);
#endif
	}

	CErrLogCallBackFunc ls_ErrLogCallBackFunc;

	void SetLogCallBackFunc(
		void (*pFunc)(const char* szError, CLogOwner* pOwner))
	{
		ls_ErrLogCallBackFunc.SetFunc(pFunc);
	}

	void CallLogCallBackFunc(const char* szError, CLogOwner* pOwner)
	{
		ls_ErrLogCallBackFunc.CallFunc(szError, pOwner);
	}

	void CallLogCallBackFunc(CError& exp, CLogOwner* pOwner)
	{
		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	LogString_t;

		LogString_t str;
		str.append("类型: ").append(exp.ErrorTitle()).append("\n");
		str.append("堆栈: \n").append(exp.StackInfo());
	
		if(exp.ErrorMsg()[0] != '\0')
			str.append("消息: ").append(exp.ErrorMsg());
	
		CallLogCallBackFunc(str.c_str(), pOwner);
	}

	//---------------------------------------------------------

	void SetErrLogEnabled(bool bEnabled)
	{
		CErrLogMgr::Inst().SetErrLogEnabled(bEnabled);
	}

	bool ErrLogEnabled()
	{
		return CErrLogMgr::Inst().ErrLogEnabled();
	}

	void SetErrLogFileName(const wchar_t* szFileName)
	{
		Ast(szFileName != NULL);
		CErrLogMgr::Inst().SetLogFileName(szFileName);
	}

	static bool gs_bErrLogEncrypt = false;
	void SetErrLogEncrypt(bool bEncrypt)
	{
		gs_bErrLogEncrypt = bEncrypt;
	}

	bool ErrLogEncrypted()
	{
		return gs_bErrLogEncrypt;
	}

#ifdef _WIN32
	static bool gs_bIORedirected = false;
	static FILE gs_fdOldStdOut;
	static FILE gs_fdOldStdErr;
	static FILE gs_fdOldStdIn;

	void RedirectIOToConsole(bool bEnabled)
	{
		if (gs_bIORedirected == bEnabled)
		{
			return;
		}

		gs_bIORedirected = bEnabled;

		if (bEnabled)
		{
			FILE *file;

			file=_fdopen(_open_osfhandle((size_t)::GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT),"w");
			VerFs(setvbuf(file,NULL,_IONBF,0));
			gs_fdOldStdOut=*stdout;
			*stdout=*file;		

			file=_fdopen(_open_osfhandle((size_t)::GetStdHandle(STD_ERROR_HANDLE),_O_TEXT),"w");
			VerFs(setvbuf(file,NULL,_IONBF,0));
			gs_fdOldStdErr=*stderr;
			*stderr=*file;

			file=_fdopen(_open_osfhandle((size_t)::GetStdHandle(STD_INPUT_HANDLE),_O_TEXT),"r");
			VerFs(setvbuf(file, NULL, _IOFBF, 4096));
			gs_fdOldStdIn=*stdin;
			*stdin=*file;
		}
		else
		{
			*stdout=gs_fdOldStdOut;
			*stdin=gs_fdOldStdIn;
			*stderr=gs_fdOldStdErr;
		}
	}

#endif	

	static uint32 gs_uRevision;
	static string gs_uVersion;
	//从文件中取得当前版本的svn版本号
	void FetchCurrentRevision(const wchar_t* szFileName)
	{
#ifdef _WIN32
		FILE* fd = _wfopen(szFileName, L"r");
#else
		char* strFileName = U16_to_U8(szFileName);
		FILE* fd = fopen(strFileName, "r");
		FreeUtfCvsDest(strFileName);
#endif
		
		if (!fd)
		{
			gs_uRevision = 0;
			return;
		}
		char szVersion[24];
		fgets(szVersion, 24, fd);
		
		for(int i = 0; i < 24; i++)
		{
			if(szVersion[i] == '\n')
			{
				if(szVersion[i - 1] == '\r')
				{
					szVersion[i - 1] = '\0';
					break;
				}
				else
				{
					szVersion[i] = '\0';
					break;
				}
			}
		}

		fclose(fd);

		sscanf(szVersion, "%u", &gs_uRevision);
	}
	//从文件中取得当前版本的发布版本号
	void FetchCurrentVersion(const wchar_t* szFileName)
	{
#ifdef _WIN32
		FILE* fd = _wfopen(szFileName, L"r");
#else
		char* strFileName = U16_to_U8(szFileName);
		FILE* fd = fopen(strFileName, "r");
		FreeUtfCvsDest(strFileName);
#endif
		
		if (!fd)
		{
			gs_uVersion = "";
			return;
		}
		char szVersion[24];
		fgets(szVersion, 24, fd);
		
		for(int i = 0; i < 24; i++)
		{
			if(szVersion[i] == '\n')
			{
				if(szVersion[i - 1] == '\r')
				{
					szVersion[i - 1] = '\0';
					break;
				}
				else
				{
					szVersion[i] = '\0';
					break;
				}
			}
		}

		fclose(fd);
		gs_uVersion = szVersion;
	//	sscanf(szVersion, "%s", &gs_uVersion);
	}
	//得到当前的SVN版本号
	uint32 GetCurrentRevision()
	{
		return gs_uRevision;
	}
	//得到当前的发行版本号
	const char * GetCurrentVersion()
	{
		return gs_uVersion.c_str();
	}



	uint32 GetRevisionFromLogFile(const char* szFileName)
	{
		string strName(szFileName);
		//处理版本号 版本号在_v和.之间，
		string::size_type stLeftBracket = strName.find_last_of('v');
		string::size_type stRightBracket = strName.find_last_of('_');

		string strVersion;

		if (stLeftBracket != string::npos && stRightBracket != string::npos)
		{
			strVersion = strName.substr(stLeftBracket + 1, stRightBracket - stLeftBracket - 1);
		}

		//如果包含非法字符，则返回空
		if(strVersion.find('/') != string::npos)
			return 0;

		uint32 uVersion = 0;
		sscanf(strVersion.c_str(), "%u", &uVersion);
		return uVersion;
	}

	void GetCurLogTime(wchar_t* szTime, size_t count)
	{
		time_t T;
		struct tm * timenow;
		time ( &T );
		timenow = localtime ( &T ); 
		
#ifdef _WIN32
		swprintf(szTime, L"%d%02d%02d_%02d%02d%02d", timenow->tm_year + 1900, timenow->tm_mon + 1, 
									 timenow->tm_mday, timenow->tm_hour,
									 timenow->tm_min, timenow->tm_sec);
#else
		swprintf(szTime, count, L"%d%02d%02d_%02d%02d%02d", timenow->tm_year + 1900, timenow->tm_mon + 1, 
			timenow->tm_mday, timenow->tm_hour,
			timenow->tm_min, timenow->tm_sec);
#endif
		szTime[count - 1] = L'\0';
	}

	//保存程序log存放路径
	static CLogString ms_strLogPath;

#ifndef _WIN32
	static char ms_strLogPathU8[256];
#endif

	void SetLogPath(const wchar_t* szPath)
	{
		if(szPath)
		{
			ms_strLogPath.assign(szPath);

#ifndef _WIN32			
			char* str = U16_to_U8(szPath);
			strncpy(ms_strLogPathU8, str, sizeof(ms_strLogPathU8) - 1);
			FreeUtfCvsDest(str);
			ms_strLogPathU8[sizeof(ms_strLogPathU8) - 1] = '\0';
#endif
		}
	}

	const wchar_t* GetLogPath()
	{
		return ms_strLogPath.c_str();
	}

#ifndef _WIN32
	const char* GetLogPathU8()
	{
		return ms_strLogPathU8;
	}
#endif

	
	static char gs_szProcessName[256] = {'\0'};

	void SetProcessName(const char* szErrLog)
	{
		strncpy(gs_szProcessName, szErrLog, sizeof(gs_szProcessName) - 1);

		gs_szProcessName[sizeof(gs_szProcessName) - 1] = '\0';
	}

	const char* GetProcessName()
	{
		return gs_szProcessName;
	}

	static unsigned char gs_ProcessGuid[16];
	static char gs_ProcessStringGuid[37] = {'\0'};	

	void SetProcessGUID(unsigned char guid[16])
	{
		memcpy(gs_ProcessGuid, guid, sizeof(gs_ProcessGuid));
		GuidToString(guid, gs_ProcessStringGuid);
	}

	unsigned char* GetProcessGUID()
	{
		return gs_ProcessGuid;
	}

	const char* GetProcessStringGUID()
	{
		return gs_ProcessStringGuid;
	}

	static struct tm gs_ProcessInitTime;
	void SetProcessInitTime()
	{
		time_t T;
		time ( &T );
		struct tm * timenow;
		timenow = localtime ( &T ); 

		memcpy(&gs_ProcessInitTime, timenow, sizeof(tm));
	}

	void GetProcessInitTime(struct tm* pInitTime)
	{
		memcpy(pInitTime, &gs_ProcessInitTime, sizeof(tm));
	}

	void ErrLogMsg(const char* key, const char* value)
	{
		if(!key || !value)
			return;

		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	LogString_t;

		LogString_t str;
		str.append(key).append(":").append(value);
		CErrLogMgr::Inst().WriteErrInfoLog(str.c_str());
	}

	void ErrLogHeadMsg(const char* key, const char* value)
	{
		if(!key || !value)
			return;

		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	LogString_t;

		LogString_t str;

		str.append(key).append(":").append(value);
		CErrLogMgr::Inst().WriteDeviceInfoLog(str.c_str());		
	}

	void SetLogFileInterval(uint32 uInterval)
	{
		CErrLogMgr::Inst().SetInterval(uInterval);
	}

	uint32 GetLogFileInterval()
	{
		return CErrLogMgr::Inst().GetInterval();
	}


	void SetCompressFun(CompressFun pFun)
	{
		CErrLogCompress::Inst().SetCompressFun(pFun);
	}

	void SetDeCompressFun(DeCompressFun pFun)
	{
		CErrLogCompress::Inst().SetDeCompressFun(pFun);
	}

	bool Compress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen)
	{
		return CErrLogCompress::Inst().Compress(szDest,uDestLen,szSource,uSourceLen);
	}

	bool DeCompress(char* szDest, uint32* uDestLen, const char* szSource, uint32 uSourceLen)
	{
		return CErrLogCompress::Inst().DeCompress(szDest,uDestLen,szSource,uSourceLen);
	}

	void Encode(char* buf, uint32 uLen)
	{
		CErrLogEncrypt::Inst().Encode(buf, uLen);
	}
	void Decode(char* buf, uint32 uLen)
	{
		CErrLogEncrypt::Inst().Decode(buf, uLen);
	}

	uint32 GetErrIndexCode(CError& exp)
	{
		const char* szErrorType = exp.ErrorTitle();
		const char* szStackInfo = exp.StackInfo();

		CLogStringStream strKey;
		strKey << szErrorType << szStackInfo;

		uint32 uLength = strKey.str().length();
		uint32 uErrId = !strKey.str().empty()?sqr::hash::jenkins::hash((uint8*)strKey.str().c_str(), uLength, uLength):0;
		return uErrId;
	}

}
