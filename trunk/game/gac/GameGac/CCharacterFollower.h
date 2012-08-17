#pragma once
#include "CEntityClient.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "HeadBlood.h"
#include "CActorFollower.h"
#include "CCharacterFollowerVariantCallback.h"
#include "CSyncVariantClientHolder.h"
#include "CTick.h"
#include "CEntityMallocObject.h"
#include "TEntityAllocator.h"

namespace sqr
{
	class CRenderObject;
	class CCoreObjectFollower;
	class ICoreObjectFollowerHandler;
}

class CVariantCallback;
class ICharacterFollowerCallbackHandler;
class CCharacterDirector;
class CFighterFollower;
//class CActorFollower;
class CCharacterVariantHandler;
class CServantClientMgr;
class CBloodRenderDialogPool;
//class CCharacterFollowerVariantCallback;
class SetNpcTypeVariantCallback;
class LevelChangeVariantCallback;
class SetClassVariantCallback;
class SetCampVariantCallback;
class SetNpcAITypeVariantCallback;
class SetNpcActiveTypeVariantCallback;
class SetAttackTypeVariantCallback;
class SetNpcGameCampVariantCallback;
class SetNpcRealNameVariantCallback;
class SetFighteDirVariantCallback;
class AddMasterVariantCallback;
class BirthCampChangeVariantCallback;
class CDirectionMgr;
class CDisplayHurt;
class OnSpeedChangeVariantCallback;
class CClientCharacterMsgDispatcher;

