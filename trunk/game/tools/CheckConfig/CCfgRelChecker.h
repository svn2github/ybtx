#pragma once

class CCfgRelChecker
{
public:
	static bool						CheckRelationAvail();

	enum EObjPersistentType
	{
		eOPT_Inexistence,
		eOPT_Inpersistent,
		eOPT_Persistent
	};

	typedef map<string, EObjPersistentType>		MapPingPongObjPersistent;
	static MapPingPongObjPersistent	ms_mapPingPongObjPersistent;


private:
	static bool						ResetMap();
};
