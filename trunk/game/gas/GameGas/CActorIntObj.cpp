#include "stdafx.h"
#include "CActorIntObj.h"
#include "PtrlGas2GacCPPDef.h"
#include "IActorIntObjHandler.h"
#include "ISend.h"
#include "CEntityServer.h"

CActorIntObj::CActorIntObj()
{
}

CActorIntObj::~CActorIntObj()
{
}

void CActorIntObj::InitActorVariant()
{
	CSyncVariantServer* pActorVariant = GetEntity()->GetSyncVariant(eCTI_Agile)->SetMember("Actor");
	(m_pVariantActionDir = pActorVariant->SetMember("ActionDir"))->SetNumber((uint8)160);
}

IActorIntObjHandler* CActorIntObj::GetHandler() const
{ 
	return class_cast<IActorIntObjHandler*>(CActorServer::GetHandler()); 
}

CSyncVariantServer* CActorIntObj::GetVariantActionDir()
{
	return m_pVariantActionDir;
}
