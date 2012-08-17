#include "stdafx.h"
#include "CObjMoveStateServer.h"
#include "CSyncTimeSystemServer.h"
#include "CSyncCoreObjectServer.h"
#include "TObjMoveState.inl"

template class TObjMoveState<CSyncTraitsServer>;

CObjMoveStateServer::CObjMoveStateServer(CSyncCoreObjectServer* pObj,CPixelPath* pPath,float fSpeed,float fBeginDist)
:Parent_t(pObj,pPath,fSpeed,fBeginDist)
{
}

CObjMoveStateServer::~CObjMoveStateServer(void)
{
	DeleteTickingDerived();
}

void CObjMoveStateServer::SetMoveCycDerived(uint32 uMoveCyc)
{
	m_uMoveCyc = uMoveCyc;
}

uint32 CObjMoveStateServer::GetMoveCycDerived()const
{
	return m_uMoveCyc;
}

void CObjMoveStateServer::CreateTickingDerived(uint32 uCyc)
{
	//CSyncTimeSystemServer::Inst()->Register( this, uCyc );
	GetObject()->GetDistortedTime()->RegistDistortedTick(this, uCyc);
}

void CObjMoveStateServer::DeleteTickingDerived()
{
	//CSyncTimeSystemServer::Inst()->UnRegister(this);
	GetObject()->GetDistortedTime()->UnregistDistortedTick(this);
}