class CCharacterFollower
	:public CEntityClient
{
	friend class OnSpeedChangeVariantCallback;
	friend class CClientCharacterMsgDispatcher;

	typedef vector<CCharacterVariantHandler*,TEntityAllocator<CCharacterVariantHandler*> >	VecCharacterVariantHandler_t;
protected:
	CCharacterFollower();
	template<typename Traits>
	void Init(typename Traits::CoreObjectType* pCoreObj, uint32 uID);
	void SetSpeed(float fSpeed) {m_fMoveSpeed = fSpeed; GetActor()->DoNowAni(true);}

public:
	static CCharacterFollower* GetCharacter(const CCoreObjectFollower*);
	static CCharacterFollower* GetCharacterByID(uint32 uEntityID);

	CCharacterFollower(CCoreObjectFollower* pCoreObj, uint32 uID, bool bIsBattleHorse);
	virtual ~CCharacterFollower();

	virtual CCharacterDirector* CastToCharacterDirector() { return NULL; }

	virtual EGameEntityType GetGameEntityType()const { return eGET_CharacterFollower; }
	
	virtual void OnPosChanged();
	virtual void OnRefreshRenderObjPos(const CFPos& PixelPos);
	void OnSpeedChanged();
	void RegSyncVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegSyncVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegSyncVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegSyncTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);

	void RegViewVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegViewVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegViewVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void RegViewTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);

	void UnRegSyncVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegSyncVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegSyncVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegSyncTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);

	void UnRegViewVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegViewVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegViewVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	void UnRegViewTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex);
	
	virtual void OnDestroy();
	virtual void OnRenderObjDestroyed();

	//只有Follower会有回调
	void OnEnteredSyncAoi();
	void OnLeftedSyncAoi();

	void OnBeSelected();
	// 注册给lua用的
	bool CppGetCtrlState(EFighterCtrlState eState)const;
	EClass CppGetClass()const;
	void CppSetClass(EClass eClass);
	uint32 CppGetLevel()const;
	void CppSetLevel(uint32 uLevel);
	ECamp CppGetBirthCamp()const;
	void CppSetBirthCamp(ECamp eBirthCamp);
	ECamp CppGetCamp()const;
	void CppSetCamp(ECamp eCamp);
	bool CppGetPKState() const;
	void CppSetPKState(bool bPKState);
	//EPKState CppGetPKState() const;
	//void CppSetPKState(EPKState ePKState);
	//void CppSetZoneType(EZoneType eZoneType);
	float CppGetPropertyValueByName(const TCHAR* szName);
	uint32 CppGetTalentPoint(const TCHAR* szName); 

	float GetSpeed()const {return m_fMoveSpeed;}
	bool CppIsAlive()const;
	void SetTargetBuff();
	float GetRemainTime(const TCHAR* sName, uint32 uId);
	const TCHAR* GetRealName() const	{ return m_strRealName.c_str(); }
	string GetHorseName() const						{ return m_strHorseName; }
	void SetHorseName(const TCHAR* szName)			{ m_strHorseName = szName; }

	CFacingMgr*			GetFacingMgr()const			{ return m_pFacingMgr; }
	CRenderLinkMgr*		GetRenderLinkMgr()const		{ return m_pRenderLinkMgr; }

	/******************************动画播放************************************/
	void	DisplayHurt(int iValue,EAgileType eAgileType,EHurtResult eHurtResult,bool bOverTime, bool IsSkill, const CFPos& Pos);
	void ClearDisplayHurt();
	virtual void OnMoveBegan();
	virtual void OnMoveEnded(EMoveEndedType eMEType);
	virtual void OnMovePathChanged();
	virtual void OnWayPointChanged();

	CActorFollower* GetActor() const;
	CCharacterFollower* GetMaster() const; 
	uint32 GetMasterID() const {return m_uMasterID;}
	void SetMaster(uint32 uMasterID) { m_uMasterID = uMasterID; }
	bool IsBattleHorse()const;
	void SetBattleHorseFlag(bool bIsBattleHorse) { m_bIsBattleHorse = bIsBattleHorse; }
	void InitRealName();
	void SetRealName(const string& strRealName);
	void SetCallbackHandler( ICharacterFollowerCallbackHandler *pCallbackHandler );
	ICharacterFollowerCallbackHandler* GetCallbackHandler() const { return m_pCallbackHandler; }
	virtual CFighterFollower* GetSelectedFighter()const { return NULL; }
	void SetIsPlayer( bool bIsPlayer );
	bool GetIsPlayer();
	void SetIsBoss( bool bIsBoss );
	bool GetIsBoss();
	void SetAttackFxFile(const TCHAR* strAttackFxFile);
	void SetActionFxFile(const TCHAR* strActionFxFile);
	void SetSkillFxFile(const TCHAR* strSkillFxFile);
	void SetSpliceName(const TCHAR* strSpliceName);
	void SetWeaponInfo(const TCHAR* strMainHandLinkName, const TCHAR* strOffHandLinkName,const TCHAR* strMainHandLinkEffect,const TCHAR* strOffHandLinkEffect);
	void SetHorseEffect(const TCHAR* strHorseEffect);
	void SetAdditionEffect(const TCHAR* strAdditionEffect);
	CRenderObject* GetHorseRenderObj()const;
	CRenderObject* GetMainHandRenderObj()const;
	CRenderObject* GetOffHandRenderObj()const;
	CRenderObject* GetAdditionRenderObj()const;
	EMoveMode GetMoveMode(uint8 uWalkBackRadius);
	void ResumeMovingActionDir(uint8 uWalkBackRadius);
	void SetDirLockedTarget(bool bDirLockedTarget)		{ m_bDirLockedTarget = bDirLockedTarget; }
	bool IsDirLockedTarget()							{ return m_bDirLockedTarget; }
	void SetHeadBloodTex(CCharacterFollower* pCharacterDirector);
	void SetHeadBloodTransparent( float fTrans );
	void AddHeadBloodRendler(CCharacterFollower* pCharacterDirector, CBloodRenderDialogPool* pDialogPool, SQRWnd* pParentWnd, bool bShow);
	void DelHeadBloodRendler(CBloodRenderDialogPool* pDialogPool);
	void UpdateHeadBloodRendler();
	void ShowBloodRendler(bool bShow);
	void SetAgileValueBeCare(bool beCare)				{ m_bAgileValueBeCare = beCare; }
	bool GetAgileValueBeCare()							{ return m_bAgileValueBeCare;}
	virtual void OnDead();
	virtual void OnReborn();
	void DoNowAni();
	void SetWeaponVisible(bool bIsMainHand, bool bVisible);
	void SetWeaponPlayType(EWeaponPlayType eWeaponPlayType);
	void SetOffWeaponPlayType(EWeaponPlayType eWeaponPlayType);
	uint32	GetWeaponPlayType();
	uint32	GetOffWeaponPlayType();
	virtual EMoveState GetMoveState();
	virtual void DoMoveState(EMoveState eMoveState, bool bForce );
	virtual void DoMoveState(EMoveState eMoveState) { DoMoveState(eMoveState, false); }
	virtual EActionState GetActionState();
	virtual void DoActionState(EActionState eActionState);
	virtual void DoActionState(EActionState eActionState, uint8 udir);
	void SetMoveDir(uint8 Dir);
	uint8 GetMoveDir()const;
	inline bool CheckSyncDataReceived() const { return m_bSyncDataReceived; }
	inline void SetSyncDataReceived(bool bRe) { m_bSyncDataReceived = bRe; }
	void SetAITypeID(ENpcAIType eNpcAITypeID);
	ENpcAIType GetAITypeID();
	void SetActiveNpc(bool bActive);
	bool BeActiveNpc();
	void SetRemovedImmediately(bool bImme);
	bool GetRemovedImmediately();
	void SetCanBeTakeOver(bool bTakeOver);
	bool CanBeTakeOver();
	void SetNpcType(ENpcType eType);
	ENpcType GetNpcType();
	void  SetNpcAttackType(EAttackType eNpcAttackType);
	EAttackType GetNpcAttackType();
	void CppSetGameCamp(int32 iGameCamp);
	int32 CppGetGameCamp();

	void CreateNpcDirTick();
	void DestroyNpcDirTick();
	void SetNpcAlertState(uint32 uAlertTargetID);
	void SetShowInClient(bool bShow);
	bool ShowInClient();
	void RegistCommonVarriantCallback();
	void RegistNpcVariantCallback();
	void InitIsShadowVariant();
	bool GetIsShadow()	{ return m_bIsShadow; }

	void	SetInterestedObj(CCharacterFollower* pTargetObj);	//设置兴趣对象
	bool	GetInterestedPos( CVector3f& outVec );			//获得兴趣点
	bool	ClearInterested(bool bImme = true);				//清除兴趣
	void	SetIsCtrlAll(bool SetCtrlAll);

	// bForce 强制转向,忽略ForbitTurnAround。bImmed 立即无过程
	void DoActionDir(uint8 uActionDir, bool bForce = false, bool bImmed = false);
	void	SetFirstDexterity(float fValue);
	bool	IsStopBeforeTurn(uint8 uDesDirection);			// 是否旋转前先站立(目标超过90度)
	bool	IsTurnOver();									// 是否转到半角度(旋转前与目标)
	void	SetBloodRenderIsShow(bool bShow){m_bBloodRenderIsShow = bShow;}

	CDirectionMgr* GetDirectionMgr()const {return m_pDirectionMgr;}
	void GetDirection(CDir& dir)const;

	float	GetPairScale();
	float	GetScale();
	float	GetFinalScale();
	void	SetScale( float fScale );
	void	SetFinalScale( float fScale, uint64 uFinalTime );
	void	UpdateScale( uint64 uCurTime );
	void	SetStealthState(bool bSteatlth);

	CCharacterFollower* GetBindObjParent()const;
	CCharacterFollower* MoveFirstBindObjChild();
	CCharacterFollower* MoveNextBindObjChild();
	bool EndOfBindObjChild()const;
	CCharacterFollower*  GetCurBindObjChild()const;
	bool BeDangerForDirector();
	void OnMaterialNumChange(uint32 uMaterialNum);
	uint32 GetCurMaterialNum();
	void OnExpOwnerChange();
	bool BeExpOwnerByDirector();
	uint32 GetExpOwnerID();
	void SetCurOnceActionLevel(uint32 uActionLevel);

