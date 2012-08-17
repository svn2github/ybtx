#pragma once
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CMagicOpTreeServer;

class CCfgRelationDeal : public CConfigMallocObject
{
	typedef set<CMagicOpTreeServer*, less<CMagicOpTreeServer*>, TConfigAllocator<CMagicOpTreeServer*> > SetMagicOpTreeServer;
public:
	static bool						RelationDeal();
	static void						AddMopTreeForSync(CMagicOpTreeServer* pMagicOpTree);

	enum EObjPersistentType
	{
		eOPT_Inexistence,
		eOPT_Inpersistent,
		eOPT_Persistent
	};

	typedef map<string, EObjPersistentType, less<string>, 
		TConfigAllocator<pair<string, EObjPersistentType > > >			MapPingPongObjPersistent;
	static MapPingPongObjPersistent	m_mapPingPongObjPersistent;


private:
	static void						SetStateNeedSyncInArg(CMagicOpTreeServer* pMagicOpTree);
	static void						SetStateNeedSyncInArgCalc(CMagicOpTreeServer* pMagicOpTree);

	static SetMagicOpTreeServer		m_setMopTreeForSyncStateInArg;
	static SetMagicOpTreeServer		m_setMopTreeForSyncStateInArgCalc;

	static bool						ResetMap();
};

