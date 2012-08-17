#pragma once

#include "ThreadTypes.h"

namespace sqr
{

class CAStarData;

class CAStarDataMgr
{
public:
	typedef vector<CAStarData*>		VecAStarData_t;

	static CAStarDataMgr& Inst();
	~CAStarDataMgr();
	
	CAStarData* GetAStarData();
	void UpdateSize(uint32 nWidth, uint32 nHeight);
	void UpdateAllNodeNum(uint32 nAllNodeNum);

	
	uint32 GetCurWidth();
	uint32 GetCurHeight();

	uint32 GetAllNodeNum();
protected:
	uint32 m_nWidth;
	uint32 m_nHeight;

	uint32 m_nAllNodeNum;
	VecAStarData_t m_vecAStarData;
	HTLSKEY	ms_AStarDataKey;
private:
	CAStarDataMgr();
};

}


