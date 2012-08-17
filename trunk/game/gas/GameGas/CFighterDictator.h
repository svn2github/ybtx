#pragma once
#include "CFighterAgileInfoServer.h"
#include "TFighterCtrlInfo.h"
#include "CFighterCalInfo.h"
#include "TPropertyHolder.h"
#include "CPos.h"
#include "PatternCOR.h"
#include "TimeHelper.h"
#include "PatternRef.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"
#include "TFighterAllocator.h"
#include "PropertyType.h"

namespace sqr
{
	class CCoreSceneServer;
	class ISend;
	class CConnServer;
	class CAppServer;
	class CSyncVariantServer;
}

class ICharacterDictatorCallbackHandler;
class CTalentHolder;
class CTalentHolderServer;
template<typename,typename>class TCoolDownMgr;
typedef TCoolDownMgr<CAppServer,CFighterDictator>	CCoolDownMgrServer;
class IFighterServerHandler;
class CTempVarMgrServer;
class CNormalAttackMgr;
class CSkillMgrServer;
class CMagicMgrServer;
class CAllStateMgrServer;
class CFighterMediator;
class CSpeedChangeMgr;
class CAureMagicDBDataMgr;
class CAureMagicDBData;
class CSkillCoolDownDBDataMgr;
class CSkillCoolDownDBData;
class CGenericTarget;
class CBattleStateManager;
class CCharacterDictator;
class CEntityServer;
class CCastingProcess;
class CMagicServer;
class CSkillInstServer;
class CNormalSkillServerSharedPtr;


///////////////////////////////////////////////////////////////////////////////////////////////
//	CFighterDictator类是服务端战斗系统的基类，同时也是所有非玩家对象的战斗系统的最终派生类（Npc，召唤兽，etc）
//	Actor和Character对Fighter来说都是可见的（通过FighterHandler上的GetCharacter或GetActor就能直接拿到对象）
///////////////////////////////////////////////////////////////////////////////////////////////

class CSyncPropertyHolder : public CBasePropertyHolder
{
public:
	virtual ~CSyncPropertyHolder(){}
	virtual void SyncToClient(CFighterDictator* pTargetFighter, CFighterDictator* pOwnerObj,EPropertyID ePropertyID) = 0;
};

