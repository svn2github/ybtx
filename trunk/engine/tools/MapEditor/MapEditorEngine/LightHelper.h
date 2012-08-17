#pragma once
#include "CColor.h"

inline CColor ToCColor(COLORREF color)
{
	return CColor(255,GetRValue(color), GetGValue(color), GetBValue(color));
}