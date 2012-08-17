#pragma once
#include "CActorFollower.h"

class CActorDirector
	:public CActorFollower
{
public:
	CActorDirector();
	~CActorDirector();
	void SetMoveDir(uint8 uDir);
	void TurnAndSyncDir(uint8 uDir);

private:
	uint8 m_uAccumulatedDir;
};
