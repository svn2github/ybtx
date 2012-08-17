#pragma once


#include "BaseTypes.h"

using namespace sqr;


// 将Uint8转为Base16, pBuf为空返回装载TCHAR的字符个数
size_t Uint82Base16( const uint8* pUint8, size_t stSrcLen, char* pBase16, size_t sizeBuf );


