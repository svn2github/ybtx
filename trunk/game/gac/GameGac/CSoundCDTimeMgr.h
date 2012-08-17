#pragma once
#include "CTick.h"

class CSoundCDTick : public CTick
{
public:
	CSoundCDTick(uint32 uCDTime);
	~CSoundCDTick();

	void EnterCoolDown(uint32 uCDTime);
	bool IsCoolDwon();

private:
	void OnTick();

	bool m_bCoolDown;
};

class CSoundCDTimeMgr
{
public:
	static void PlaySound(const char* soundName, uint32 uCDTime);
	static void OnDestroy();

private:
	typedef map<string, CSoundCDTick*> MapSoundWithCD;
	static MapSoundWithCD ms_mapSoundWithCD;
};
