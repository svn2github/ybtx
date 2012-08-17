#pragma once
#include "IActorClientHandler.h"

class CCharacterFollower;

class IActorFollowerHandler :public IActorClientHandler
{
public:
	IActorFollowerHandler(){}

	virtual ~IActorFollowerHandler(){}
	virtual CCharacterFollower* GetCharacter() const = 0;
};
