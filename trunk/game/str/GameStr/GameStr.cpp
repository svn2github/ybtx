#include "stdafx.h"

#include "ScriptX.h"
#include "ExpHelper.h"
#include "RegistStrShell.h"
#include "CScriptAppStress.h"
#include "CPathMgr.h"
#include "Module.h"
#include "CodeCvs.h"


using namespace sqr;

//static void PrintUsage();

static int GetNumFromArgs(int nArgCount,wchar_t* aryArg[],const wchar_t* szOption);

static void RegistStressNum(CScript& Script);

static int StressNum;
#ifdef _WIN32
static int StressMaxNum = MAXIMUM_WAIT_OBJECTS - 2;
#else
static int StressMaxNum = 62;
#endif

SQRENTRY int SqrMain(int argc, wchar_t* argv[])
{
	//由于我们的命令行格式为 命令 -n Num ，只有3个参数，所以参数不对就不会运行
	//if( argc != 3 )
	//{
	//	PrintUsage();
	//	return 0;
	//}

	StressNum = GetNumFromArgs(argc, argv, L"-n");


	if(StressNum == 0)
	{
		cerr << "压力测试连接数目不能为0" << endl;
		return 0;
	}

	SQR_TRY
	{
		CScriptAppStress Runner("etc/str/StrConfig.xml");
	
		CScript* pScript = Runner.GetMainVM();

		RegistStressShell(*pScript);
		RegistStressNum(*pScript);

		string str_path = pScript->GetLoadPath("str");
		string str_main = str_path + string("main_frame/StrMain");

		string sError=Runner.Run(str_main.c_str());

		if( !sError.empty() )
			GenErr(sError);
	}
	SQR_CATCH(exp)
	{
		clog << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
		clog << exp.StackInfo() << endl;
	}
	SQR_TRY_END;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//					帮助
/////////////////////////////////////////////////////////////////////////////////
//打印程序的用法
/*
static void PrintUsage()
{
	cerr<<"运行: StressRunner :"<<endl
		<<"格式: StressRunner -n Num "<<endl
		<<"例子：StressRunner -n 100 "<<endl
		<<endl
		<<"  必要参数 "<<endl	
		<<"    -n Num     运行压力测试连接数"<<endl
		<<endl;
}
*/
/////////////////////////////////////////////////////////////////////////////////
//					参数处理
/////////////////////////////////////////////////////////////////////////////////

//从命令行参数中提取相应信息
static int GetNumFromArgs(int nArgCount,wchar_t* aryArg[],const wchar_t* szOption)
{
	for(int i=1;i<nArgCount;++i)
	{
		const wchar_t* szArg=aryArg[i];

		if(wcsncmp(szOption,szArg,wcslen(szOption))!=0)
			continue;

		//如果参数匹配了，我们将取出参数后面的整形变量并返回
		if(i + 1 < nArgCount)
		{
#ifdef _WIN32
			return _wtoi(aryArg[i + 1]);
#else
			return atoi( utf16_to_utf8(aryArg[i + 1]).c_str() );
#endif
		}
	}
	return 1;
}



//注册压力测试需要的两个参数，即压力测试连接数目，以及压力测试类型
static void RegistStressNum(CScript& Script)
{
	REGIST_CONSTANT(Script, StressNum);
	REGIST_CONSTANT(Script, StressMaxNum);
}

