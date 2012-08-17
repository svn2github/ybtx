#include "stdafx.h"
#include "CActorIntObjClient.h"
#include "IActorIntObjHandler.h"
#include "CRenderObject.h"
#include "CEntityClient.h"
#include "TIntObjAdapterClient.h"
#include "IIntObjClientHandler.h"
#include "CIntObjClient.h"
#include "IFighterFollowerHandler.h"
#include "IIntObjCallbackHandler.h"
#include "IRenderObjClientHandler.h"

CActorIntObjClient::CActorIntObjClient()
{
}
CActorIntObjClient::~CActorIntObjClient()
{
}

IActorIntObjHandler* CActorIntObjClient::GetHandler()
{
	return	class_cast<IActorIntObjHandler*>(CActorClient::GetHandler());
}

CRenderObject* CActorIntObjClient::GetRenderObj()
{
	Ast(GetHandler());
	return class_cast<CIntObjClientAdapter*>(GetHandler())->GetEntity()->GetRenderObject();
}

bool CActorIntObjClient::CIntObjDoActor(const TCHAR* cActorName)
{
	return GetRenderObj()->DoAni(cActorName, true);
}

void CActorIntObjClient::CIntObjDoEffect(const TCHAR* cEffectFile, const TCHAR* cEffectName)
{
	GetRenderObj()->AddEffect(cEffectFile, cEffectName, -1, NULL);
}

void CActorIntObjClient::CIntObjSetDir(uint8 uActionDir)
{
	//SetActionDir(uActionDir);
	CDir dir;
	dir.uDir = uActionDir;
	GetRenderObj()->SetDirection(dir);
}

void CActorIntObjClient::OnAnimationEnd(const TCHAR* szName)
{
	Ast(GetHandler());
	CIntObjClient* IntObj = class_cast<CIntObjClient*>(class_cast<CIntObjClientAdapter*>(GetHandler())->GetEntity());
	IntObj->GetCallbackHandler()->OnAnimationEnd(IntObj,szName);
}