#include "stdafx.h"
#include "CSoundCDTimeMgr.h"
#include "CAppClient.h"
#include "IAudioModule.h"

CSoundCDTimeMgr::MapSoundWithCD	CSoundCDTimeMgr::ms_mapSoundWithCD;

void CSoundCDTimeMgr::PlaySound(const char* soundName, uint32 uCDTime)
{
	MapSoundWithCD::iterator it = ms_mapSoundWithCD.find(soundName);
	if (it == ms_mapSoundWithCD.end())
	{
		PlayCue(soundName);
		CSoundCDTick* pCSoundCDTick = new CSoundCDTick(uCDTime);
		ms_mapSoundWithCD.insert(make_pair(soundName, pCSoundCDTick));
	}
	else
	{
		if (!it->second->IsCoolDwon())
		{
			PlayCue(soundName);
			it->second->EnterCoolDown(uCDTime);
		}
	}
}

void CSoundCDTimeMgr::OnDestroy()
{
	ClearMap(ms_mapSoundWithCD);
}

CSoundCDTick::CSoundCDTick(uint32 uCDTime)
: m_bCoolDown(true)
{
	CAppClient::Inst()->RegisterTick(this, uCDTime);
}

CSoundCDTick::~CSoundCDTick()
{
	CAppClient::Inst()->UnRegisterTick(this);
}

void CSoundCDTick::EnterCoolDown(uint32 uCDTime)
{
	m_bCoolDown = true;
	CAppClient::Inst()->RegisterTick(this, uCDTime);
}

bool CSoundCDTick::IsCoolDwon()
{
	return m_bCoolDown;
}

void CSoundCDTick::OnTick()
{
	m_bCoolDown = false;
}
