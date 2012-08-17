#pragma once
#include "CActorClient.h"

namespace sqr
{
	class CRenderObject;
}

class IActorIntObjHandler;

class CActorIntObjClient
	:public CActorClient
{
public:
	CActorIntObjClient();
	~CActorIntObjClient();
	bool CIntObjDoActor(const TCHAR* cActorName);
	void CIntObjDoEffect(const TCHAR* cEffectFile, const TCHAR* cEffectName);
	void CIntObjSetDir(uint8 uActionDir);
	void OnAnimationEnd(const TCHAR* szName);
private:
	IActorIntObjHandler* GetHandler();
	CRenderObject* GetRenderObj();
};
