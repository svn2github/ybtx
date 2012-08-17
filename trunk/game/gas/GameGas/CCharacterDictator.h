#pragma once
#include "CEntityServer.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "CNpcAI.h"
#include "TEntityAllocator.h"

namespace sqr
{
	class CCoreObjectDictator;
	class CCoreObjectServer;
}

class CStateDBDataSet;
class CFighterDictator;
class INpcAIHandler;
class ICharacterDictatorCallbackHandler;
class CCharacterMediator;
class CServantServerMgr;
class CIntObjServer;
class CNpcGroupMgr;
class CNpcQueueMgr;
class CNpcChaserQueueMgr;
class CNormalSkillServerSharedPtr;

///////////////////////////////////////////////////////////////////////////////////////////////
//	CCharacterDicatator类的功能：
//	1.Character类是角色系统中唯一暴露给lua代码的类，所有Fighter和Actor上的功能都必须通过Character来封装再注册给lua
//	2.CoreObj、Fighter、Actor对象作为Character对象的成员变量，因此Character对象能够使用前面三种对象身上的所有功能.
//	从这个角度来看，Character所处的层次是最高的，权利也是最大的。但是Fighter可以通过FighterHandler来获取Character对象，
//	这样Character上的所有public函数Fighter都能使用，所以从这个角度来看的话Character的地位就有些尴尬了，对于外界来说它只是作为Fighter的
//	的转调层（上面所提到的功能1），对于Fighter来说就是下面将提到的功能3.
//	3.封装了一些和战斗系统关系不太直接的数据成员（引擎对象，仆人管理器，坐骑，etc），并提供相应的操作函数。

class CCharacterDictator : public CEntityServer
{
	typedef set<uint32, less<uint32>, TEntityAllocator<uint32> > ServantSetType;
	typedef map<uint32, CCharacterDictator*, less<uint8>, TEntityAllocator<pair<uint8, CCharacterDictator*> > > ChaserMapType;
	typedef ChaserMapType::iterator ChaserMapIter;
public:
	friend class CEntityServerCreator;
	CCharacterDictator(CCoreObjectDictator *pCoreObj);

	TPtRefee<CCharacterDictator, CIntObjServer>& GetRefsByIntObj(){return m_RefsByIntObj;}
	TPtRefee<CCharacterDictator, CNpcAI>& GetRefByNpcAI() {return m_RefsByNpcAI;}
	TPtRefee<CCharacterDictator, CNpcGroupMgr>& GetRefByGroupMgr() {return m_RefsByGroupMgr;}

	void TheaterMoveToGird(const CPos& gPos, bool bWalk);
	void NpcMoveToPixel(const CFPos& fPos, bool bWalk);
	void NpcMoveInPath();
	bool	IsBattleHorse()const;	//是否是召唤兽坐骑
	bool	IsServant()const;
	CCharacterDictator* GetBattleHorse()const;
	virtual EGameEntityType GetGameEntityType()const { return eGET_CharacterDictator; }
	virtual CCharacterMediator* CastToCharacterMediator() { return NULL; }
	static CCharacterDictator* GetCharacterByCoreObj(CCoreObjectServer* pCoreObj);
	static CCharacterDictator* GetCharacterByID(uint32 uEntityID);
	void AddSkill(const TCHAR* szSkillName,uint8 uSkillLevel);
	bool IsSkillActive(const TCHAR* szSkillName);
	void RemoveSkill(const TCHAR*  szSkillName);
	void ClearAllSkill();
	const string& GetHorseName() const					{ return m_strHorseName; }
	void SetHorseName(const string& strHorseName)		{ m_strHorseName = strHorseName; }
	const string& GetRealName() const					{ return m_strRealName; }
	void SetRealName(const char* szRealName);
	CCharacterDictator*	GetMaster();
	CCharacterDictator* GetGroupMaster();
	void SetMaster(CCharacterDictator* pMaster);
	CServantServerMgr*	GetServantMgr()const{return m_pServantMgr;}
	void AddServant(CCharacterDictator* pServant);
	void DestroyServantMgr();
	bool IsMasterHaveServant(ENpcType eType);
	void DestoryServant(ENpcType eType);

	//以下为拆分出来的NPC群组管理器，解决与召唤兽管理器混杂在一起的问题
	CNpcGroupMgr* GetGroupMgr()const{return m_pGroupMgr;}
	void SetGroupLeader(CCharacterDictator* pLeader);
	void AddGroupMember(CCharacterDictator* pMember);
	void RemoveGroupMember(CCharacterDictator* pMember);
	void RemoveAllGroupMember();
	void DestroyGroupMgr();
	bool AdjustIsGroupMember(uint32 uGlobalID);
	CCharacterDictator*	GetGroupLeader();
	void GetGroupMemberRebornPosition(uint32 uMemID,CFPos &pos);
	bool IsInitMemberPathPoint(uint32 uMemID);

