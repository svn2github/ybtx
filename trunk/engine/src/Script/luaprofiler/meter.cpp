
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "meter.h"
#include "stack.h"


#include "GlobalState.h"


// 计算局部时间
static void ComputeLocalTime(ProfData* e)
{
	ASSERT(e, "local time null");
	e->LocalTime += GetSeconds(e->MarkerFunctionLocalTime);
}


// 计算总时间
static void ComputeTotalTime(ProfData* e)
{
	ASSERT(e, "total time null");
	e->TotalTime += GetSeconds(e->MarkerFunctionTotalTime);
}

// 为当前函数计算时间，CallFunc
void PauseLocalTime(ProfState* S)
{
	ComputeLocalTime(S->top);
}


// 暂停总时间，CallHookOut
void PauseTotalTime(ProfState* S)
{
	ProfData* aux;

	ASSERT(S->top, "pause_TotalTime: stack_top null");

	// 取出整个栈链，将其时间相加，得到总执行时间
	aux = S->top;
	while (aux)
	{
		ComputeTotalTime(aux);
		aux = aux->next;
	}
}


// 恢复local函数的计时
void ResumeLocalTime(ProfState* S)
{
	ASSERT(S->top, "resume_LocalTime: stack_top null");
	StartTimer(&(S->top->MarkerFunctionLocalTime));
}


// 恢复total函数的计时
void ResumeTotalTime(ProfState* S)
{
	ProfData* aux = NULL;

	ASSERT(S->top, "resume_TotalTime: stack_top null");

	// 恢复链上所有函数的计时
	aux = S->top;
	while (aux)
	{
		StartTimer(&(aux->MarkerFunctionTotalTime));
		aux = aux->next;
	}
}


// 恢复local和total函数的计时，CallHookOut
void ResumeAllTime(ProfState* S)
{
	ASSERT(S->top, "resume_function: stack_top null");

	ResumeLocalTime(S);
	ResumeTotalTime(S);
}


// 函数返回
ProfData RetFunc(ProfState* S, int IsResume)
{
	ProfData RetData; // 返回函数的记录信息

	ASSERT(S->top, "leave_function: stack_top null");

	RetData = StackPop(S);

	ComputeTotalTime(&RetData);
	if (RetData.TotalTime < 0.000001)
		RetData.LocalTime = 0.0;
	else
		ComputeLocalTime(&RetData);

	if (IsResume) // 目前情况该值恒0
		ResumeLocalTime(S);

	return RetData;
}


