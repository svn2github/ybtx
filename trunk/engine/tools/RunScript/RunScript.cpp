// DbPerfTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CScript.h"
#include "ErrLogHelper.h"

using namespace sqr;

//打印使用帮助
static void PrintUsage();


//命令行参数提取
static const TCHAR* GetFileNameFromArgs(int nArgCount,TCHAR* aryArg[]);
static const TCHAR* GetOptionFromArgs(int nArgCount,TCHAR* aryArg[],const TCHAR* szOption);


int main(int argc, TCHAR* argv[])
{	
	if( argc < 2 )
	{
		PrintUsage();
		return 1;
	}

	const TCHAR* szFileName=GetFileNameFromArgs(argc,argv);
	if( !szFileName)
	{
		PrintUsage();
		return 1;
	}

	const TCHAR* szDebuger=GetOptionFromArgs(argc,argv,"-d");
	const TCHAR* szProfiler=GetOptionFromArgs(argc,argv,"-p");

	if( szDebuger && szProfiler )
	{
		PrintUsage();
		return 1;
	}

	EScriptHelper eScriptHelper;

	if( szDebuger )
	{
		eScriptHelper = eSH_Debuger;
	}
	else if( szProfiler)
	{
		eScriptHelper = eSH_Profiler;
	}
	else
	{
		eScriptHelper = eSH_None;
	}


	CScript* pScript=new CScript( eScriptHelper );
	
	int nResult=0;

	SQR_TRY
	{
		string sResult=pScript->RunFile(szFileName);
		cerr<<sResult;
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		nResult = 2;
	}
	SQR_TRY_END;

	delete pScript;

	return nResult;
}



/////////////////////////////////////////////////////////////////////////////////
//					帮助
/////////////////////////////////////////////////////////////////////////////////
//打印程序的用法
static void PrintUsage()
{
	cerr<<"Usage: RunScript [Options] FileName"<<endl
		<<"  Options:"<<endl
		<<"    -d      Enable debuger.It is mutually exclusive with -d option."<<endl
		<<"    -p      Enable profiler.It is mutually exclusive with -p option."<<endl
		<<"  FileName          Script file name."<<endl;
}


/////////////////////////////////////////////////////////////////////////////////
//					参数处理
/////////////////////////////////////////////////////////////////////////////////

//从命令行参数中提取脚本文件名称
static const TCHAR* GetFileNameFromArgs(int nArgCount,TCHAR* aryArg[])
{
	for(int i=1;i<nArgCount;++i)
	{
		const TCHAR* szArg=aryArg[i];
		if( szArg[0] != '-' )
			return szArg;
	}
	return NULL;
}

//从命令行参数中提取某个开关的额外参数
static const TCHAR* GetOptionFromArgs(int nArgCount,TCHAR* aryArg[],const TCHAR* szOption)
{
	for(int i=1;i<nArgCount;++i)
	{
		const TCHAR* szArg=aryArg[i];

		if(strncmp(szOption,szArg,strlen(szOption))!=0)
			continue;

		return szArg+2;
	}
	return NULL;
}
