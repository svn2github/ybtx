#pragma once
#include "FightDef.h"
#include "CMagicEffClient.h"
#include "CStaticObject.h"
#include "TCfgSharedPtr.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

DefineCfgSharedPtr(CSkillClient)


class CCfgCalc;
class CCastingProcessCfgClientSharedPtr;

class CSkillClient
	: public virtual CStaticObject
	, public CConfigMallocObject
{
public:
	CSkillClient();
	~CSkillClient();

	static					bool LoadSkillConfig(const string& szFileName);
	static					bool LoadNPCSkillConfig(const string& szFileName);
	static					bool LoadIntObjSkillConfig(const string& szFileName);
	static					void UnloadSkillConfig();

	static					CSkillClient* GetSkillByNameForLua(const TCHAR* szName);	//注册给lua用的
	static					CSkillClientSharedPtr& GetSkillByName(const TCHAR* szName);
	static					CSkillClientSharedPtr& GetSkillByID(uint32 uId);

	uint32					GetId()								{return m_uId;}
	const TCHAR*			GetName()							{return m_sName.c_str();}
	bool					GetIsUpdateDirection()				{return m_bIsUpdateDirection;}
	bool					GetHorseLimit()						{return m_bHorseLimit;}
	bool					GetIsStanceSkill()					{return m_bIsStanceSkill;}
	bool					GetTargetIsCorpse()					{return m_bTargetIsCorpse;}
	EFSTargetType			GetSelectTargetType()				{return m_eSelectTargetType;}
	EAttackType				GetAttackType()						{return m_eAttackType;}
	EAlterNormalAttackType	GetStartNormalAttack()const			{return m_eAlterNormalAttackType;}
	CCfgCalc*				GetMinCastSkillDistance()			{return	m_pMinCastSkillDistance;}
	CCfgCalc*				GetMaxCastSkillDistance()			{return	m_pMaxCastSkillDistance;}
	ESkillCoolDownType		GetCoolDownType()					{return m_eCoolDownType;}
	CCfgCalc*				GetCoolDownTime()					{return m_pCoolDownTime;}
	const TCHAR*			GetCastAction()						{return	m_sCastAction.c_str();}
	bool					IsAllBodyCastAni()					{return m_bAllBodyCastAni;}
	const TCHAR*			GetCastEffect()						{return m_sCastEffect.c_str();}
	CMagicEffClientSharedPtr&		GetMagicEff();
	uint32					GetPositionArea(CFighterDirector* pFrom, uint8 uSkillLevel);
	bool					MustIdle();	
	bool					HasCastingProcess()const;

	EConsumeType			GetConsumeType();
	CCfgCalc*				GetConsumeValue();
	ECastingProcessType     GetCastingProcessType();
	CCfgCalc*				GetCastingProcessTime();

private:
	typedef map<string, EAlterNormalAttackType, less<string>, 
		TConfigAllocator<pair<string, EAlterNormalAttackType > > > MapAlterNormalAttackType;
	typedef map<string, CSkillClientSharedPtr*, less<string>, TConfigAllocator<pair<string, CSkillClientSharedPtr*> > >	MapSkillClientByName;
	typedef vector<CSkillClientSharedPtr*, TConfigAllocator<CSkillClientSharedPtr*> >		MapSkillClientByID;
	void InitCastingProcess(const CCastingProcessCfgClientSharedPtr& cfg);

	static MapSkillClientByName		ms_mapSkillByName;
	static MapSkillClientByID		ms_mapSkillByID;
	static MapAlterNormalAttackType	ms_mapAlterNormalAttackType;

	uint32							m_uId;								//技能编号
	string 							m_sName;							//技能名称
	bool							m_bIsUpdateDirection;				//是否更新方向
	bool							m_bHorseLimit;						//是否是坐骑技能
	bool							m_bIsStanceSkill;					//是否为姿态技能
	bool							m_bTargetIsCorpse;					//目标是否为尸体
	EFSTargetType					m_eSelectTargetType;				//选中目标类型
	EAttackType						m_eAttackType;						//攻击类型
	CCfgCalc*						m_pMinCastSkillDistance;			//最小施法距离
	CCfgCalc*						m_pMaxCastSkillDistance;			//最大施法距离
	ESkillCoolDownType				m_eCoolDownType;					//技能冷却类型
	CCfgCalc*						m_pCoolDownTime;					//冷却时间
	CCfgCalc*						m_pSkillLevel2Level;				//等级映射
	string							m_sReachUpAction;					//起手动作
	string							m_sReachUpEffect;					//起手特效
	string							m_sCastAction;						//出手动作
	bool							m_bAllBodyCastAni;					//全身出手动作
	string							m_sCastEffect;						//出手特效
	string							m_sMagicEff;
	CCfgCalc*						m_pPositionArea;	
	CCfgCalc*						m_pCastingProcessTime;
	ECastingProcessType				m_eCastingProcessType;
	bool							m_bMustIdle;
	bool							m_bHasCastingProcess;
	EAlterNormalAttackType			m_eAlterNormalAttackType;		//对普攻的影响
};
