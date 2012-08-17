#include "stdafx.h"
#include "Npc.h"

BEGIN_CLASS(Npc)	
	ADD_REALPROP_FLAG	(Life,				1.0,		0)
	ADD_BOOLPROP_FLAG	(IsDead,			false,		0)
	ADD_STRINGPROP_FLAG	(m_szName,			"",			PF_FILENAME)	
	ADD_LONGINTPROP_FLAG(m_nSpeed,			0,			0)
END_CLASS(Npc, CBaseClass, CF_ATTACHTOMAP|CF_MAXCACHE, 256)


Npc::Npc()
{
	m_fLife			= 100.0f;
	strcpy(m_szName, "Npc");
	m_nSpeed = 2;
	m_bIsDead = true;
}

Npc::~Npc()
{
	m_fLife			= 100.0f;
	strcpy(m_szName, "Npc");
	m_nSpeed = 2;
	m_bIsDead = true;
}