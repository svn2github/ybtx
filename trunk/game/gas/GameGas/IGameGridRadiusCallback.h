#pragma once

#include "CDynamicObject.h"
#include "CallBackX.h"

namespace sqr
{
	class CCoreSceneServer;
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
}

class CEntityServer;
class IGameGridRadiusCallback
	:public virtual CDynamicObject
{
public:
	void ForEachInRadius(const CCoreSceneServer* pScene, const CPos& GridPos, float fRadius);
	void ForEachMediatorInRadius(const CCoreSceneServer* pScene, const CPos& GridPos, float fRadius);
	virtual void Exec(CEntityServer* pEntityServer) {
		CALL_CLASS_CALLBACK_1(pEntityServer)
	}
};
