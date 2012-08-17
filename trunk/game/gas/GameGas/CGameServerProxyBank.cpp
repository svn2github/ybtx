#include "stdafx.h"
#include "CGameServerProxyBank.h"
#include "PtRefProxy.inl"
#include "TSqrAllocator.inl"
#include "CEnmityMgr.h"
#include "CNpcAI.h"
#include "CNpcAINatureMgr.h"
#include "CNpcEventMsg.h"
#include "INpcAIHandler.h"
#include "CCharacterDictator.h"
#include "CSkillRuleDoSkillMoveMgr.h"
#include "CSkillRule.h"
#include "CSkillRuleMgr.h"
#include "StateMachineCheckValid.h"
#include "CNpcAlertMgr.h"
#include "IIntObjCallbackHandlerServer.h"
#include "CIntObjServer.h"
#include "CFighterDictator.h"
#include "CMagicServer.h"
#include "CCastingProcess.h"
#include "IFighterServerHandler.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CNpcGroupMgr.h"

void InitGameServerProxyBank()
{
	TPtProxyBankMgr<CEnmityMgr,CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CNpcNatureMgr,CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CNpcAI, CNpcEventMsg>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<INpcAIHandler, CCharacterDictator>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CSkillRuleDoSkillMoveMgr, CSkillRule>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CSkillRuleMgr, CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<StateMachineCheckValid, CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CNpcAlertMgr, CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<IIntObjCallbackHandlerServer,CIntObjServer>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CFighterDictator, CMagicServer>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CFighterDictator, CCastingProcess>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CFighterDictator, CFighterDictator>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<IFighterServerHandler,CFighterDictator>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<ICharacterDictatorCallbackHandler,CCharacterDictator>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CIntObjServer>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CNpcAI>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CNpcGroupMgr>::Inst()->CreateProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CCharacterDictator>::Inst()->CreateProxyBank();
}

void UnitGameServerProxyBank()
{
	TPtProxyBankMgr<CEnmityMgr,CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CNpcNatureMgr,CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CNpcAI, CNpcEventMsg>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<INpcAIHandler, CCharacterDictator>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CSkillRuleDoSkillMoveMgr, CSkillRule>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CSkillRuleMgr, CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<StateMachineCheckValid, CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CNpcAlertMgr, CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<IIntObjCallbackHandlerServer,CIntObjServer>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CFighterDictator, CMagicServer>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CFighterDictator, CCastingProcess>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CFighterDictator, CFighterDictator>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<IFighterServerHandler,CFighterDictator>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<ICharacterDictatorCallbackHandler,CCharacterDictator>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CIntObjServer>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CNpcAI>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CNpcGroupMgr>::Inst()->DestroyProxyBank();
	TPtProxyBankMgr<CCharacterDictator, CCharacterDictator>::Inst()->DestroyProxyBank();
}

