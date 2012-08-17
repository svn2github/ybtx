#pragma once
#include "CFighterMallocObject.h"
#include "TFighterAllocator.h"

typedef map<double, uint32, less<double>, TFighterAllocator<pair<double, uint32> > > MapSpeedChange;

class CSpeedChangeMgr
	:public CFighterMallocObject
{
public:
	CSpeedChangeMgr()
	{
		InitChange();
	}
	void InitChange();
	void AddChange(double& fChange, bool bCascade = false);
	void DecChange(double& fChange, bool bCascade = false);

private:
	MapSpeedChange					m_mapSpeedDownChange;					
	MapSpeedChange					m_mapSpeedUpChange;					
};
