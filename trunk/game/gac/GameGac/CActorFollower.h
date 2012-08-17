#pragma once
#include "FightDef.h"
#include "CActorClient.h"

namespace sqr
{
	class CSyncVariantClient;
	class CRenderObject;
}

class CPlayerFX;
class CNPCGazedAtMgr;
class CActorFollower;
class CCharacterFollower;
class CActorAniCfg;
class IActorFollowerHandler;
class CMoveStateNode;
class CCharacterController;
class CWhiskSoundPlayer;
class CHitSoundPlayer;
class CMoveStateCallback;
class CMoveDirCallback;
class CActionStateCallback;
class CActionDirCallback;
class CWalkBackRadiusCallback;
class CNAActionRhythmCallback;

typedef void (CActorFollower::*MoveFunc)();

enum EWeaponPosAndBattleState
{
	eWPBS_BackWeapon,		// 背武器
	eWPBS_HoldWeapon,		// 持武器
	eWPBS_BattleState,		// 战斗状态
};

class CActorFollower
	: public CActorClient
{
	friend class CCharacterFollower;

public:
	CActorFollower();
	~CActorFollower();
	void ClearVariantCallbackFunc();
	void RegVariantCallbackFunc();

	uint32					GetEntityID() const;
	IActorFollowerHandler*	GetHandler() const;
	CCharacterFollower*		GetCharacter() const;

	// 改变状态 并 播放动作 接口
	bool DoMoveState(EMoveState eMoveState, bool bForce = false);
	void DoMoveAnimation();

	bool DoActionState(EActionState eActionState, int16 uActionDir = -1, bool bImmed = false, bool bForce = false, uint8 uActionIndex = 0);
	void DoActionSimplify(EActionState eActionState);
	void DoNowAni(bool bIsMoveAction = false);

	// 表情动作
	bool DoFunActionState(EActionState eActionState, uint8 uActionIndex);
	void EndFunActionState();

	// 当前动作级别
	EActionPlayPriority	GetCurOnceActionLevel()				{ return m_eCurOnceActionLevel; }
	void SetCurOnceActionLevel(EActionPlayPriority eLevel)	{ m_eCurOnceActionLevel = eLevel; }

	void PlayFxByActionState(EActionState eActionState, const string& strAniName);
	void SplitAttackFxFile(string &FxFile, string &FxName);
	void SplitSkillFxFile(const string &InFx, string &FxFile, string &FxName, bool bNeedSpliceName);
	void SplitSkillFxFile(const string &InFx, string &FxFile, string &FxName, uint32& uDeleyTime,bool bNeedSpliceName);

	void SetAttackFxFile(const string& strAttackFxFile)			{ m_strAttackFxFile = strAttackFxFile; }
	void SetActionFxFile(const string& strActionFxFile)			{ m_strActionFxFile	= strActionFxFile; }
	void SetSkillFxFile(const string& strSkillFxFile)			{ m_strSkillFxFile	= strSkillFxFile; }
	void SetSpliceName(const string& strSpliceName)			{ m_strSpliceName	= strSpliceName; }

	void	SetNAActionRhythm(float fActionRhythm)		{ m_fNAActionRhythm = fActionRhythm; }
	float	GetNAActionRhythm() const					{ return m_fNAActionRhythm; }
	void	SetNATempRhythm(float fTempRhythm)			{ m_fNATempRhythm = fTempRhythm; }
	float	GetNATempRhythm() const						{ return m_fNATempRhythm; }
	void	SetWalkBackRadius(uint8 uWalkBackRadius)	{ m_uWalkBackRadius = uWalkBackRadius; }
	uint8	GetWalkBackRadius()	const					{ return m_uWalkBackRadius; }

	// 调整动作播放速率
	EActionState AdjustActionRhythm(EActionState eActState);

	void SetNeedMoveUpBodyAni(bool bNeed)		{ m_bNeedMoveUpBodyAni = bNeed; }
	bool GetNeedMoveUpBodyAni() const			{ return m_bNeedMoveUpBodyAni; }
	void SetComboSing(bool bComboSing)			{ m_bComboSing = bComboSing; }
	bool GetComboSing()							{ return m_bComboSing; }

	uint8	GetMoveDir() const  				{ return m_uMoveDir; }	
	uint8	GetActionDir() const  				{ return m_uActionDir; }
	uint8	GetActionIndex() const				{ return m_uActionIndex; }
	virtual void SetMoveDir(uint8 uMoveDir)		{ m_uMoveDir = uMoveDir; }
	void	SetActionDir(uint8 uActionDir)		{ m_uActionDir = uActionDir; }
	void	SetActionIndex(uint8 uActionIndex)	{ m_uActionIndex = uActionIndex; }
	void	SetAllBodyCastAni(bool bAllBody)	{ m_bIsAllBodyAni = bAllBody; }

	void	SetTargetArmorString(string strArmor)	{ m_strArmorString = strArmor; }
	string	GetTargetArmorString()					{ return m_strArmorString; }

	void	SetHurtResult(EHurtResult eHurtResult)	{ m_eHurtResult = eHurtResult; }
	EHurtResult GetHurtResult()						{ return m_eHurtResult; }

	void OnSepAnimationEnd(const TCHAR* szName);
	void OnAnimationEnd(const TCHAR* szName);
	void OnResLoaded();
	void OnSetVisable(bool isVisable);
	void OnKeyFrame( const TCHAR * szName );

private:
	typedef std::list<CMoveStateNode*>		MOVE_LIST;
	MOVE_LIST			m_lMoveList;

	// 动作控制
	bool				m_bIsAllBodyAni;
	bool				m_bNeedMoveUpBodyAni;
	EActionPlayPriority m_eCurOnceActionLevel;
	bool				m_bComboSing;
	bool				m_bNoHandlerActor;
	int					m_nUpperBodyID;

	// 动作特效
	string			m_strAttackFxName;
	string			m_strAttackFxFile;
	string			m_strActionFxFile;
	string			m_strSkillFxFile;
	string			m_strSpliceName;
	CPlayerFX*		m_pActionFX;

	// 动作播放速率
	float			m_fNAActionRhythm;
	float			m_fNATempRhythm;
	float			m_fCurAniRhythm;

	CMoveStateCallback*		m_pMoveStateCallback;
	CMoveDirCallback*		m_pMoveDirCallback;
	CActionStateCallback*	m_pActionStateCallback;
	CActionDirCallback*		m_pActionDirCallback;
	CWalkBackRadiusCallback*	m_pWalkBackRadiusCallback;
	CNAActionRhythmCallback*	m_pNAActionRhythmCallback;

	uint8			m_uWalkBackRadius;		// 后退走半径
	uint8			m_uMoveDir;				// 移动朝向
	uint8			m_uActionDir;			// 动作朝向
	uint8			m_uActionIndex;			// 表情动作索引

	string			m_strArmorString;
	EHurtResult		m_eHurtResult;

private:

	// MoveState相关
	void RegisterMoveAnimation(MoveFunc func);	//将动作函数与其参数注册进动作链表
	void DoStop();
	void DoMove();
	EActionState GetStopActState();
	EActionState GetMoveActState();

	bool SetActionDirection(EActionState EActionState, int16 nActionDir, bool bImmed, bool bForce, bool bIsSepMode);

	// 规则
	bool PlayActionRule(CRenderObject* RenderObj, EActionState eActionState, bool bIsSepMode, EActionPlayPriority ePriorityLevel);
	void ExecuteSpecialRuleForAllBodyAni(EActionState eActionState);

	// 得到持武器和战斗状态
	EWeaponPosAndBattleState GetWeaponPosAndBattleState();
};

