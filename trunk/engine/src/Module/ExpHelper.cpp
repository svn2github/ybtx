#include "stdafx.h"
#include "ExpHelper.h"
#include "CJmpStack.h"
#include "CCallStackInfo.h"
#include "CError.h"
#include "ErrLogHelper.h"
#include "CArtError.h"
#include "UtfCvs.h"
#include "CLock.h"
#include "ThreadHelper.inl"
#include "jenkins.h"

#ifdef _WIN32
#include <eh.h>
#include <float.h>
#endif

#ifndef _WIN32
#include "execinfo.h"
#endif

namespace sqr
{
	static void ShowErr( const char* szMsg)
	{
#ifndef _WIN32
		printf("abort|%s\n",szMsg);
		abort();
#else
		switch(MessageBox(NULL,szMsg,"异常",MB_ABORTRETRYIGNORE|MB_ICONERROR))
		{
		case IDABORT:
			exit(1);
		case IDIGNORE:
			break;
		case IDRETRY:
			db();
			break;
		}
#endif
	}


	void MODULE_API DbgGenErr(const std::string& sErrorType)
	{
		if( !CatchErrorEnabled() )
		{
			GenErr( sErrorType );
		}
		else
		{
			LogErr( sErrorType );
		}
	}

	void MODULE_API DbgGenErr(const std::string& sErrorType, const std::string& sErrorMsg)
	{
		if( !CatchErrorEnabled() )
		{
			GenErr( sErrorType,sErrorMsg );
		}
		else
		{
			LogErr( sErrorType,sErrorMsg );
		}
	}

	
	void GenErr(const string& sErrorType)
	{
		if ( CatchErrorEnabled() )
		{
			CError *pError = new CError(sErrorType);
			SQR_THROW(pError);			
		}
		else
		{
			ostringstream strm;
			strm << sErrorType << endl;
			ShowErr(strm.str().c_str());
		}
	}

	void GenErr(const string& sErrorTitle, const string& sErrorMsg)
	{
		if ( CatchErrorEnabled() )
		{
			CError *pError = new CError(sErrorTitle);
			pError->AppendMsg(sErrorMsg);

			SQR_THROW(pError);
		}
		else
		{
			ostringstream strm;
			strm << sErrorTitle << endl << sErrorMsg << endl;
			ShowErr(strm.str().c_str());
		}
	}

	void GenErrnoErr( const string& sErrorTitle )
	{
#ifdef _WIN32
		DWORD dw=::GetLastError();
		ostringstream strm;
		strm<<sErrorTitle<<"\nGetLastError():"<<dw;
#else
		ostringstream strm;
		strm<<sErrorTitle<<endl<<::strerror(errno);		
#endif

	}

	void GenErrnoErr(const std::string& sErrorTitle, int32 nErrorNum )
	{
		ostringstream strm;
		strm<<sErrorTitle<<nErrorNum;
		GenErr( strm.str() );
	}

	void CfgErr(const std::string& sErrorType)
	{
		if ( CatchErrorEnabled() )
		{
			CError *pError = new CError(sErrorType);
			pError->SetCategory(CFG_ERR_MSG);
			SQR_THROW(pError);			
		}
		else
		{
			ostringstream strm;
			strm << CFG_ERR_MSG << "  " << sErrorType << endl;
			ShowErr(strm.str().c_str());
		}
	}

	void CfgErr(const std::string& sErrorType, const std::string& sErrorMsg)
	{
		if ( CatchErrorEnabled() )
		{
			CError *pError = new CError(sErrorType);
			pError->SetCategory(CFG_ERR_MSG);
			pError->AppendMsg(sErrorMsg);
			SQR_THROW(pError);			
		}
		else
		{
			ostringstream strm;
			strm << CFG_ERR_MSG << "  " << sErrorType << " " << sErrorMsg << endl;
			ShowErr(strm.str().c_str());
		}
	}



	void MODULE_API db()
	{
#ifdef _WIN32
		__debugbreak();
#else
		abort();
#endif
	}

	//如果coredump为true，则发生异常时候直接coredump了，否则发生异常写log
	static bool gs_bCatchError = false;

	void MODULE_API EnableCatchError(bool bCatchError)
	{
		if( bCatchError )
		{
			fputs("开启捕获异常",stderr);
		}
		else
		{
			fputs("关闭捕获异常",stderr);
		}
		gs_bCatchError = bCatchError;
	}

	bool MODULE_API CatchErrorEnabled()
	{
		return gs_bCatchError;
	}

#ifdef _WIN32
	CLock	ls_ShowErrorMsgLock;
#endif

