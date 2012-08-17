#include "stdafx.h"
#include "Memory.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "Module.h"
#include "MemoryHelper.h"
#include "CXmlConfig.inl"
#include "ThreadHelper.h"
#include "TimeHelper.h"
#include "ErrLogHelper.h"
#include "CLog.h"
#include "CPathMgr.h"
#include "CMapFileMgr.h"
#include "ExpHelper.h"
#include "GuidHelper.h"
#include "CThreadWatcherMap.h"
#include "CSqrAllocPoolMgr.h"
#include "UtfCvs.h"

#if defined _WIN32
#include <direct.h>
#else
#include <unistd.h>
#define _chdir chdir
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#define _getcwd getcwd
#define _strdup strdup
#endif

#ifdef _WIN32
#include "Shellapi.h"
#include <float.h>
#else
#include <locale.h>
#include <dlfcn.h>
#include <malloc.h>
#include <fenv.h>
#include <fpu_control.h>
#endif

#include "bzip2/bzlib.h"

#ifndef _WIN32
#	define _countof(array)	(sizeof(array)/sizeof(array[0]))
#endif

//--------------------------------------------------------------------------------------
//为了让MemoryManger单件被最先创建出来，这个工程不应该使用任何静态变量或者全局变量。
//如果使用cout或者cerr对象，也会导致这两个全局变量被初始化，因此应该尽量使用c的打印函数
//--------------------------------------------------------------------------------------

DEFINITION_OF_OPERATOR_NEW


#ifdef _WIN32
int run(int argc, wchar_t* argv[]);
#else
int run(int argc, char* argv[]);
#endif


static void PrintMessage(const wchar_t* szMessage)
{
#ifdef _WIN32
	MessageBoxW( NULL, szMessage, L"Error", MB_OK|MB_ICONERROR );
#else
	char* strMessage = U16_to_U8(szMessage);
	fputs( strMessage, stderr );
	FreeUtfCvsDest(strMessage);
#endif
}

int LoadFindLeakageConfig()
{
	int nCheckLevel = 0;

	FILE* f=fopen( "../../etc/common/FindLeakage" , "r" );
	
	if( f )
	{
		nCheckLevel=1;

		char szLine[128];

		while ( fgets( szLine , sizeof( szLine ) , f ) )
		{
			nCheckLevel=2;
			size_t const stLen=strlen( szLine )-1;
			if( szLine[stLen] == '\n' )
				szLine[ stLen ] = 0;

			if( stLen>0 )
				CSqrAllocPoolMgr::Inst().EnableFindLeakageOnPool( szLine );
		}	

		fclose( f);
	}

	return nCheckLevel;
}




#ifdef _WIN32

void InitExpHandler();
void UnitExpHandler();
void LogExpMsg(const wchar_t*);


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	int nResult = 0;
	SQR_TRY
	{
		WatchLeakage_Begin( LoadFindLeakageConfig() );

		InitExpHandler();

		unsigned control_word;
		_controlfp_s(&control_word,_PC_53,_MCW_PC);
		
		SetErrorMode( 0 );

		GetHDProcessTime();//初始化QueryPeformanceCounter的相关静态函数

		int nArgs;
		LPWSTR *wzArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		if(!wzArglist)
			GenErr("CommandLineToArgvW failed.");

		wchar_t**	szArgList= new wchar_t*[nArgs];

		for( int i=0;i<nArgs;++i )
		{
			wchar_t* wzArg=wzArglist[i];

			size_t stLen=wcslen(wzArg);
			wchar_t* szArg=new wchar_t[stLen+1];
			wcscpy(szArg, wzArg);
			szArg[stLen]=0;

			szArgList[i] = szArg;
		}

		LocalFree(wzArglist);

		nResult=run(nArgs,szArgList);

		for( int i=0;i<nArgs;++ i )
			delete[] szArgList[i];
		delete[] szArgList;

		//WL_End放在这里已经是不能再晚的时机了，一旦跳出main函数，所有线程都会强行被杀死，包括MemThread，那就根本查不到内存泄漏了
		WatchLeakage_End();
	}
	SQR_CATCH(exp)
	{
		wchar_t* wzMsg=U8_to_U16(exp.ErrorTitle());
		PrintMessage( wzMsg );
		FreeUtfCvsDest( wzMsg );		
	}
	SQR_TRY_END;

	return nResult;
}

#else

int main(int argc, char* argv[])
{
	WatchLeakage_Begin( LoadFindLeakageConfig() );

	fpu_control_t cw;
	_FPU_GETCW(cw);
	cw &= ~_FPU_EXTENDED;
	cw |= _FPU_DOUBLE;
	_FPU_SETCW(cw);
	feenableexcept(FE_DIVBYZERO|FE_OVERFLOW);
	setlocale( LC_ALL, "" );

	int nResult=run(argc, argv);

	//WL_End放在这里已经是不能再晚的时机了，一旦跳出main函数，所有线程都会强行被杀死，包括MemThread，那就根本查不到内存泄漏了
	WatchLeakage_End();

	return nResult;
}

#endif



