#pragma once


class IBarrierInfo
{
public:
	virtual	inline uint32 GetExactWidth() = 0;
	virtual inline uint32 GetExactHeight() = 0;
	virtual inline int GetBarrierType(int x, int y) = 0;
};