#pragma once

class IFighterDirectorHandler;
class CCharacterFollower;
class CEntityClient;

class IFighterFollowerHandler 
{
public:
	virtual ~IFighterFollowerHandler(){}
	virtual IFighterFollowerHandler* CastToFighterFollowerHandler()const	{return const_cast<IFighterFollowerHandler*>(this);}
	virtual IFighterDirectorHandler* CastToFighterDirectorHandler()const	{return NULL;}

	virtual CCharacterFollower* GetCharacter() const=0;
	virtual uint32 GetEntityID()const=0;
	virtual CEntityClient* GetEntity() const = 0;
};
