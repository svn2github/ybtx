#pragma once
#include "CDistortedTick.h"
#include "TMagicStateAllocator.h"

class CFighterDictator;
class CBaseStateCfgServer;

class CSizeChangeMgr
	: public CDistortedTick
	, public CMagicStateMallocObject
{
public:
	CSizeChangeMgr(CFighterDictator* pOwner);
	~CSizeChangeMgr();
	void ChangeSizeRate(CBaseStateCfgServer* pCfg, int32 iChangeRateCount);
	void OnTick();
	double GetStateZoomRate();

private:
	double m_dFinalScale;
	double m_dCurScale;
	uint32 m_uScaleMaxCount;
	uint32 m_uScaleCurCount;

	CFighterDictator* m_pOwner;
};

//在控制台打印AOI SIZE和BARRIER SIZE随状态模型大小变化的信息
//#define COUT_SIZE_CHANGE_INFO

