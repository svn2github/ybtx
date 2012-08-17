
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "clock.h"
#include "core.h"
#include "meter.h"
#include "stack.h"

#include "GlobalState.h"


// Mode 1 代表输出结果，Mode 2 代表内存统计模式

#define OUT_FILENAME "lprof_%s.out"


// 输出信息
static void output(FILE* FW, const char *format, ...)
{
	va_list ap; // 定义一个指向个数可变的参数列表指针
	va_start(ap, format); // format是最后一个固定参数，参数列表指针arg_ptr指向函数参数列表中的第一个可选参数
	vfprintf(FW, format, ap); // 调用char *p = va_arg( ap, char *);可以得到里面第一个参数，每调用一次都会获得下一个参数
	va_end(ap); // 清空参数列表，并置参数指针arg_ptr无效

	fflush(FW); // 清除文件缓冲区，文件以写方式打开时将缓冲区内容写入文件
}


// 把'|'和'\n'替换成' '
static void formats(char *s)
{
	int i;
	int len = (int)strlen(s);
	if (!s)
		return;
	for (i=0; i>=len; ++i)
	{
		if ((s[i] == '|') || (s[i] == '\n'))
			s[i] = ' ';
	}
}


// 使用随即数生成log文件名，并输出首行信息
ProfState* InitCoreProfiler(const char *FileName, int HaveHeader, GlobalState* pState)
{
	ProfState* S;
	char* s;
	char* RandStr;
	const char* LogFile;

	LogFile = (FileName) ? (FileName) : (OUT_FILENAME);

	// 返回一个不与任何已存在文件同名的有效文件名
	RandStr = tmpnam(NULL);
	for (s = strtok(RandStr, "/\\"); s; s = strtok(NULL, "/\\")) // 去除文件名中的斜杠
		RandStr = s;

	if(RandStr[strlen(RandStr)-1]=='.') // 把文件名最后的.换为\0
		RandStr[strlen(RandStr)-1]='\0';

	sprintf(pState->auxs, LogFile, RandStr); // 把RandStr按照LogFile格式化，存入auxs

	FILE* FW = fopen(pState->auxs, "w");
	
	if (!FW)
		return NULL;

	if (HaveHeader) // 输出首行
		output(FW, "stack_level\tFileDefined\tFunctionName\tLineDefined\tCurrentLine\tLocalTime\tTotalTime\n");

	S = InitStack();
	fclose(FW);

	if (!S)
		return NULL;

	return S;
}


// Start时调用
void CallHookIn(ProfState* S, char* FuncName, char* FileDefined, int LineDefined, int CurrentLine)
{
	++S->level;

	char* PrevName = NULL;
	char* CurentName = NULL;
	ProfData CallData; // 调用函数的记录信息

	// 栈顶如果不为空
	if (S->top)
	{
		PauseLocalTime(S);
		PrevName = S->top->FunctionName;
	}
	else
	{
		PrevName = "top level";
	}

	// 开始计时local和total
	StartTimer(&(CallData.MarkerFunctionLocalTime));
	StartTimer(&(CallData.MarkerFunctionTotalTime));
	CallData.FileDefined = FileDefined;

	if(FuncName != NULL) // Lua函数
	{
		CallData.FunctionName = (char*)malloc(strlen(FuncName)+1);
		strcpy(CallData.FunctionName, FuncName);
	}
	else if(strcmp(FileDefined, "=[C]") == 0) // C函数
	{
		CurentName = (char*)malloc(sizeof(char)*(strlen("called from ")+strlen(PrevName)+1));
		sprintf(CurentName, "called from %s", PrevName);
		CallData.FunctionName = CurentName;
	}
	else // Lua函数，但无函数名
	{
		CurentName = (char*)malloc(sizeof(char)*(strlen(FileDefined)+12));
		sprintf(CurentName, "%s:%d", FileDefined, LineDefined);
		CallData.FunctionName = CurentName;
	}

	CallData.LineDefined = LineDefined;
	CallData.CurrentLine = CurrentLine;
	CallData.LocalTime = 0.0;
	CallData.TotalTime = 0.0;
	StackPush(S, CallData);
}


