
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include "BaseTypes.h"
#include "stack.h"


// ³õÊ¼»¯Õ»
ProfState* InitStack(void)
{
	ProfState* S = NULL;
	S = (ProfState*)malloc(sizeof(ProfState));
	if (S)
	{
		S->level = 0;
		S->top = NULL;
		return S;
	}

	return NULL;
}

void StackPush(ProfState* S, ProfData r)
{
	ProfData* q = NULL;
	q = (ProfData*)malloc(sizeof(ProfData));
	*q = r;
	q->next = S->top;
	S->top = q;
}


ProfData StackPop(ProfState* S)
{
	ProfData r;
	ProfData* q;

	r = *(S->top);
	q = S->top;
	S->top = S->top->next;
	free(q);

	return r;
}


ProfData* StackGet(ProfState* S, int idx)
{
	ProfData* p = S->top;

	int i;
	for (i=0; i<idx; i++)
		p = p->next;

	return p;
}