private:
	struct NpcAlertTick : public CTick, public CEntityMallocObject
	{
		public:
			NpcAlertTick(CCharacterFollower* pOwner, uint32 uTarget)
			:m_pOwner(pOwner)
			,m_uTarget(uTarget)
			,m_bLockedTarget(false)
			{
				m_pSyncRootVariant = m_pOwner->GetSyncVariantHolder(eCTI_Agile);
			}
			~NpcAlertTick()
			{
				m_pOwner->GetActor()->DoActionState(eAS_Idle_BackWpn);
				m_pSyncRootVariant = NULL;
			}
			void OnTick();
		private:
			CCharacterFollower*					m_pOwner;
			uint32								m_uTarget;
			const CSyncVariantClientHolder*		m_pSyncRootVariant;
			bool								m_bLockedTarget;
	};

	struct NpcFaceEnemyTick : public CTick, public CEntityMallocObject
	{
	public:
		NpcFaceEnemyTick(CCharacterFollower* pOwner) : m_pOwner(pOwner){}
		void OnTick();
	private:
		CCharacterFollower*			m_pOwner;
	};

protected:
	CCharacterController*	m_pIKCtrl;			//逆向控制器
	VecCharacterVariantHandler_t m_vecSyncVariantHandler;
	VecCharacterVariantHandler_t m_vecViewVariantHandler;
private:
	uint32	m_uMasterID;
	ICharacterFollowerCallbackHandler* m_pCallbackHandler;

	CFacingMgr*			m_pFacingMgr;
	CRenderLinkMgr*		m_pRenderLinkMgr;
	CDirectionMgr*		m_pDirectionMgr;
	CDisplayHurt*			m_pDisplayHurtTick;
	bool m_bSyncDataReceived;
	bool m_bIsBattleHorse;
	bool m_bIsPlayer;
	bool m_bIsBoss;
	string m_strRealName;		//召唤兽及召唤兽坐骑的真实名字
	string m_strHorseName;		// 坐骑的名字
	ENpcAIType m_NpcAIType;			//NPC的AI名称，主角为空
	ENpcType m_eNpcType;
	EAttackType m_eNpcAttackType;
	bool m_bActiveNpc;
	bool m_bRemovedImmediately;
	bool m_bCanBeTakeOver;
	bool m_bDirLockedTarget;
	bool m_bShowInClient;
	HeadBlood* m_HeadBloodRendlerDialog;
	bool m_bAgileValueBeCare;
	// 而m_fMoveSpeed是由慢变快后的最终速度
	float m_fMoveSpeed;			// 原始速度

	CTick*		m_pChangeNpcDirTick;			//NPC战斗中实时面向玩家使用
	CTick*		m_pNpcAlertTick;
	bool		m_bIsShadow;
	bool		m_bBloodRenderIsShow;
	bool		m_bDestroying;
};
