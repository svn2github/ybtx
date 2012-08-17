#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LuaProfiler.h"
#include "GlobalState.h"

#include "clock.h"
#include "core.h"

static int StateID;

#define GLOBALSTATE_NAME "__GlobalState__"

//因为现在profiler要支持脚本库多线程，所以就不能用全局的这个标示了
//GlobalState  g_State;



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static const luaL_reg OpFunc[] = {
	{ "Start", profiler_init },
	{ "Stop", profiler_stop },
	{ NULL, NULL }
};


// 注册luaopen_profiler的C方法到Lua中，并注册一些基本信息
int luaopen_profiler(lua_State *L)
{
	luaL_openlib(L, "profiler", OpFunc, 0); // 注册方法
	lua_pushliteral(L, "_COPYRIGHT"); // metatable._COPYRIGHT = "Copyright (C) 2003-2007 Kepler Project"
	lua_pushliteral(L, "Copyright (C) 2009 Ybtx Project");
	lua_settable(L, -3); // 把上面的两个压入字串组成键值组成的表地址压入栈，并从栈中弹出键值
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, "LuaProfiler is a program grow with Ybtx project for find bottlenecks.");
	lua_settable (L, -3);
	lua_pushliteral (L, "_NAME");
	lua_pushliteral (L, "LuaProfiler");
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "0.1.0");
	lua_settable (L, -3);

	return 1;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// 计算出Lua调用一个函数需要的大约时间
static double calcCallTime(lua_State *L)
{
	sqr::uint64 timer;
	char Code[] = "                  \
		function MesureFunc()        \
			local i                  \
                                     \
			local t = function()     \
			end                      \
                                     \
			i = 1                    \
			while (i < 100000) do    \
				t()                  \
				i = i + 1            \
				end                  \
			end                      \
                                     \
			MesureFunc()             \
			MesureFunc = nil         \
		";

	StartTimer(&timer);
	luaL_dostring(L, Code); // 运行Lua代码

	return GetSeconds(timer) / (double) 100000;
}


// 调用或返回函数事件触发函数，通过CallHookIn和CallHookOut实现时间统计
static void CallHook(lua_State *L, lua_Debug *ar)
{
	int CurrentLine;
	lua_Debug PrevAr;
	ProfState* S = NULL;
	lua_pushlightuserdata(L, &StateID);
	lua_gettable(L, LUA_REGISTRYINDEX); // 根据StateID键获取registry表中的table
	S = (ProfState*)lua_touserdata(L, -1);

	// 查看level 1是否存在
	if (lua_getstack(L, 1, &PrevAr) == 0)
	{
		CurrentLine = -1;
	}
	else
	{
		lua_getinfo(L, "l", &PrevAr); // 获取该level的信息，并存储于局部lua_Debug结构中
		CurrentLine = PrevAr.currentline;
	}

	lua_getinfo(L, "nS", ar);

	if ( strstr(ar->source, "function") == NULL ) //  && strcmp(info->FileDefined, "=[C]") != 0
	{
		if (!ar->event) // 返回0代表是函数入口，1则是函数出口
			CallHookIn(S, (char*)ar->name, (char*)ar->source, ar->linedefined, CurrentLine);
		else
		{
			lua_getglobal(L, GLOBALSTATE_NAME);
			GlobalState* pGlobalState = (GlobalState*)lua_touserdata(L, -1);
			lua_pop(L, 1);
			CallHookOut(S, pGlobalState);
		}
	}
}


// 注册hook事件，生成log文件，并把当前profile状态压入registry表
int profiler_init(lua_State *L)
{
	lua_getglobal(L, GLOBALSTATE_NAME);
	//如果不为nil，则证明是多次init了，这时候我们先stop上一次的，在启动新的
	if(!lua_isnil(L, -1))
	{
		profiler_stop(L);	
	}
	lua_pop(L, 1);

	// 初始化全局变量
	GlobalState* pGlobalState = (GlobalState*)malloc(sizeof(GlobalState));
	lua_pushlightuserdata(L, pGlobalState);
	lua_setglobal(L, GLOBALSTATE_NAME);

	pGlobalState->BufCount = 0;
	pGlobalState->Buffer = (BufferData*)malloc(sizeof(BufferData) * MAX_BUFFER_COUNT);
	ProfState *S;
	const char* OutFile;

	pGlobalState->FuncCallTime = calcCallTime(L);
	OutFile = NULL;
	if (lua_gettop(L) == 1)
		OutFile = luaL_checkstring(L, -1); // 因为start有一个输出文件名的可选参数

	lua_sethook(L, (lua_Hook)CallHook, LUA_MASKCALL | LUA_MASKRET, 0);
	pGlobalState->HookMark = 1;

	if (!(S=InitCoreProfiler(OutFile, 1, pGlobalState))) // 生成log文件，并输出首行信息，返回初始化成员的S
	{
		luaL_error(L, "LuaProfiler error: output file could not be opened!");
		lua_pushnil(L);
		return 0;
	}

	// 输出键值对，把生成表存储在以LUA_REGISTRYINDEX为假索引的registry表（栈）中
	lua_pushlightuserdata(L, &StateID);
	lua_pushlightuserdata(L, S);
	lua_settable(L, LUA_REGISTRYINDEX);

	CallHookIn(S, "start", "(C)", -1, -1);
	lua_pushboolean(L, 1); // 为后面的检测提供依据

	return 1;
}


// 性能分析结束
int profiler_stop(lua_State *L)
{
	lua_getglobal(L, GLOBALSTATE_NAME);
	GlobalState* pGlobalState = (GlobalState*)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushnil(L);
	lua_setglobal(L, GLOBALSTATE_NAME);

	ProfState *S;
	lua_sethook(L, NULL, 0, 0); // 第三个参数为空，代表注销所有事件监听
	lua_pushlightuserdata(L, &StateID);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) // 如果metaltable存在
	{
		S = (ProfState*)lua_touserdata(L, -1);
		// 释放函数性能测试栈链，并停止所有计数，同时输出栈链信息
		while ( CallHookOut(S, pGlobalState) );
		CloseCoreProfiler(S, pGlobalState);
		lua_pushboolean(L, 1);
	}
	else
		lua_pushboolean(L, 0);

	pGlobalState->HookMark = 0;
	free(pGlobalState->Buffer);
	free(pGlobalState);
	return 1;
}

