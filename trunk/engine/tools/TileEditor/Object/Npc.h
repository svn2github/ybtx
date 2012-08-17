#ifndef _NPC_H_
#define _NPC_H_

#include "cbaseclass.h"

class Npc : CBaseClass
{
public:
	Npc();
	~Npc();

protected:
	float		m_fLife;
	char		m_szName[64];
	int			m_nSpeed;
	bool		m_bIsDead;
};

#endif