	void ShowErrorMsg(const char* szMsg, const char* szType, bool bShowOnce)
	{
		if (!szMsg)
		{
			return;
		}

#ifdef _WIN32
		if (bShowOnce)
		{
			uint32 uLen = (uint32)strlen(szMsg);
			uint32 uMsgCode = sqr::hash::jenkins::hash((uint8*)szMsg, uLen, uLen);

			static map<uint32, bool>	ls_mapErrToShow;

			map<uint32, bool>::iterator iter = ls_mapErrToShow.find(uMsgCode);
			if (iter == ls_mapErrToShow.end())
			{
				ls_mapErrToShow.insert( make_pair(uMsgCode, true) );
			}	

			ls_ShowErrorMsgLock.Lock();
			if (ls_mapErrToShow[uMsgCode])
			{
				std::string str;
				str.append(szMsg);
				str.append("\n\n是否不再提示这个错误？\n");

				uint32 uRetType = MessageBox(NULL, str.c_str(), szType, MB_YESNO);
				switch(uRetType)
				{
				case IDYES:
					ls_mapErrToShow[uMsgCode] = false;
					break;
				case IDNO:
					break;
				}	
			}
			ls_ShowErrorMsgLock.Unlock();
		}
		else
		{
			MessageBox( NULL, szMsg, szType, MB_OK|MB_ICONERROR );
		}
#else
		fprintf( stderr, "%s %s\n", szType, szMsg );
#endif		
	}

	void ShowErrorMsgAndExit(const char* szMsg)
	{
#ifdef _WIN32
		MessageBox( NULL, szMsg, "FATAL ERROR",  MB_OK|MB_ICONERROR );
#else
		fputs( szMsg, stderr );
#endif
		_exit(1);
	}

#ifdef _WIN32
	void MODULE_API CreateCoreDump( _EXCEPTION_POINTERS* pException )
	{
		const char* sz_guid = GetProcessStringGUID();
		wchar_t* szGuid = U8_to_U16(sz_guid);

		static int32 uESHCoreCount = 1;

		wostringstream strm;
		strm << GetLogPath() << L"/core_" << szGuid 
			<< "_" << Atomic_FetchAndAdd(&uESHCoreCount, 1) 
			<<  "v" << GetCurrentRevision() << ".dmp";

		FreeUtfCvsDest(szGuid);

		HANDLE hReportFile = CreateFileW( strm.str().c_str(), GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0 );
		if( !hReportFile )
			return;

		//#ifdef USE_DEBUGHELP
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pException;
		ExInfo.ClientPointers = NULL;

		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hReportFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		//#else
		//		DWORD n;
		//		WriteFile( hReportFile, pException->ExceptionRecord, sizeof(EXCEPTION_RECORD), &n, NULL );
		//		WriteFile( hReportFile, pException->ContextRecord, sizeof(CONTEXT), &n, NULL );
		//#endif

		CloseHandle( hReportFile );
	}

	static HTLSKEY  gs_hESHKey;

	class InitESHKey
	{
	public:
		InitESHKey(){ TLS_CreateKey(&gs_hESHKey); }
		~InitESHKey() { TLS_DestroyKey(gs_hESHKey); }
	};

	static InitESHKey gs_InitESHKey;

	void EnableESHDump(bool bEnable)
	{
		if (bEnable)
		{
			TLS_SetValue(gs_hESHKey, (void*)1);
		}
		else
		{
			TLS_SetValue(gs_hESHKey, (void*)0);
		}
	}

	bool ESHDumpEnabled()
	{
		return (TLS_GetValue(gs_hESHKey) == (void*)1);
	}

	//当发生windows结构化异常时候转换成c++标准异常，供外面捕获
	void MODULE_API TransESHFun(unsigned int, struct _EXCEPTION_POINTERS* pContext)
	{
		//这个时候可能是发生浮点异常出现的，所以我们先清空浮点标记位
		//不然下一次浮点操作就又会爆掉
		_clearfp();

		if (ESHDumpEnabled())
		{
			CreateCoreDump(pContext);
		}

		CCallStackInfo* pInfo=new CCallStackInfo(pContext->ContextRecord);
		
		const char* buf = pInfo->GetText();

		ostringstream strm;
		DWORD ErrNum = pContext->ExceptionRecord->ExceptionCode;
		PVOID ErrAddr = pContext->ExceptionRecord->ExceptionAddress;

		char* szError = (char*)malloc(1024 * sizeof(char));
		sprintf(szError, "函数:0x%x发生0x%x号结构化异常", ErrAddr,ErrNum );

		//如果发生的是堆栈溢出的bug，我们记录一个dmp
		if (ErrNum == EXCEPTION_STACK_OVERFLOW)
		{
			CreateCoreDump(pContext);
		}

		CError error(szError, buf);

		//对于非法地址访问，我们可以得到更加详细的信息
		if(ErrNum == EXCEPTION_ACCESS_VIOLATION)
		{
			const char* szOpt = pContext->ExceptionRecord->ExceptionInformation[0] ? "写入" : "读取";
			size_t stMemAddr = pContext->ExceptionRecord->ExceptionInformation[1];

			const char* szType = NULL;
			
			if(stMemAddr > 1024)
			{
				szType = "非法内存访问";
			}
			else
			{
				szType = "无效内存访问";
			}

			sprintf(szError, "在%s内存地址 0x%x 的时候发生%s\n", szOpt, stMemAddr, szType);
			error.AppendMsg(szError);	
		}
		
		free(szError);
		delete pInfo;

		SQR_THROW(error);

	}
#endif

	void ArtErr(const std::string& sErrorType)
	{
		CArtError::Inst().ArtError(sErrorType);	
	}
	
	void ArtErr(const std::string& sErrorType, const std::string& sErrorMsg)
	{
		CArtError::Inst().ArtError(sErrorType, sErrorMsg);		
	}

}
