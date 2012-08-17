#pragma once
#include "ICoreSceneClientHandler.h"

namespace sqr
{
	class CCoreObjectDirector;
	class CCoreObjectFollower;
}

class CGameSceneHandler 
	: public ICoreSceneClientHandler
{
public:
	void OnObjectCreated(CCoreObjectFollower* pObject);
	void OnDirectorCreated(CCoreObjectDirector* pObject);
private:
	void CreateNpc(CCoreObjectFollower* pObj);
	void CreatePlayer(CCoreObjectFollower* pObj);
	void CreateIntObj(CCoreObjectFollower* pObj);
	void CreateTrap(CCoreObjectFollower* pObj);
	void CreateMagic(CCoreObjectFollower* pObj);
};
