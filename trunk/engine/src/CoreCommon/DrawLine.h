#pragma once


#include "MathAlgorithm.h"
#include "CFindPathMallocObject.h"

namespace sqr
{
class CBarrierManager;
class CMapInfoManager;

class DrawLine
	:public ILineAction
	,public CFindPathMallocObject
{
public:
	DrawLine(CBarrierManager* pBarrierMgr, CMapInfoManager* pMapInfo);
	~DrawLine();
	bool Do( int32 x, int32 y );
	int32 GetLastX();
	int32 GetLastY();

private:
	int32 m_nX;
	int32 m_nY;  // 保存直线查询的最后两个坐标，这样在最后碰到障碍的时候能够返回前一个有效坐标点
	CBarrierManager*   m_BarrierMgr;
	CMapInfoManager*   m_MapInfo;
};

}