//得到game name，如GameGas返回Gas，GameGac返回Gac
static wchar_t* GetGameName(const wchar_t* szModuleName)
{
	static wchar_t szGameName[256];

	wstring strGameName;
	strGameName.append(szModuleName);
	string::size_type uGamePos = strGameName.find(L"Game");
	if(uGamePos == strGameName.npos)
		return NULL;
	uGamePos += 4;

	wstring::size_type uDotPos = strGameName.find_last_of(L'.');
	strGameName = strGameName.substr(uGamePos, uDotPos - uGamePos);

	wcscpy(szGameName, strGameName.c_str());
	return szGameName;
}





void LoadConfigFile()
{
	const char* szDevConfigFileName = "etc/common/DevConfig.xml";	

	CXmlConfig DevConfig;
	
	DevConfig.LoadConfig( "dev_config", szDevConfigFileName );

	bool bCatchError			= true;
	bool bErrLogEncrypt			= true;	
	int32 uLogInterval			= -1;

#ifdef _WIN32
	bool bFloatingPointExp		= false;

	bFloatingPointExp	= DevConfig.DftGet<bool>( bFloatingPointExp , "win_float_exception" );

	if( bFloatingPointExp )
	{
		unsigned control_word;
		_controlfp_s(&control_word,0,0);
		unsigned cw = control_word;
		cw &=~(_EM_ZERODIVIDE|_EM_DENORMAL|_EM_OVERFLOW);
		_controlfp_s(&control_word,cw,_MCW_EM);
	}
#endif

	bCatchError			= DevConfig.DftGet<bool>( bCatchError , "catch_error" );
	bErrLogEncrypt		= DevConfig.DftGet<bool>( bErrLogEncrypt , "errlog_encrypt" );	
	uLogInterval		= DevConfig.DftGet<int32>( uLogInterval, "log_interval" );

	EnableCatchError( bCatchError );
	SetErrLogEncrypt(bErrLogEncrypt);
	SetLogFileInterval((uint32)uLogInterval);
	
	if(bCatchError)
	{
		//这个代码很重要，因为加入某次测试一个异常都没有发生就放出，那错误系统本身的bug就无法被测试出来。
		//这个代码就是为了避免这种情况
		SQR_TRY
		{
			SQR_THROW("Test SQR TRY and CATCH");
		}
		SQR_CATCH(exp)
		{
			puts(exp.ErrorTitle());
		}
		SQR_TRY_END;
	}
}