	//强制从召唤兽管理器中删除召唤兽，同时干掉这个召唤兽
	void RemoveServant( CCharacterDictator* pServant );
	//只从主角的召唤兽管理器删除，不删除游戏实体，主要用于切换AI
	void RemoveServantFromServantMgr( uint32 uEntityID);	
	CCharacterDictator* GetServantByID( uint32 uGlobalID ) const;
	CCharacterDictator* GetServantByType( ENpcType eType ) const;
	void ClearServantType(ENpcType eNpcType);
	void ClearAllServant();
	void ClearAllServantByMasterDie();
	//这个函数是在主人死亡或者下线后通知自己召唤兽做相应操作的函数
	void OnMasterDestroyed();

	void SetTeamID(uint32 uTeamID)const;
	uint32 GetTeamID() const;
	void SetTroopID(uint32 uTroopID)const;
	uint32 GetTroopID() const;
	void SetTongID(uint32 uTongID) const;
	uint32 GetTongID() const;
	uint32 GetMasterTongID() const;
	void OnMoveBegan();
	void OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID);
	void OnCoreEvent(const void* pCoreEvent);
	
	// Actor
	bool SetAndSyncMoveState(EMoveState eMoveState, bool bForce = false);
	void SetAndSyncMoveDir(uint8 uMoveDir);
	void SetAndSyncActionState(EActionState eActionState, float fNARhythm, EHurtResult eHurtResult = eHR_Hit);
	void SetAndSyncActionState(EActionState eActionState) { SetAndSyncActionState(eActionState, 0.0f); }
	void SetAndSyncActionDir(uint8 uDir, bool bIgnoreForbitTurn, bool bDirGac2Gas = false);
	void SetAndSyncActionDir(uint8 uDir) { SetAndSyncActionDir(uDir, false); }
	void SetAndSyncActionDirByPos(CFPos pos, bool bIgnoreForbitTurn = false);
	void SetWalkBackRadius(uint8 WalkBackRadius);
	EMoveState GetMoveState() const;
	uint8 GetMoveDir()const;
	EActionState GetActionState() const;
	uint32 GetActionDir()const;
	void SetNpcBirthDir(uint8 uDir);
	uint8 GetWalkBackRadius() const;
	void OnChangeWFAniKeyFrame(const TCHAR* szAniFileName);
	uint32 GetKeyFrameNumOfTakeAni();
	uint32 GetKeyFrameNumOfFireAni();
	uint32 GetEndFrameNumOfFireAni();
	CCharacterDictator* GetTarget();
	ESetPosResult SetGridPosByTransport(const CPos& GridPos);
	virtual ESetPosResult SetPixelPosByTransport(const CFPos& PixelPos);

	ICharacterDictatorCallbackHandler* GetCallbackHandler() const;
	void SetCallbackHandler( ICharacterDictatorCallbackHandler* pCallbackHandler);
	INpcAIHandler* GetNpcAIHandler() const;
	void SetNpcAIHandler( INpcAIHandler* pNpcAIHandler);
	CActorServer* GetActor() const { return CEntityServer::GetActor(); }
	CFighterDictator* GetFighter() const { return m_pFighter; }
	void CreateEnded();

	// 注册给lua用的
	void	CppInit(EClass eClass, uint32 uLevel, ECamp eCamp);
	bool CppGetCtrlState(EFighterCtrlState eState)const;
	bool CppSetCtrlState(EFighterCtrlState eState,bool bSet);
	bool CppSetCtrlStateForLua(EFighterCtrlState eState,bool bSet);
	void CppSetDoSkillCtrlStateForLua(EDoSkillCtrlState eState,bool bSet);
	bool CppGetDoSkillCtrlState(EDoSkillCtrlState eState);
	void CppReborn();
	EClass CppGetClass()const;
	void CppSetClass(EClass eClass);
	uint32 CppGetLevel()const;
	void CppSetLevel(uint32 uLevel);
	ECamp CppGetBirthCamp()const;
	void CppSetBirthCamp(ECamp eCamp);
	ECamp CppGetCamp()const;
	void CppSetCamp(ECamp eCamp);
	void CppSetGameCamp(int32 iGameCamp);
	int32 CppGetGameCamp();
	void ReSetGameCamp();
	bool CppGetPKState() const;
	void CppSetPKState(bool bPKState);
	//EPKState CppGetPKState() const;
	//void CppSetPKState(EPKState ePKState);
	//EZoneType CppGetZoneType() const;
	//void CppSetZoneType(EZoneType eZoneType);
	void GM_CppSetPropertyValueByName(const TCHAR* szName, float fValue);
	void CppChangePropertyValueByName(const TCHAR* szName, float fValue);
	void CppSetHP(uint32 uHp);
	float CppGetHP();
	float CppGetMP();
	float CppGetMaxHP();
	float CppGetMaxMP();
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnGroupDestroyed();
	virtual void OnEnterBattleState();
	virtual void OnLeaveBattleState();
	virtual void FacialActionHappened(uint32 uPlayerID, const TCHAR* szFacialName);
	virtual void SetNatureInfo(const TCHAR* szNatureName, const TCHAR* szNatureArg, uint32 uNatureExtraArg, const TCHAR* NatureAction, uint32 uColdTime, const TCHAR* szAction, const TCHAR* szActionArg, uint32 uLevel);
	virtual void OnSendWarningEvent();
	virtual void OnSendChangeModelEvent();
	virtual void OnSendExitSubMachineEvent();
	void OnAgileValueChanged(uint32 uValueTypeID, float fValue) const;
	void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const;
	void OnDoSkillCtrlStateChanged(uint32 uStateID, bool bSet)const;
	void OnSkillBegin()const;
	void OnSkillEnd(bool bSucceed)const;
	inline ENpcType GetNpcType() const { return m_eNpcType; }
	void SetNpcType(ENpcType eNpcType);

	bool SetCtrlState(EFighterCtrlState eState,bool bSet, CSkillInstServer* pSkillInst);

	void SetOnDead(bool bNormalDead = false);
	void KillNpcByLifeTimeEnd();	//专门提供给生存期结束时候使用
	void KillNpcBySpecialAction(CCharacterDictator* pAttacker, const TCHAR* sActionName);
	void SetOnDisappear(bool bNotReborn);
	void SetNpcDeadNormal();

	bool CppIsLive();
	void DoPosNPCSkill(const TCHAR* sName, const CFPos& Pos);
	void DoPosNPCSkill(const CNormalSkillServerSharedPtr* pCfg, const CFPos& Pos);
	void DoNPCSkill(const TCHAR* sName, CEntityServer* pTarget);
	void DoNPCSkill(const CNormalSkillServerSharedPtr* pCfg, CEntityServer* pTarget);
	void ServerDoNoRuleSkill(const TCHAR* sName, CEntityServer* pTarget); //用于无技能规则的NPC施放技能
	void ClearState(const TCHAR* sName);
	bool EraseErasableState(const TCHAR* sName);
	bool ExistState(const TCHAR* sName);
	uint32 GetStateCascade(const TCHAR* sName);
	bool IsSingTypeSkill(const TCHAR* sName);
	uint32 GetSkillCastAniFrameCnt(const TCHAR* szSkillName);
	bool ExistBirthAniFrame();
	uint32 GetNpcBirthAniFrameCnt();
	bool ExistSpecialActionAni();
	uint32 GetSpecialActionAniCnt();
	virtual void Release();
	void ForceLock(CCharacterDictator* pTarget, uint32 uChaosTime = 10000);
	void SetDefaultWalkState(bool bWalkState);
	void SetMoveRange(uint32 range);
	void MoveToDest(uint32 uX,uint32 uY);
	void MoveToPathPoint(uint8 point);
	void SetMovePath(uint8 uPoint, float x, float y);
	void SetMoveType(uint8 uMoveType);
	uint32 GetOneActorAround();	//获取NPC周围距离最近的一个玩家
	uint32 GetOneNpcAround();	//获取Npc周围随机一个NPC
	uint32 GetRandomOneAround();//或取NPC周围任意一个对象（玩家或者NPC）
	uint32 GetObjectInArea();
	uint32 GetExpOwnerID();
	void SetTarget(uint32 uTargetId);
	void OnBeAttacked(uint32 uTarget);
	void OnBeginRandomMove();
	void ReSetNpcExistTick(uint32 uExitTime);
	uint32 GetNpcExistTime();
	int32 GetNpcLeftLifeTime();
	EAttackType GetNpcAttackType();	
	void AddChaser(uint32);
	void DelChaser(uint32);
	void ClearAllChaser();
	uint32 GetChaserTargetNum() {return m_MapChaser.size();}
	void GetRegulateNpcPlace(uint32 uEntityID, CFPos& Pos);
	void TalkWithPlayerStart(uint32 uEntityID);
	void TalkWithPlayerEnd();
	void OnBeHurt(int32 iHpChange);
	void SetGroupMemberID(uint32 id);																//通知NPC朝向改变																	//通知NPC目标开始移动																	//通知NPC目标停止
	void OnChangeSpeedBySkill();																	//技能系统通知NPC减速，更新Trace速度
	virtual void OnTraceEnded(ETraceEndedType eType);											//Trace结果回掉
	void MasterAttackCommand(ENpcType eType);
	void MasterRetreatCommand(ENpcType eType);
	void MasterDisbandCommand(ENpcType eType);
	void MasterSpreadCommand(ENpcType eType);
	bool MasterOrderServantMoveCommand(ENpcType eType, uint32 x, uint32 y);
	ENpcAIType GetAITypeID();
	void SetNpcAIType(ENpcAIType eAIType);
	void OnBeginDestory(bool bNormalDead = false, uint32 uExpOwnerID = 0, uint32 KillerID = 0, bool bServantChangeScene = false);
	const string& GetNpcName() const			{ return m_sNpcName; }
	void SetNpcName(const char* szNpcName)			{ m_sNpcName = szNpcName; }
	void SetNoDissolve();
	bool CanAIChange();
	bool CanBeRavin();
	bool CanBeSelected();
	void ChangeAIType(CCharacterDictator* pMaster, ENpcAIType eAIType, ENpcType eNpcType, ECamp eCamp);
	void SynToFollowCanBeTakeOver();
	void SynToFollowCanNotTakeOver();
	bool CanBeTakeOver(){ return m_bCanBeTakeOver;};
	void ReSetNpcGridAttackScope(float fScope);
	void RevertNpcAttackScope();
	bool CanDelInBattle();
	void SetCanDelInBattle(bool bCanDel);
	void SetDeadAfterBattle();
	void NpcShowContentBySkill(const TCHAR* sSkillName, uint32 uTargetID);
	//Npc队列管理器
	int32 AddMemberToQueueMgr(uint32 uEntityID);
	void RemoveMemberFromQueueMgr(uint32 uEntityID);
	void DestoryQueueMgr();
	double GetStateZoomRate();
	void CheckPlayInNpcSleepDim();
	void SetNpcSleepState(bool bSleep);
	bool BeBossType();
	CStateDBDataSet* SerializeStateToDB(uint32 uGrade);		//往数据库存状态
	void SaveStateToDBEnd();		//往数据库存状态结束
	bool LoadStateFromDB(CStateDBDataSet* pRet);			//从数据库取状态
	bool HaveBeSetPath();
	void RemoveEnemyFromServantEnmityList(CCharacterDictator* pEnemy);
	void SendNpcDestoryMsg();
	float GetNpcSize();
	void SetBeShowInClient(bool bShow);
	void SetActiveState(bool bActiveNpc);
	void SetAttackType(EAttackType eAttackType);
	void SetFightDir(bool bFaceEnemy);
	bool TruckCanBeTakeOver(CCharacterDictator* pNewMaster);
	void TakeOverTruck(CCharacterDictator* pTruck);
	void ChangeTruckState(uint8 uState);
	void SetNpcSkillLevel(uint32 uSkillLevel);
	uint32 GetNpcSkillLevel();
	void ClearNpcAlertEffect();
	void ChaseBack();
	uint32 GetNpcDoSkillLevel();
	uint32 GetDefence();
	uint32 GetNatureResistance();
	uint32 GetDestructionResistance();
	uint32 GetEvilResistance();
	void SetDoSpecialActionProbability(uint32 uProbability);
	void ClearAllEnmity();

	CSyncVariantServer* GetCampVariant() const;
	CSyncVariantServer* GetBirthCampVariant() const;
	CSyncVariantServer* GetGameCampVariant() const;
	CSyncVariantServer* GetLevelVariant() const;
	CSyncVariantServer* GetClassVariant() const;
	CSyncVariantServer* GetNpcTypeVariant()const;
	CSyncVariantServer* GetNpcAITypeVariant()const;
	CSyncVariantServer* GetBeActiveVarint() const;
	CSyncVariantServer* GetNpcAttackTypeVariant() const;
	CSyncVariantServer* GetRealNameVariant()const;
	CSyncVariantServer* GetShowInClientVariant()const;
	CSyncVariantServer* GetFightDirVariant()const;
	CSyncVariantServer* GetPKSwitchTypeVariant()const;
	CSyncVariantServer* GetZoneTypeVariant()const;
	CSyncVariantServer* GetTargetIDVariant()const;
	CSyncVariantServer* GetSpeedVarint()const;
	CSyncVariantServer* GetStealthVariant()const;
	CSyncVariantServer* GetAlertTargetIDVariant()const;
	CSyncVariantServer* GetAlertActionVariant()const;
	CSyncVariantServer* GetMasterIDVariant()const;
	CSyncVariantServer* GetMasterTeamIDVariant()const;
	CSyncVariantServer* GetMasterTroopIDVariant() const;
	CSyncVariantServer* GetMasterTongIDVariant() const;
	CSyncVariantServer*	GetExpOwnerIDVariant() const;
