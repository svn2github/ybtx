#include "stdafx.h"
#include "SQRWndEx.h"

SQRWndEx::SQRWndEx()
{

}

SQRWndEx::~SQRWndEx()
{

}

void SQRWndEx::OnLButtonDown(uint32 nFlags, int32 x, int32 y )
{
	SendCtrlMsg(CILCK_WNDER, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
}