class CFighterDictator
	:public CFighterAgileInfoServer
	,public TFighterCtrlInfo<CountType>
	,public CFighterCalInfo
	,public CFighterSyncToDirectorCalInfo
	,public TPropertyHolder<CFighterDictator>
	,public CPtCORSubject
	,public CFighterMallocObject
{
public:
	typedef CFighterAgileInfoServer	AgileInfo_t;
	typedef map<string, uint32, less<string>, TFighterAllocator<pair<const string, uint32> > >	MapActiveSkill;							//免疫任务攻击类型
	typedef set<uint32, less<uint32>, TFighterAllocator<uint32> >	SetSyncEntity;
	typedef map<EPropertyID,CSyncPropertyHolder*>	MapSyncCalProperty;

	typedef TPtRefee<CFighterDictator, CMagicServer> RefFighterByMagic;
	typedef TPtRefee<CFighterDictator, CCastingProcess> RefFighterByCast;
	CFighterDictator();
	virtual ~CFighterDictator();
	virtual void Init();
	virtual CFighterMediator*		CastToFighterMediator()		{return NULL;}
	IFighterServerHandler*		GetHandler()const;
	void SetHandler(IFighterServerHandler* pHandler);
	ICharacterDictatorCallbackHandler*	GetCallBackHandler()const;
	void CppInit(EClass eClass, uint32 uLevel, ECamp eCamp);
	virtual void CppDie(CFighterDictator* pAttacker, const TCHAR* szSkillName);			// 死亡
	virtual void CppReborn();									// 重生
	void CppSetClass(EClass eClass);
	void CppSetCamp(ECamp eCamp);
	void CppSetLevel(uint32 uLevel);
	void CppSetBirthCamp(ECamp eCamp);
	void CppSetPKState(bool bPKState);
	//void CppSetPKState(EPKState ePKState);
	//void CppSetZoneType(EZoneType eZoneType);
	bool ExistState(const string& name);						// 查看是否存在状态
	uint32 GetStateCascade(const string& sName);					// 获取状态层数
	void ClearState(const string& name);						// 清除一个自己的魔法状态
	bool EraseErasableState(const string& name);

	CTempVarMgrServer*		GetTempVarMgr()const			{return m_pTempVarMgr;}
	CNormalAttackMgr*		GetNormalAttackMgr()const		{return m_pNormalAttackMgr;}
	CSkillMgrServer*		GetSkillMgr()	const			{return m_pSkilllMgr;}
	CMagicMgrServer*		GetMagicMgr()const				{return m_pMagicMgr;}
	CAllStateMgrServer*		GetAllStateMgr()const			{return	m_pAllStateMgr;}
	CCoolDownMgrServer*		GetCoolDownMgr()const			{return m_pCoolDownMgr;}
	CTalentHolder*			GetTalentHolder()const;
	CSpeedChangeMgr*		GetMoveSpeedChangeMgr() const	{return m_pMoveSpeedChangeMgr; }
	CSpeedChangeMgr*		GetWalkSpeedChangeMgr() const	{return m_pWalkSpeedChangeMgr; }
	CSpeedChangeMgr*		GetNASpeedChangeMgr() const		{return m_pNASpeedChangeMgr; }
	CBattleStateManager*	GetBattleStateMgr() const		{return m_pBattleStateMgr;}

	void CppInitNPCNormalAttack(const TCHAR* szMHName);
	void CppInitNormalAttack(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType, 
	const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType);
	void CppInitMHNA(const TCHAR* szMHName, const TCHAR* MHAttackType, const TCHAR* MHWeaponType, float fAttakSpeed);
	void CppInitAHNA(const TCHAR* szAHName, const TCHAR* AHAttackType, const TCHAR* AHWeaponType, float fAttakSpeed);
	bool DoNormalAttack(CFighterDictator* pTarget);
	virtual void PauseAttack(bool bLockRestart = false, bool bCalDamageImmediately = true);
	virtual bool RestartAttack(bool bUnlockRestart = false);
	virtual void CancelAttack(bool bCalDamageImmediately = false);
	virtual void CancelNormalAttackAutoTracking() {}
	bool IsAttacking();

	void DoNPCSkill(const string& strSkillName, uint8 uSkillLevel, CGenericTarget* pTarget,bool bDoSkillRule = true);
	void DoNPCSkill(const CNormalSkillServerSharedPtr* pCfg, uint8 uSkillLevel, CGenericTarget* pTarget,bool bDoSkillRule = true);
	void DoIntObjSkill(const string& strSkillName, uint8 uSkillLevel, CGenericTarget* pTarget);
	void ChangeImmuneTypeValue(const string& szName, int32 iValue);
	bool IsInImmuneType(const string& szName);
	
	CFighterDictator*		GetTotem()const;
	CFighterDictator*		GetPet()const;

	void		InsertTalent(const string& szName, uint32 uPoint);
	void		InsertEquipTalent(const string& szName, uint32 uPoint);
	bool		RemoveEquipTalent(const string& szName, uint32 uPoint);
	void		ClearAllTalent();

	void ServerRelSoulBullet(uint32 uTargetEntityID, const TCHAR* szBulletName, EBurstSoulType eBurstSoulType, uint32 uCount);

	void SetEnmityToTarget(CFighterDictator* pTarget);
	void SetTarget(CFighterDictator* pTarget, bool bCalNAImmediately = false);
	void SetTargetPos(const CFPos& pos)			{m_TargetPos=pos;}
	void SetLockingTarget(CFighterDictator* pTarget);
	CFighterDictator* GetTarget()const			{ return m_Target.Get(); }
	const CFPos& GetTargetPos()const				{ return m_TargetPos; }
	CFighterDictator* GetLockingTarget()const	{ return m_LockingTarget.Get(); }

	void AddSkill(const string& strSkillName,uint8 uSkillLevel);
	void RemoveSkill(const string& strSkillName);
	bool IsSkillActive(const string& strSkillName,uint8 uSkillLevel)const;
	uint32 GetActiveSkillLevel(const string& strSkillName)const;
	void ClearAllSkill();
	MapActiveSkill& GetActiveSkillMap(){return m_mapActiveSkill;}

	template<typename CmdType>
	bool				SendCmdToGac(const CmdType*	pCmd,uint32 uRange=0)const	{return SendToGac(pCmd,sizeof(CmdType),uRange);}

	virtual	bool		SendToGac(const void* pData,uint32 uSize,uint32 uRange=0)const;

	void OnDeadToQuest(CFighterDictator* pAttacker, const TCHAR* szDeadBySkillName)const;
	void OnDead(CFighterDictator* pAttacker, const TCHAR* szDeadBySkillName)const;
	void OnReborn()const;
	void OnLevelChanged(uint32 uLevel)const;
	void OnClassChanged(EClass eClass)const;
	void OnCampChanged(ECamp eCamp)const;
	void OnGameCampChange(int32 iGameCamp)const;
	void OnPlayerBirthCampChange(ECamp eCamp) const;
	void OnPKStateChanged(bool ePKState) const;
	//void OnPKStateChanged(EPKState ePKState)const;
	//void OnZoneTypeChanged(EZoneType eZoneType)const;
	void RemoveFacingPair()const;
	void NotifyNPCBeHurt( int32	iHurt);
	virtual void OnMoveSpeedChange(float fSpeed, const char* szSpeedChangeReason, const char* szSkillName)const;
	void OnRealNameChange(ENpcType eType, const TCHAR* szServantShowName)const;
	void SyncTarget(uint32 uTargetID);
	//属性
	void OnAgileValueChanged(uint32 uValueTypeID, float fValue)const;
	void OnCalValueChanged(uint32 uValueTypeID, float fValue)const;
	void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const;
	void OnSkillBegin()const;
	void OnSkillEnd(bool bSucceed)const;
	void OnAddServant(ENpcType eType, uint32 uServantObjID);
	void OnDestroyServant(ENpcType eType, uint32 uServantID);
	void SynToFollowCanBeTakeOver();
	void SynToFollowCanNotTakeOver();
	void SyncToClientTalentChange(const TCHAR* szTalentName, int32 iPoint)const;
	void SyncToClientTalentRemove(const TCHAR* szTalentName)const;
	void OpenObjPanelByID(uint32 uEntityID, bool bIsCare, bool bIsTargetPanel);
	void OnChangeAttackType();
	CSkillCoolDownDBDataMgr* ReturnSkillCoolDownTime();
	uint32 GetSkillCoolDownTimeByName(const TCHAR* szSkillName);
	void SaveSkillCoolDownTimeToDBEnd();
	void CPPInitSkillCoolDownTime(CSkillCoolDownDBDataMgr* pSkillCoolDownDBDataMgr);
	CAureMagicDBDataMgr* SerializeAureMagicToDB();
	void SaveAureMagicToDBEnd();
	void LoadAureMagicFromDB(const TCHAR* szAureMagicName, uint8 uSkillLevel, const TCHAR* sSkillName, EAttackType eAttackType);
	void ResetAllCoolDown();

	void OnPrintFightInfo(EFightInfoIndex eFightInfoIndex, uint32 uObj1ID, uint32 uObj2ID, CSkillInstServer* pSkillInst, int32 iHPChanged, 
		EAttackType eAttackType, uint32 uHPExtra, uint32 uRealChangeValue, bool bToTeamMates = false, uint32 uStateId = 0, const TCHAR* szArgName = "", uint16 uArgLen = 0);

	CCharacterDictator* GetCharacter() const;
	CEntityServer* GetEntity() const;
	uint32 GetEntityID() const;

	CCoreSceneServer* GetScene() const;
	void GetGridPos(CPos& GridPos)const;
	ESetPosResult SetGridPos(const CPos& GridPos);
	void GetPixelPos(CFPos& PixelPos)const;
	const CFPos& GetPixelPos()const;
	ISend* GetIS(uint32 uRange)const;
	bool IsMoving()const;
	ESetPosResult SetPixelPos(const CFPos& PixelPos);
	EStopMovingResult	StopMoving()const;
	bool DirectorMovingIsDisabled()const;
	CConnServer* GetConnection() const;

	bool	IsExistMoveMgr()const;

	void ChangeTaskAttackImmuneValue(const string& szName, int32 iValue);
	bool IsInTaskAttackImmuneType(const string& szName)  const;
	bool PrintInfoIsOff() const;
	RefFighterByMagic& GetRefFighterByMagic() {return m_RefsByMagic;}
	RefFighterByCast& GetRefFighterByCaset() {return m_RefsByCast;}

	bool	CurCastingProcessCanBeInterrupted()const;
	void	OnTurnAround();

	float GetFighterDist(const CFighterDictator* pFighter)const;
	float GetFighterDistInGrid(const CFighterDictator* pFighter)const;

	void InitAgileValue();
	void ResetCtrlValueVariant();	
	void CppSetGameCamp(int32 iGameCamp);
	bool SetCtrlState(EFighterCtrlState eState, bool bSet, CSkillInstServer* pSkillInst);
	bool CppSetCtrlState(EFighterCtrlState eState, bool bSet);
	bool FilterCtrlState(EFighterCtrlState eState);
	void CppSetAlive(bool bAlive);
	void SetLevelPress(bool bLevelPress);
	bool GetLevelPress();
	uint32 GetDefence();
	uint32 GetNatureResistance();
	uint32 GetDestructionResistance();
	uint32 GetEvilResistance();

	void OnTransferEnd(CCoreSceneServer* pOldScene,const CFPos& OldPos);
	void ClearAllStateMgr();

	static void BuildCalPropertyTable();
	static void ReleaseCalPropertyTable();

	void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
	void UnRegistDistortedTick(CDistortedTick* pTick);
	uint64 GetDistortedProcessTime();
	uint64 GetDistortedFrameTime();
	void GM_SetMoveSpeedPercent(float fPercent);

	void SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet);
	bool GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState);

	void IntDie();	//设置死亡标志位及死亡前的清楚操作
	
