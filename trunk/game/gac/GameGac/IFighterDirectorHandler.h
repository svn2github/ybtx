#pragma once
#include "IFighterFollowerHandler.h"
#include "FindPathDefs.h"
#include "CVector3.h"
#include "CoreObjectDefs.h"

namespace sqr
{
	class CCoreSceneClient;
}

class CFighterFollower;

class IFighterDirectorHandler
	:public IFighterFollowerHandler
{
public:
	virtual ~IFighterDirectorHandler(){}
	virtual IFighterDirectorHandler* CastToFighterDirectorHandler()const	{return const_cast<IFighterDirectorHandler*>(this);}

	virtual bool IsPassive()=0; 
	virtual bool IsMoving()=0;
	virtual CFighterFollower* GetSelectedFighter()const=0;
	virtual CCoreSceneClient* GetScene()const=0;
};