protected:
	CCharacterDictator();
	virtual ~CCharacterDictator();
	template<typename EntityTraits>
	void Init(CCoreObjectDictator *pCoreObj);
	void InitNpcVariant();
	void InitPlayerVariant();
private:
	ENpcType			m_eNpcType;		
	string				m_strHorseName;
	string				m_strRealName;		//召唤兽及召唤兽坐骑的真实名字	
	uint32				m_uSkillLevel;
public:
	string				m_sNpcName;
private:
	bool				m_bCanBeTakeOver;
	CServantServerMgr*	m_pServantMgr;
	CNpcGroupMgr*		m_pGroupMgr;		 
	CNpcQueueMgr*		m_pNpcQueueMgr;
	ChaserMapType			m_MapChaser; 
	CNpcChaserQueueMgr*		m_NpcChaserQueueMgr;
private:
	CSyncVariantServer*		m_pViewVariantAttribute_Type;
	CSyncVariantServer*		m_pViewVariantAttribute_EntityID;
	CSyncVariantServer*		m_pViewVariantAttribute_Camp;
	CSyncVariantServer*		m_pViewVariantAttribute_BirthCamp;
	CSyncVariantServer*		m_pViewVariantAttribute_GameCamp;
	CSyncVariantServer*		m_pViewVariantAttribute_Level;
	CSyncVariantServer*		m_pViewVariantAttribute_Class;

	CSyncVariantServer*		m_pViewVariantAttribute_NpcType;
	CSyncVariantServer*		m_pViewVariantAttribute_NpcAIType;
	CSyncVariantServer*		m_pViewVariantAttribute_BeActiveNpc;
	CSyncVariantServer*		m_pViewVariantAttribute_AttackType;
	CSyncVariantServer*		m_pViewVariantAttribute_RealName;
	CSyncVariantServer*		m_pViewVariantAttribute_ShowInClient;
	CSyncVariantServer*		m_pViewVariantAttribute_FightDir;

	CSyncVariantServer*		m_pViewVariantPKSwitch_PKSwitchType;
	CSyncVariantServer*		m_pViewVariantPKSwitch_ZoneType;

	CSyncVariantServer*		m_pSyncVariantAttribute1_TargetID;
	CSyncVariantServer*		m_pSyncVariantAttribute1_Speed;
	CSyncVariantServer*		m_pSyncVariantAttribute1_Stealth;
	CSyncVariantServer*		m_pSyncVariantAttribute1_ExpOwnerID;

	CSyncVariantServer*		m_pSyncVariantAlert_AlertTargetID;
	CSyncVariantServer*		m_pSyncVariantAlert_AlertAction;

	CSyncVariantServer*		m_pViewVariantMaster_MasterID;
	CSyncVariantServer*		m_pViewVariantMaster_MasterTeamID;
	CSyncVariantServer*		m_pViewVariantMaster_MasterTroopID;
	CSyncVariantServer*		m_pViewVariantMaster_MasterTongID;

private:
	TPtRefee<CCharacterDictator, CIntObjServer>	m_RefsByIntObj;	
	TPtRefee<CCharacterDictator, CCharacterDictator>	m_RefsByCharacter;	
	TPtRefer<CCharacterDictator, CCharacterDictator>	m_pMaster;
	TPtRefer<CCharacterDictator, CCharacterDictator>	m_pGroupLeader;
	TPtRefee<CCharacterDictator, CNpcAI> m_RefsByNpcAI;
	TPtRefee<CCharacterDictator, CNpcGroupMgr> m_RefsByGroupMgr;
	TPtRefer<CCharacterDictator, ICharacterDictatorCallbackHandler>	m_CallbackHandler;	
	TPtRefer<CCharacterDictator, INpcAIHandler>	m_NpcAIHandler;	
};

