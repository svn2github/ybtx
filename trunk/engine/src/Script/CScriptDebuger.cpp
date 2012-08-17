#include "stdafx.h"
#include "CScriptDebuger.h"
#include "ScriptCommon.h"
#include "CScript.h"
#include "ScriptX.h"
#include "ThreadHelper.h"
#include "CScriptData.h"
#include "ExpHelper.h"
#include "ScriptDebugHelper.h"
#ifdef _WIN32
#include "Console.h"
#endif
#include <algorithm>
#include "TimeHelper.h"
#include "CScriptDebugerMgr.h"


namespace sqr
{
	int DebugBreak(lua_State* pState)
	{
		CScriptDebuger* pDebuger=_S( pState ).GetData()->m_pDebuger;
		if(!pDebuger)
			return 0;
		//pDebuger->SetStepOut(pState);//从DebugBreak返回处开始断点

		pDebuger->Debug(pState);

		return 0;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//实验证明，创建一个新的coroutine的时候，会将原coroutine的钩子函数自动复制到新coroutine上
//当coroutine执行yield的时候，会将子coroutine的钩子函数复制到父coroutine上
//////////////////////////////////////////////////////////////////////////////////////////


CScriptDebuger::CScriptDebuger(lua_State* pState)
:m_pBuf(NULL),m_nBreakStackLevel(-1),m_pPrevState(NULL),m_pBaseState(NULL),m_pCurHook(NULL)
, m_pDbContainer(NULL)
{
	m_szLineBuffer[0]=0;
	lua_sethook( pState, &CScriptDebuger::HookProc, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0 );
}

void CScriptDebuger::Release(lua_State* pState)
{
	lua_sethook( pState, 0, 0, 0 );

	delete this;
}

bool CScriptDebuger::PrintLine( lua_State *pState, uint32 uLevel, int32 nLine, bool bIsCurLine )
{
	lua_Debug ld;
	lua_getstack ( pState, uLevel, &ld );
	lua_getinfo ( pState, "S", &ld );


	if( ld.source[0] == '=' )
	{
		printf( "Source not available for c function.\n" );
		return false;
	}

	if( ld.source[0] != '@' )
	{
		//trunk

		const char* pPos=ld.source;

		for( int i=1 ;i< nLine ;++i )
		{
			pPos=strchr(pPos,'\n');
			if( !pPos )
			{
				printf("Line number %d out of range; (trunk) has %d lines.\n",i+1,i);
				return false;
			}
			++pPos;
		}

		const char* pEndPos=pPos;

		while( ((*pEndPos) != '\n') && ((*pEndPos) !='\0') )
			++pEndPos;

		printf( "%d ", nLine);
		if(bIsCurLine)
			printf( ">>" );
		printf( "\t" );
		fwrite( pPos, 1, pEndPos - pPos, stdout );
		printf( "\n" );

		return true;
	}

	return DebugShowLine( ld.source + 1, nLine, bIsCurLine );
}

const char* CScriptDebuger::ReadWord(bool bNewLine)
{
	if(bNewLine)
		m_pBuf=NULL;

	if( !m_pBuf )
	{
		if(!bNewLine)
			return NULL;

		fprintf(stderr,"(db) ");
		
		char szPrevLineBuffer[EInputLineLength];
		memcpy( szPrevLineBuffer , m_szLineBuffer, sizeof(szPrevLineBuffer) );

		char* p;

		if( !fgets(m_szLineBuffer,sizeof(m_szLineBuffer),stdin) )
			return NULL;

		p=m_szLineBuffer;

		while( *p == ' ' || *p == '\t' )
			p++;

		if( *p == '\n' || *p == '\0' )
		{			
			strncpy( m_szLineBuffer , szPrevLineBuffer, sizeof(m_szLineBuffer) );
			
			p=m_szLineBuffer;

			while( *p == ' ' || *p == '\t' )
				p++;
		}

		strncpy( m_szWordBuffer , m_szLineBuffer, sizeof(m_szWordBuffer) );

		m_pBuf=p;
	}

	
	char* pWord = m_pBuf;

	//跳过当前词
	while( *m_pBuf != ' ' && *m_pBuf != '\t' && *m_pBuf != '\n' && *m_pBuf != '\0' )
		m_pBuf++;

	//吃掉所有空格和制表符
	while( *m_pBuf == ' ' || *m_pBuf == '\t' )
	{
		*m_pBuf = '\0';
		m_pBuf++;
	}

	//判断是到达了行结尾还是到达了下一个词语的起始位置
	if( *m_pBuf == '\0' || *m_pBuf == '\n' )
	{
		*m_pBuf = '\0';
		m_pBuf = NULL;
	}

	return pWord;
}

/*
hook的调用规律:
调用函数的时候，HOOKCALL和HOOKLINE先后在函数的第一行触发一次
从函数返回的时候，HOOKLINE和HOOKRETURN先后在函数的end处出发一次
在函数参数处直接调用函数时，函数返回后不会再参数处停留，而是直接进入参数所要传递给的函数。
*/

void CScriptDebuger::HookProc( lua_State *pState, lua_Debug* pDebug )
{
	CScriptDebuger* pThis=_S( pState ).GetData()->m_pDebuger;

	CScriptDebugerMgr::Inst().CheckBreak(pState, pDebug);
		
	bool(CScriptDebuger::* pCurHook)(lua_State*,lua_Debug*)=pThis->m_pCurHook;
	if( !pCurHook )
		return;

	bool bResult=(pThis->*pCurHook)( pState, pDebug );

	pThis->m_pPrevState = pState;

	if( !bResult )
		return;

	pThis->Debug(pState);
}

/*
	跳进处理（适用于同线程的情况）
	此函数用来处理进入函数的情况，如果没有在没有函数的地方运行，则等同于执行下一条语句
	line触发
	假设代码如下
	a = function()
		print("Hello World")
	end

	a()

	此时光标如果在a()处，我们点击i，则debugger会进入a函数里面，光标停留在a函数代码第一行即print前面
*/
bool CScriptDebuger::StepInHook( lua_State *pState,lua_Debug* pDebug )
{
	switch(pDebug->event)
	{
	case LUA_HOOKLINE:
		if(m_pBaseState != pState)
			return false;	
		return true;	
	case LUA_HOOKCALL:	
		if(m_pBaseState != pState)
			return false;			
		return false;
	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:
		return false;
	default:
		GenErr("Invalid event in lua hook function");
		return false;
	}
}

/*
	跳出处理（适用于同线程的情况）
	此函数用来从当前函数跳出
	return 触发
	假设代码如下
	a = function()
		print("Hello World")
	end

	a()
	
	光标在print处，如果此时按o，则光标会跳到end处，即函数结束处，再按o，则会跳出a函数，光标停留在a()下一行处

*/
bool CScriptDebuger::StepOutHook( lua_State *pState,lua_Debug* pDebug )
{
	switch(pDebug->event)
	{
	case LUA_HOOKCALL:
		return false;
	case LUA_HOOKLINE:
		if( m_pBaseState != pState )
		{		
			return false;
		}

		{
			//uint32 uRunningStackLevel=GetRunStackDepth(pState);
			//if( m_nBreakStackLevel == uRunningStackLevel + 1)
			//	return true;

		}
		return false;
	case LUA_HOOKTAILRET:
	case LUA_HOOKRET:
		if( m_pBaseState != pState )
		{		
			return false;
		}
		
		{
			int32 uRunningStackLevel=GetRunStackDepth(pState);
			if( m_nBreakStackLevel == uRunningStackLevel)
				return true;
			//Ast( m_nBreakStackLevel < uRunningStackLevel );
		}
		return false;
	default:
		GenErr("Invalid event in lua hook function");
		return false;
	}
}

/*
	执行下一条语句（适用于同线程情况）
	此函数用来执行下一条语句
	line触发
	假设代码如下
	a()
	b()
	此时光标在a处，运行n，则debugger会运行a()然后返回a下一条语句，即b前面

	如果在函数end处运行n，则跳出函数返回执行该函数处下一行
*/
bool CScriptDebuger::StepOverHook( lua_State *pState,lua_Debug* pDebug )
{
	switch(pDebug->event)
	{
	case LUA_HOOKCALL:
		return false;
	case LUA_HOOKTAILRET:
	case LUA_HOOKRET:
		if(m_pBaseState != pState)
		{
			return false;
		}	
		
		//nothing
		//{
		//	uint32 uRunningStackLevel=GetRunStackDepth(pState);
		//	if(m_nBreakStackLevel==uRunningStackLevel)
		//		m_nBreakStackLevel=uRunningStackLevel;
		//	Ast( m_nBreakStackLevel < uRunningStackLevel );
		//}
		return false;
	case LUA_HOOKLINE:
		if( m_pBaseState != pState )
			return false;		
		{
			int32 uRunningStackLevel = GetRunStackDepth(pState);
			if( uRunningStackLevel > m_nBreakStackLevel)
			{				
				return false;
			}
		}
		printf("step over hook\n");
		return true;
	default:
		GenErr("Invalid event in lua hook function");
		return false;
	}
}

/*
	切换线程
	此函数用来在不同的coroutine进行切换
	line触发
	假设代码如下

	co = coroutine.create(function()
		for i = 0, 10 do
			coroutine.yield()
		end
	end)

	coroutine.resume(co)
	coroutine.resume(co)

	光标在第一个resume前面，如果此时按s，则debugger会进入co函数里面，停在co函数第一行处，即切换到了coroutine的线程
	如果运行到yield处再按s，则debugger会切换到主线程，光标会停留在第二个resume前面。

	若没有coroutine切换，则s相当于执行i语句。

	另外必须注意，对于不同线程的处理只有s能有效，至于n，i，o则只能在同一个线程运行。
*/
bool CScriptDebuger::StepSwitchHook(lua_State *pState, lua_Debug* pDebug)
{
	switch(pDebug->event)
	{
	case LUA_HOOKCALL:
	case LUA_HOOKTAILRET:
	case LUA_HOOKRET:
		return false;
	case LUA_HOOKLINE:
		{
			if(m_pBaseState != pState)
			{
				printf("the stack is not the same\n");
			}
		}
		return true;
	default:
		GenErr("Invalid event in lua hook function");
		return false;
	}
}

void CScriptDebuger::SetStepSwitch(lua_State* pState)
{
	//if(m_pCurHook==&CScriptDebuger::StepSwitchHook)
	//	return SetStepIn(pState);

	m_pPrevState = m_pBaseState = pState;
	m_pCurHook = &CScriptDebuger::StepSwitchHook;
	m_nBreakStackLevel=INT_MAX;					//无堆栈级别需求，任何情况都可以断
}

void CScriptDebuger::SetStepOver(lua_State* pState)
{
	//if(m_pCurHook==&CScriptDebuger::StepOutHook)
	//	return SetStepIn(pState);
	//m_pPrevState=m_pBaseState=pState;
	m_pBaseState=pState;
	m_pCurHook=&CScriptDebuger::StepOverHook;
	m_nBreakStackLevel=GetRunStackDepth(pState); //栈级别必须与当前执行深度相同
}

void CScriptDebuger::SetStepIn(lua_State* pState)
{
	//m_pPrevState=m_pBaseState=pState;
	m_pBaseState=pState;
	m_pCurHook=&CScriptDebuger::StepInHook;
	m_nBreakStackLevel=INT_MAX;	//无堆栈级别需求，任何情况都可以断
}

void CScriptDebuger::SetStepOut(lua_State* pState)
{
	if(m_pCurHook==&CScriptDebuger::StepOutHook)
		return SetStepOver(pState);
	//m_pPrevState=m_pBaseState=pState;
	m_pBaseState=pState;
	m_pCurHook=&CScriptDebuger::StepOutHook;
	m_nBreakStackLevel=GetRunStackDepth(pState); //栈级别必须与当前执行深度相同
}

static void GetDbChannelCo(lua_State* pState)
{
	lua_getglobal(pState, "g_DbChannelMgr");	
	Ast(!lua_isnil(pState, -1));				
	lua_getfield(pState, -1, "_m_AllCoInfo");	
	lua_remove(pState, -2);						
}

static void PrintDbChannelCo(lua_State* pState)
{
	int top = lua_gettop(pState);
	
	GetDbChannelCo(pState);

	//把suspend的coroutine打印出来
	int row = 0;
	printf("\n");
	lua_pushnil(pState);//stack:dbco,nil
	while (lua_next(pState, -2) != 0)//stack: dbco, key ,value
	{
		lua_State *pCoState = lua_tothread(pState, -2);
		if (LUA_YIELD != lua_status(pCoState))
		{
			row++;
			printf("第");
			printf("%d", row);
			printf("个coroutine: ");
			PrintFrame(lua_tothread(pState, -2), 2);
		}
		lua_pop(pState, 1);
	}
	if (row == 0)
	{
		printf("此时DbChannelMgr没有被yield的coroutine！\n");
	}

	lua_settop(pState, top);
}

void CScriptDebuger::Debug(lua_State* pState)
{
	CScriptDebugerMgr::Inst().WaitForDebuger(this);

#ifdef _WIN32
	ShowConsole(true);	//控制台程序运行到这里的时候一定会返回false
#endif
	
	HTHREAD hThread;
	GetCurrentThread(&hThread);
	EnableWatchThread(hThread, false);

	uint64 uDebugBeginTime = GetProcessTime();

	const char* szBuf;
	
	m_bAtFileEnd=false;
	m_nCurLevel = 0;
	m_nCurLine = GetCurLine( pState,m_nCurLevel );

	PrintFrame(pState,m_nCurLevel);
	PrintLine(pState,m_nCurLevel,m_nCurLine,true);

	for(;;)
	{
		szBuf = ReadWord(true);
		if( !szBuf )	//Control+C
			exit(0);
		
		if( strlen( szBuf) == 0 )
			continue;
		
		if( !strcmp( szBuf, "help") )
		{
			HandleHelp();
		}
		else if( !strcmp( szBuf, "quit" ) || !strcmp( szBuf, "exit" ) || !strcmp( szBuf, "q" ) || !strcmp(szBuf, "Q"))
		{
			exit(0);
		}
		else if( !strcmp( szBuf, "continue" ) || !strcmp( szBuf, "c" ) || !strcmp(szBuf, "C"))
		{
			m_pPrevState=m_pBaseState=NULL;
			m_pCurHook=NULL;
			break;
		}
		else if( !strcmp( szBuf, "load" ) || !strcmp( szBuf, "lo") )
		{
			HandleLoad(pState);
		}
		else if( !strcmp( szBuf, "next" ) || !strcmp( szBuf, "n" ) || !strcmp(szBuf, "N"))
		{
			SetStepOver(pState);
			break;
		}
		else if( !strcmp( szBuf, "nl" ) )
		{
			SetStepOver(pState);
			HandleList(pState);
			break;
		}
		else if( !strcmp( szBuf, "in" ) || !strcmp( szBuf, "i" ) || !strcmp(szBuf, "I"))
		{
			SetStepIn(pState);
			break;
		}
		else if( !strcmp( szBuf, "out" ) || !strcmp( szBuf, "o" ) || !strcmp(szBuf, "O"))
		{
			SetStepOut(pState);
			break;
		}
		else if( !strcmp( szBuf, "switch" ) || !strcmp( szBuf, "s" ) || !strcmp(szBuf, "S"))
		{
			SetStepSwitch(pState);
			break;
		}
		else if( !strcmp( szBuf, "list" ) || !strcmp( szBuf, "l" ) || !strcmp(szBuf, "L"))
		{
			HandleList(pState);
		}
		else if( !strcmp( szBuf, "backtrace" ) || !strcmp( szBuf, "bt" ) || !strcmp(szBuf, "BT"))
		{
			for( int i = 0; PrintFrame( pState,i); i++ );
		}
		else if( !strcmp( szBuf, "frame" ) || !strcmp( szBuf, "f" ) || !strcmp(szBuf, "F"))
		{
			HandleFrame(pState);
		}
		else if( !strcmp( szBuf, "printco") || !strcmp( szBuf, "pco" ))
		{
			PrintDbChannelCo(pState);
		}
		else if( !strcmp( szBuf, "showco") || !strcmp( szBuf, "sco" ))
		{
			HandleShowCo(pState);
		}
		else if( !strcmp( szBuf, "print" ) || !strcmp( szBuf, "p" ) )
		{
			HandlePrint(pState);
		}
		else if(!strcmp( szBuf, "info" ) )
		{
			HandleInfo(pState);
		}
		else if(!strcmp( szBuf, "thread" ))
		{
			HandleThread(pState);
		}
		else if(!strcmp(szBuf, "break") || !strcmp(szBuf, "b"))
		{
			HandleAddBreak(pState);
		}
		else if(!strcmp(szBuf, "delete") || !strcmp(szBuf, "d"))
		{
			HandleDelBreak(pState);
		}
		else
		{   
			fprintf( stderr, "输入命令错误！！！\n" );
		}
	}

	SetProcessBeginTime( GetProcessTime() - uDebugBeginTime + GetProcessBeginTime() );

	EnableWatchThread(hThread, true);

#ifdef _WIN32
	ShowConsole(false);	//控制台程序运行到这里的时候一定会返回false
#endif

	CScriptDebugerMgr::Inst().CancelBlock(this);
}

void CScriptDebuger::HandleHelp()
{
	const char* szHelp=
		"backtrace/bt           列出调用堆栈\n"
		"continue/c             继续执行\n"
		"next/n                 执行下一行\n"
		"in/i                   跳进\n"
		"out/o                  跳出\n"
		"switch/s               切换coroutine\n"
		"list/l                 列出附近几行代码\n"
		"frame/f n              转到第n层堆栈\n"
		"print/p v              打印v的值\n"
		"load/lo file           载入或重新载入并执行指定文件\n"
		"printco/pco            打印DbChannel当前所有coroutine\n"
		"showco/sco n           跳入第n个coroutine执行堆栈\n"
		"info thread            显示脚本线程情况\n"
		"thread n               切换到另一个线程的debuger\n"
		"break/b source:line    设置断点\n"
		"info break             显示脚本所有断点\n"
		"delete/d n             删除某个断点\n"
		"delete/d               删除所有断点\n"
		"help                   打印帮助\n"
		;
			
	printf(szHelp);
}

void CScriptDebuger::HandleLoad(lua_State* pState)
{
	const char* szBuf = ReadWord();
	if(!szBuf)
	{
		lua_Debug ld;
		lua_getstack ( pState, m_nCurLevel, &ld );
		lua_getinfo ( pState, "S", &ld );

		if(*ld.source!='@')
		{
			fprintf( stderr, "Can only load lua file.\n" );
			return;
		}

		szBuf=(ld.source+1);
	}
	if(luaL_dofile( pState, szBuf ))
	{
		printf( "%s\n", lua_tostring( pState, -1 ));
		lua_pop( pState, 1);
	}
}


void CScriptDebuger::HandleList(lua_State* pState)
{
	const char* szBuf = ReadWord();

	int nLineCount=15; //一次显示代码的行数

	if(szBuf)
	{
		if(isdigit(*szBuf))
		{
			m_nCurLine=atoi(szBuf);
		}
		else
		{
			if(m_bAtFileEnd)
			{
				m_bAtFileEnd=false;
				m_nCurLine-=nLineCount;
			}
			else
			{
				m_nCurLine-=nLineCount*2;
			}

			if(m_nCurLine<=nLineCount/2)
				m_nCurLine=nLineCount/2;
		}
	}

	int nShowBeginLine=m_nCurLine-nLineCount/2; //显示代码的起始位置
	int nShowCounter=0;

	for( nShowCounter = 0; nShowCounter < nLineCount; nShowCounter++ )
	{
		int nShowLine=nShowBeginLine+nShowCounter;
		if(nShowLine>0)
		{
			if(!PrintLine( pState,m_nCurLevel,nShowLine,nShowLine==GetCurLine(pState,m_nCurLevel)))
			{
				m_bAtFileEnd=true;
				break;
			}
		}
	}
	m_nCurLine+=nShowCounter;
}

void CScriptDebuger::HandleFrame(lua_State* pState)
{
	const char* szBuf = ReadWord();
	bool bValidStackNumber=true;

	if(szBuf)
	{
		int nLevel = m_nCurLevel;

		if(isdigit(*szBuf))
		{
			nLevel=atoi( szBuf );

			lua_Debug ld;
			if( lua_getstack ( pState, nLevel, &ld ) )
			{
				m_nCurLevel = nLevel;
				m_nCurLine = GetCurLine( pState, m_nCurLevel );
				m_bAtFileEnd=false;
			}
			else
				bValidStackNumber=false;
		}
		else
			bValidStackNumber=false;
	}
	else
	{
		m_nCurLine = GetCurLine( pState, m_nCurLevel );
	}
	if(!bValidStackNumber)
	{
		fprintf(stderr,"Invalid stack number.\n");
		return;
	}
	PrintFrame( pState,m_nCurLevel );
	PrintLine( pState, m_nCurLevel,m_nCurLine,true);
}

void CScriptDebuger::HandleShowCo(lua_State* pState)
{
	const char* szBuf = ReadWord();
	if(szBuf)
	{
		int nLevel = 1;

		if(isdigit(*szBuf))
		{
			nLevel = atoi(szBuf);
			GetDbChannelCo(pState);
			lua_pushnil(pState);
			int num = 1;
			lua_State *pCoState = NULL;
			while (lua_next(pState, -2) != 0)
			{
				if(num == nLevel)
				{
					pCoState = lua_tothread(pState, -2);		
					break;
				}
				lua_pop(pState, 1);
				num++;
			}
			if(pCoState)
			{
				Debug(pCoState);
			}
		}
	}
}

static bool FindAndPrintInLocal(lua_State* pState, lua_Debug* pDebug, 
									 const string& sName, const list<string>& listField)
{
	int n = 1;
	const char* name = NULL;
	bool bFound = false;
	int nTop = lua_gettop(pState);
	while(( name = lua_getlocal( pState, pDebug, n++ ) ) != NULL )
	{
		bFound = (sName == name);
		if( bFound )
		{
			Print_lua( pState, listField );
			break;
		}
		else
		{
			lua_pop( pState, 1 );
		}
	}

	lua_settop(pState, nTop);
	return bFound;
}

static bool FindAndPrintInUpvalue(lua_State* pState, lua_Debug* pDebug, 
									 const string& sName, const list<string>& listField)
{
	bool bFound = false;
	const char* name = NULL;
	int nTop = lua_gettop(pState);

	lua_getinfo( pState, "f", pDebug );

	int n = 1;

	while( !bFound && ( name = lua_getupvalue( pState, -1, n++ ) ) != NULL )
	{
		bFound = (sName == name);
		if( bFound )
		{
			Print_lua( pState, listField );
			break;
		}
		else
		{
			lua_pop( pState, 1 );
		}
	}

	lua_settop(pState, nTop);

	return bFound;
}

static bool FindAndPrintInEnv(lua_State* pState, lua_Debug* pDebug, 
									   const string& sName, const list<string>& listField)
{
	bool bFound = false;
	
	int nTop = lua_gettop(pState);
	
	lua_getinfo( pState, "f", pDebug );

	lua_getfenv(pState, -1);

	lua_getfield(pState, -1, sName.c_str());
	if( ( bFound = lua_type( pState, -1 ) != LUA_TNIL ) )
		Print_lua( pState, listField );
	else
		lua_pop( pState, 1 );
	
	lua_settop(pState, nTop);
	return bFound;
}

static bool FindAndPrintInGlobal(lua_State* pState, lua_Debug* pDebug, 
								   const string& sName, const list<string>& listField)
{
	bool bFound = false;
	int nTop = lua_gettop(pState);

	lua_getglobal( pState, sName.c_str() );
	if( ( bFound = lua_type( pState, -1 ) != LUA_TNIL ) )
		Print_lua( pState, listField );
	else
		lua_pop( pState, 1 );

	lua_settop(pState, nTop);
	return bFound;
}

void CScriptDebuger::HandlePrint(lua_State* pState)
{
	const char* szBuf = ReadWord();

	string sVarName;

	if(!szBuf )
	{
		if(m_sLastVarName.empty())
		{
			printf("The history is empty.\n");
			return;
		}
		sVarName=m_sLastVarName;
	}
	else
	{
		sVarName=szBuf;
	}

	lua_Debug ld;
	if( lua_getstack ( pState, m_nCurLevel, &ld ) )
	{
		list<string> listField;

		vector<char> szTemp( sVarName.size() + 1, 0 );
		memcpy( &szTemp[0], sVarName.c_str(), sVarName.size() );

		const char* szSep = ".[]";

		char buffer[1024];
		strcpy(buffer, sVarName.c_str());

		char* token = strtok( buffer, szSep ); 
		while( token != NULL )
		{
			listField.push_back(token);
			token = strtok( NULL, szSep ); 
		}

		if (listField.empty())
		{
			printf("Input Invalid Parameter\n");
			return;
		}

		string sFirstName = *listField.begin();
		listField.erase( listField.begin() );

		bool bFound = false;

		do 
		{
			if(bFound = FindAndPrintInLocal(pState, &ld, sFirstName, listField))
				break;
			
			if(bFound = FindAndPrintInUpvalue(pState, &ld, sFirstName, listField))
				break;

			if (bFound = FindAndPrintInEnv(pState, &ld, sFirstName, listField))
				break;

			if (bFound = FindAndPrintInGlobal(pState, &ld, sFirstName, listField))
				break;

		} while (0);

		if( !bFound )
		{
			string strExe = "print(" + sVarName + ")";
			luaL_dostring( pState, strExe.c_str() );
		}
	}
	else
		printf( "Invalid stack number!\n" );

	m_sLastVarName=sVarName;
}

void CScriptDebuger::HandleThread(lua_State* pState)
{
	const char* szBuf = ReadWord();

	if(szBuf)
	{
		if(isdigit(*szBuf))
		{
			uint32 uId = (uint32)atoi( szBuf );
			CScriptDebugerMgr::Inst().SwitchDebuger(uId, this);
		}
		else
		{
			printf("Please Input Corrent Thread ID\n");
		}
	}
}

void CScriptDebuger::HandleInfo(lua_State* pState)
{
	const char* szBuf = ReadWord();
	if(!strcmp( szBuf, "thread" ))
	{
		CScriptDebugerMgr::Inst().PrintDebugerInfo();		
	}
	else if(!strcmp( szBuf, "break") )
	{
		CScriptDebugerMgr::Inst().ListBreaks();
	}
}

void CScriptDebuger::HandleAddBreak(lua_State* pState)
{
	const char* szBuf = ReadWord();
	if(szBuf)
	{
		CScriptDebugerMgr::Inst().AddBreak(szBuf);	
	}
}

void CScriptDebuger::HandleDelBreak(lua_State* pState)
{
	const char* szBuf = ReadWord();
	if(szBuf)
	{
		if(isdigit(*szBuf))
		{
			uint32 uBreak = (uint32)atoi(szBuf);
			CScriptDebugerMgr::Inst().DelBreak(uBreak);
		}
		else
		{
			printf("请输入正确的断点编号\n");
		}
	}
	else
	{
		CScriptDebugerMgr::Inst().DelBreaks();
	}
}

