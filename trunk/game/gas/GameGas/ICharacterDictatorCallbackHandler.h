#pragma once

#include "PatternRef.h"
#include "CStaticObject.h"
#include "FightDef.h"
#include "GameDef.h"
#include "CEntityMallocObject.h"

class ICharacterMediatorCallbackHandler;
class CCharacterDictator;
class CCharacterMediator;

namespace sqr
{
	class CCoreSceneServer;
}

class ICharacterDictatorCallbackHandler 
	: public virtual CStaticObject
	, public CEntityMallocObject
{
public:
	ICharacterDictatorCallbackHandler(){m_RefByCharacter.SetHolder(this);}

	virtual ~ICharacterDictatorCallbackHandler(){}

	typedef TPtRefee<ICharacterDictatorCallbackHandler,CCharacterDictator> RefCallbackHandlerByCharacter;

	RefCallbackHandlerByCharacter& GetRefByCharacter(){return m_RefByCharacter;}

	virtual ICharacterMediatorCallbackHandler* CastToMediatorHandler() { return NULL; }

	virtual void OnCreateServant( uint32 uObjID,const char* szServantName,uint32 uLevel,float fPosX, float fPosY,bool bNormalAI) ;

	virtual void OnCreateServantWithTarget( uint32 uObjID,const char* szServantName,uint32 uLevel, uint32 uTargetID,float fPosX, float fPosY,bool bNormalAI) ;

	virtual void OnCreateBattleHorse( uint32 uObjID, const char* szServantName, const char* szServantType, uint32 uLevel, float fPosX, float fPosY);

	virtual void OnCreateNPCWithTarget( uint32 uObjID, uint32 uTargetID, uint32 uMasterID, const char* szNPCName,
		uint32 uLevel,float fPosX,float fPosY) ;

	virtual void OnCreateDeadNPC( uint32 uObjID, uint32 uLevel, float fPosX, float fPosY) ;

	virtual void OnDestroyNpc(CCharacterDictator* pObject);

	virtual void OnCreateShadow( uint32 uObjID,const char* szServantName,uint32 uLevel,
		float fPosX,float fPosY,bool bNormalAI) ;

	//注册给lua用的回
	virtual void OnNPCDead(CCharacterDictator* self, uint32 uExpOwnerID, uint32 KillerID, bool bNormalDead);

	//npc死亡之后通知任务系统
	virtual void OnDeadToQuest(CCharacterDictator* self,const char* szSkillName,uint32 uKillerID,uint32 uExpOwnerID);
	
	//npc死亡之后 服务端重生的实现 就放在这个里面 又因为 现已经有了一堆OnDead() 所以起名为 BeDead()
	virtual void OnDead(CCharacterDictator* self,const char* szSkillName,uint32 uKillerID,uint32 uExpOwnerID, bool bNormalDead);

	virtual void OnDeadByTick(CCharacterDictator* pObject);

	virtual void OnLeaveBattleState(CCharacterDictator* pObject);


	virtual void OnEnterBattleState(CCharacterDictator* pObject);

	virtual void OnDisappear(CCharacterDictator* self, bool bNotReborn);
	
	//表情响应
	virtual bool OnReaction2Facial(uint32 uEntityID, const char* uActionName, uint32 uPlayerEntityID);
		
	virtual bool DoFacialReaction(uint32 uEntityID, const char* uActionName, uint32 uPlayerEntityID);


	//个性数据
	virtual bool HaveNature(uint32 uEntityID) ;

	virtual void InitNatureData(uint32 uEntityID);

	//用于在个性中播放动作
	virtual void DoNatureAction(uint32 uEntityID, const char* szActionName);

	// 下面代码专为剧场功能实现，by : super_xiang
	virtual void SendNpcRpcMessage(uint32 uObjID, uint32 NpcId, uint32 uMessageId);

	virtual void MoveEndMessage(CCharacterDictator* pObject) ;

	virtual void ChangeDir(uint32 NpcId, uint32 uX, uint32 uY) ;

	virtual void ReplaceNpc(CCharacterDictator* pServant, CCharacterDictator* pMaster, const char* sAIName, const char* sNpcType, const char* szServantRealName, ECamp eCamp) ;
	
	//为赶小猪实现在函数
	virtual void SetDrivePig(uint32 uObjId, uint32 uEntityId);

	virtual void DestoryObj(uint32 ObjId);

	//设置Npc的影身状态
	virtual void SetNpcAlphaValue(uint32 uEntityID, uint8 cValue);

	virtual void SetNpcCanBeTakeOver(uint32 uEntityID, ENpcAIType eNpcAIType, ENpcType eNpcType, ECamp eCamp);

	//召唤兽给主角上技能的两个接口
	virtual void CreateServantSkillToMaster(CCharacterDictator* pMaster, const char* sServantName);
	virtual void DelServantSkillFromMaster(CCharacterDictator* pMaster, const char* sServantName);

	//召唤兽小弟让主人Npc复活自己
	virtual void MasterReBornCortege(uint32 uMasterID, uint32 uCortegeID);

	//召唤兽主人恢复自己的小弟
	virtual void MasterRevertNpcGroup(uint32 uMasterID);
	
	//Npc释放技能同时说话
	virtual void NpcShowContentBySkill(CCharacterDictator* pNpc, const char* sSkillName, uint32 uTargetID);

	virtual void BecomeTargetShadow(CCharacterDictator* pCharacter, CCharacterDictator* pTarget);

	virtual void TakeOverTruckByNewMaster(CCharacterDictator* pTruck, CCharacterDictator* pNewMaster);

	virtual void ClearNpcAlertEffect( CCharacterDictator* pAlertNpc);

	virtual void ReplaceTruck(CCharacterDictator* pOldTruck, CCharacterDictator* pNewTruck);

	virtual void PosCreateNpcWithMaster(uint32 uMasterID, const TCHAR* szNPCName, float fPosX, float fPosY);
private:
	RefCallbackHandlerByCharacter	m_RefByCharacter;
};