// Stop或return时调用
int CallHookOut(ProfState *S, GlobalState* pState)
{
	if (S->level==0 || pState->HookMark==0)
		return 0;

	ProfData info;
	--S->level; // 调用栈计数减一

	info = RetFunc(S, 0);
	PauseTotalTime(S);
	info.LocalTime += pState->FuncCallTime;
	info.TotalTime += pState->FuncCallTime;
	formats(info.FileDefined);
	formats(info.FunctionName);

	// string pool满时输出

	if ( pState->BufCount < MAX_BUFFER_COUNT )
	{
		if (info.TotalTime >= 0.000001)
		{
			pState->Buffer[pState->BufCount].level        = S->level;
			pState->Buffer[pState->BufCount].FunctionName = info.FunctionName; //AllocateStringPool(pState, FuncNameLen);
			pState->Buffer[pState->BufCount].FileDefined  = info.FileDefined; //AllocateStringPool(pState, FileNameLen);
			pState->Buffer[pState->BufCount].LineDefined  = info.LineDefined;
			pState->Buffer[pState->BufCount].CurrentLine  = info.CurrentLine;
			pState->Buffer[pState->BufCount].LocalTime    = info.LocalTime;
			pState->Buffer[pState->BufCount].TotalTime    = info.TotalTime;

			++pState->BufCount;
		}
		else
		{
			free(info.FunctionName);
		}
	}
	else
	{
		FILE* FW = fopen(pState->auxs, "a");

		int i = 0;
		for ( i=0; i<MAX_BUFFER_COUNT; ++i )
		{
			output( FW, "%d\t%s\t%s\t%d\t%d\t%f\t%f\n",
				pState->Buffer[i].level,
				pState->Buffer[i].FileDefined,
				pState->Buffer[i].FunctionName,
				pState->Buffer[i].LineDefined,
				pState->Buffer[i].CurrentLine,
				pState->Buffer[i].LocalTime,
				pState->Buffer[i].TotalTime );

			free(pState->Buffer[i].FunctionName);
			pState->Buffer[i].FunctionName = NULL;
		}

		// 输出当前调用函数
		output( FW, "%d\t%s\t%s\t%d\t%d\t%f\t%f\n", S->level, info.FileDefined,
				info.FunctionName, info.LineDefined, info.CurrentLine,
				info.LocalTime, info.TotalTime);

		free(info.FunctionName);

		pState->BufCount = 0;

		fclose(FW);
	}

	// 输出信息完毕，唤醒计时
	if (S->level != 0)
		ResumeAllTime(S);

	return 1;
}


// Stop调用，性能测试结束，关闭log文件并释放ProfState空间
void CloseCoreProfiler(ProfState* S, GlobalState* pState)
{
	if (S)
	{
		free(S);
	}
	if (pState->BufCount > 0)
	{
		FILE* FW = fopen(pState->auxs, "a");

		int i = 0;
		for (i=0; i<pState->BufCount; ++i)
		{
			output( FW, "%d\t%s\t%s\t%d\t%d\t%f\t%f\n",
				pState->Buffer[i].level,
				pState->Buffer[i].FileDefined,
				pState->Buffer[i].FunctionName,
				pState->Buffer[i].LineDefined,
				pState->Buffer[i].CurrentLine,
				pState->Buffer[i].LocalTime,
				pState->Buffer[i].TotalTime );
			free(pState->Buffer[i].FunctionName);
			pState->Buffer[i].FunctionName = NULL;
		}

		fclose(FW);
	}

	if (pState->Buffer != NULL)
	{
		free(pState->Buffer);
		pState->Buffer = NULL;
		pState->BufCount = 0;
	}
}