protected:
	void SetSpeed( float fSpeed)const;

	CTempVarMgrServer*				m_pTempVarMgr;				// 临时变量管理器
	CNormalAttackMgr*				m_pNormalAttackMgr;
	CSkillMgrServer*				m_pSkilllMgr;
	CMagicMgrServer*				m_pMagicMgr;
	CAllStateMgrServer*				m_pAllStateMgr;
	CCoolDownMgrServer*				m_pCoolDownMgr;
	CTalentHolderServer*			m_pTalentHolder;
	CSpeedChangeMgr*				m_pMoveSpeedChangeMgr;		// 速度改变管理器（目前只支持改变移动速度百分比的魔法操作，不支持改变移动速度的魔法操作）
	CSpeedChangeMgr*				m_pWalkSpeedChangeMgr;		// 速度改变管理器（目前只支持改变移动速度百分比的魔法操作，不支持改变移动速度的魔法操作）
	CSpeedChangeMgr*				m_pNASpeedChangeMgr;		// 速度改变管理器（目前只支持改变移动速度百分比的魔法操作，不支持改变移动速度的魔法操作）
	CBattleStateManager*			m_pBattleStateMgr;			// 战斗状态管理器
	CSkillCoolDownDBDataMgr*		m_pSkillCoolDownRet;
	CAureMagicDBDataMgr*			m_pAureMagicRet;
	CFPos							m_TargetPos;				// 技能选中的目标地点
	MapActiveSkill					m_mapActiveSkill;
	SetSyncEntity					m_setSyncEntity;
	static MapSyncCalProperty		ms_mapSyncCalProperty;
	bool							m_bLevelPress;