#ifdef _WIN32
int run(int argc, wchar_t* argv[])
#else
int run(int argc, char* argv[])
#endif
{
	LowerCurThreadPriority();
	
	if( argc < 2 )
	{
		PrintMessage( L"No module to load." );
		return 1;
	}

	//library name
	wstring szModuleName;
#ifdef _WIN32
	szModuleName = argv[1];
#else
	wchar_t* utf16Tmp = U8_to_U16(argv[1]);
	szModuleName = utf16Tmp;
	FreeUtfCvsDest(utf16Tmp);
#endif

	//game name
	wchar_t* szGameName = GetGameName(szModuleName.c_str());

	if(!szGameName)
	{
		PrintMessage( L"Not a valid game module");
		return 1;
	}
	
	wchar_t curPath[MAX_PATH];

#ifndef _WIN32
	char szBufTmp[MAX_PATH];
	getcwd(szBufTmp, sizeof(szBufTmp));
	utf16Tmp = U8_to_U16(szBufTmp);
	wcscpy(curPath, utf16Tmp);
	FreeUtfCvsDest(utf16Tmp);
#else
	_wgetcwd(curPath, _countof(curPath));
#endif

#ifdef _WIN32
	_wchdir(L"../..");
#else
	chdir("../..");
#endif

	LoadConfigFile();
	
	//设置启动进程的guid已经名字
	guid_t guid;
	CreateGuid(guid);

	SetProcessGUID(guid);

	//设置程序启动的时间
	SetProcessInitTime();

	ErrLogHeadMsg("Process GUID", GetProcessStringGUID());


	//取得当前版本号，现在我们先用svn的版本号来表示
	FetchCurrentRevision(L"etc/Revision.txt");
	FetchCurrentVersion(L"etc/Version.txt");

	char szVersion[16];
	sprintf(szVersion, "%u", GetCurrentRevision());
	ErrLogHeadMsg("Version", szVersion);

#ifdef _WIN32
	_wchdir(curPath);
#else
	char* utf8Tmp = U16_to_U8(curPath);
	chdir( utf8Tmp );
	FreeUtfCvsDest(utf8Tmp);
#endif

#ifdef _WIN32
	//容错模式下面，读取运行目录下面map file文件，
	//如果没有map file文件，则出错之后显示的堆栈信息只有函数地址

	//由于map文件是固定的，先在这里写死了，或者考虑以后写到配置文件
	wchar_t szGameMap[256];
	swprintf( szGameMap , _countof(szGameMap), L"Game%s.map" , szGameName );
	CMapFileMgr::Inst().HandleMapFile( szGameMap );
	CMapFileMgr::Inst().HandleMapFile( L"Module.map" );
	CMapFileMgr::Inst().HandleMapFile( L"RunModule.map" );
#endif

	wstring szCurPath;

#ifdef _WIN32
	wchar_t* utf16Tmp = _wcsdup(szGameName);
	wstring sGame = utf16Tmp;
	free(utf16Tmp);
#else
	utf8Tmp = U16_to_U8(szGameName);
	char* utf8Game = strdup(utf8Tmp);
	utf16Tmp = U8_to_U16(utf8Game);
	free(utf8Game);
	wstring sGame = utf16Tmp;
	FreeUtfCvsDest(utf8Tmp);
	FreeUtfCvsDest(utf16Tmp);
#endif

	//文件名以全部小写
	for (std::wstring::iterator i = sGame.begin(); i != sGame.end(); i++)
	{
		if (*i >= L'A' && *i <= L'Z')
			*i = (*i) + (L'a' - L'A');
	}

	//配置var目录下面的log文件，mem文件
	wchar_t szVarPath[256];
#ifdef _WIN32
	swprintf(szVarPath, _countof(szVarPath), L"../../var/%s", sGame.c_str());
#else
	swprintf(szVarPath, _countof(szVarPath), L"../../var/%ls", sGame.c_str());
#endif

	//进入var下面的game(如gas,gac)目录
#ifdef _WIN32
	_wchdir(szVarPath);
	utf16Tmp = _wgetcwd(NULL, MAX_PATH);
	szCurPath = utf16Tmp;
	free(utf16Tmp);
#else
	utf8Tmp = U16_to_U8(szVarPath);
	chdir( utf8Tmp );
	FreeUtfCvsDest(utf8Tmp);
	utf8Tmp = getcwd(NULL, MAX_PATH);
	utf16Tmp = U8_to_U16(utf8Tmp);
	free(utf8Tmp);
	szCurPath = utf16Tmp;
	FreeUtfCvsDest(utf16Tmp);
#endif


	//设定log路径，以后所有的log，包括err log，简易log，内存泄露都会写入此路径下面
	SetLogPath(szCurPath.c_str());

	//在var game目录下面创建gas的err log
	SetErrLogFileName(sGame.c_str());

	//设置log的压缩与解压缩函数
	SetCompressFun(&BZ2_bzBuffToBuffCompress);
	SetDeCompressFun(&BZ2_bzBuffToBuffDecompress);


#ifdef _WIN32	
	_wchdir(curPath);
#else
	utf8Tmp = U16_to_U8(curPath);
	chdir( utf8Tmp );
	FreeUtfCvsDest(utf8Tmp);
#endif


	int nResult = 0;

	CThreadWatcherMap::Init();

#ifdef _WIN32
	HMODULE hModule=LoadLibraryW( szModuleName.c_str() );
#else
	HMODULE hModule=LoadLibrary( argv[1] );
#endif
	
	if( !hModule )
	{
#ifdef _WIN32
		int error = GetLastError();
#endif
		
		wostringstream oss;

		oss << L"LoadLibrary \"" <<szModuleName<< L"\" failed with error:";


#ifndef _WIN32
		oss<< dlerror();
#else
		if( error == 0xc000001d )
		{
			//由于这个错误号在ErrorLookup程序中是找不到的，是一个没有在任何文档里面记录的错误号，所以这里专门写一个代码提示这个信息。
			PrintMessage( L"必须支持SSE2指令集的CPU才能运行该程序！" );

			nResult = 2;
			goto out;
		}
		oss <<error<<endl;
#endif


		oss<<L"argv is:"<<endl;

		for( int i=0;i<argc;++i )
			oss<<argv[i]<<endl;

#ifdef _WIN32
		LogExpMsg(oss.str().c_str());
		PrintMessage(L"LoadLibrary Error");
#else
		PrintMessage(oss.str().c_str());
#endif
	
		nResult = 2;
		goto out;
	}

	SQRENTRYPROC EntryProc;
	EntryProc = reinterpret_cast<SQRENTRYPROC>(GetProcAddress( hModule, "SqrMain"));

	if( !EntryProc )
	{
		PrintMessage( L"Not a valid module." );

		nResult = 3;
		goto out;
	}

#ifdef _WIN32
	//进入game之后我们需要屏蔽这个结构化异常的处理函数，因为这个只是为runmodule做的
	UnitExpHandler();
#endif

	EntryProc( argc-1, argv+1 );

#ifndef _WIN32
	/*
	如果开了内存管理，还是得把FetchSymbol放在这里，如果放在gcc扩展的__attribute__((destructor))
	那个函数里面，会无法运行下去，用gdb跟进去发现此时内存管理的多线程的堆栈已经没了，可能是因为
	多线程库是静态库，而这个被RunModule以及GameGas共享，当卸载GameGas之后，把多线程库也卸载了
	并且测试了一下，linux下面结束的时候在这里fetch all symbol的时间是350ms，这个时间完全能够接受
	*/

	//windows下不需要调用这个玩意儿，因为我们有mapfile
	WatchLeakage_FetchAllSymbol();
#endif

	FreeLibrary( hModule );

out:

	CThreadWatcherMap::Unit();

	return nResult;
}
