#pragma once

class ITeamServerManagerHandler
{
public:
	virtual ~ITeamServerManagerHandler(){}
	virtual void OnAddTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID){}
	virtual void OnRemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID){}
	virtual void OnCreateTeam(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID){}
	virtual void OnDestroyTeam(uint32 uTeamID,ETeamType eTeamType){}
	virtual void OnChangeTeamMaster(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID){}
};