public:
	CSyncVariantServer*				m_pCastIDVariant;
	CSyncVariantServer*				m_pCastTimeVariant;
	CSyncVariantServer*				m_pCastGuessStartTimeVariant;
	CSyncVariantServer*				m_pCastTargetIDVariant;
	CSyncVariantServer*				m_pCastSkillIDVariant;
	CSyncVariantServer*				m_pCastIsFinishVariant;

	CSyncVariantServer*				m_pCtrlValueVariant;
	CSyncVariantServer*				m_pCtrlValueTypeIDVariant;
	CSyncVariantServer*				m_pCtrlSetVariant;
	CSyncVariantServer*				m_pCtrlIsAliveVariant;

	CSyncVariantServer*				m_pHealthPointAdderVariant;
	CSyncVariantServer*				m_pHealthPointMutiplierVariant;
	CSyncVariantServer*				m_pHealthPointAgileVariant;
	CSyncVariantServer*				m_pConsumePointAdderVariant;
	CSyncVariantServer*				m_pConsumePointMutiplierVariant;
	CSyncVariantServer*				m_pConsumePointAgileVariant;

private:
	TPtRefer<CFighterDictator, IFighterServerHandler>	m_Handler;
	TPtRefee<CFighterDictator, CFighterDictator>	m_RefsByFighter;	
	TPtRefer<CFighterDictator, CFighterDictator>	m_Target;
	TPtRefer<CFighterDictator, CFighterDictator>	m_LockingTarget;
	RefFighterByMagic	m_RefsByMagic;	
	RefFighterByCast	m_RefsByCast;
};

extern uint32 GetFighterEntityID(CFighterDictator* pFighter);
