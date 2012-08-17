#include "stdafx.h"
#include "CFighterStress.h"
#include "PtrlGac2GasCPPDef.h"
#include "CConnStress.h"
#include "CCoreObjectStress.h"
#include "CPos.h"
CFighterStress* CFighterStress::InitFighterStress(CConnStress* pConn)
{
	CFighterStress* pFighterStress=new CFighterStress;
	//pFighterStress->m_uGlobalID=CCoreObjectStress::GetGlobalID(pConn);
	pFighterStress->m_pConn=pConn;
	return pFighterStress;
}

void CFighterStress::RemoveFighterStress()
{
	delete this;
}

void CFighterStress::DoNormalSkill( uint32 uSkillID, uint8 uSkillLevel, float uPosFromX, float uPosFromY, uint32 uTargetID)
{
	//CGac2GasCPP_LockObj cmdLockObj;
	//cmdLockObj.uLockObjId = uTargetID;
	//SendToGas(&cmdLockObj);

	CFPos Pos;
	Pos.x = uPosFromX;
	Pos.y = uPosFromY;

	CGac2GasCPP_DoSkill cmd;
	cmd.uqbSkillID = uSkillID;
	cmd.uSkillLevel = uSkillLevel;
	cmd.fPosFrom= Pos;
	SendToGas(&cmd);
}

template<typename CmdType>
inline bool CFighterStress::SendToGas(const CmdType* pCmd)const
{
	m_pConn->SendCmd(pCmd);
	return true;
}
