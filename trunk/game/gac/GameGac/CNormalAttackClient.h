#pragma once
#include "CNormalAttackCfgClient.h"
#include "CFighterMallocObject.h"

class CFighterFollower;
class CFighterDirector;

class CNormalAttackClient : public CFighterMallocObject
{
public:
	CNormalAttackClient(CFighterDirector* pAttacker);
	~CNormalAttackClient(void);

	static uint32 GetTickTime();
	CFighterFollower* GetTarget()				{ return m_pTarget; }
	//void	SetAutoNormalAttack(bool bAutoNormalAttack)		{m_bIsAutoNormalAttack=bAutoNormalAttack;}
	void	InitNormalAttack(CFighterFollower* pTarget, float fSpeed, bool bIsNeedAutoTracker);
	const CNormalAttackCfgClientSharedPtr&	GetNormalAttackCfg(){return m_pNormalAttackCfg;}
	float	GetMainHandWeaponInterval()			{return m_fMainHandWeaponInterval;}
	float	GetAssistantWeaponInterval()		{return m_fAssistantWeaponInterval;}
	void	CancelNormalAttack();
	void	SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,EWeaponType MHWeaponType, EAttackType eAttackType);
	void	SetAHNACfg(const TCHAR* szName,float fAssistantWeaponInterval, EWeaponType AHWeaponType);
	uint32	CheckNACondAndSetNATarget(CFighterFollower* pTarget);
	bool	IsTargetInNAScope(CFighterFollower* pTarget);		//目标是否在普攻范围内	
	bool	IsAutoNormalAttack(){return m_bIsAutoNormalAttack;}
	bool	IsAttacking()						{ return m_bIsNormalAttacking; }
	bool	IsShortDistanceNA()const;
	bool	HasWeapon();
	EWeaponType GetMHWeaponType()const				{return m_eWeaponType;}
	void SetMHWeaponType(EWeaponType eWeaponType)	{m_eWeaponType = eWeaponType;}
	EAttackType GetMHAttackType()const				{return m_eAttackType;}
	void SetMHAttackType(EAttackType eAttackType)	{m_eAttackType = eAttackType;}
	EWeaponType GetAHWeaponType()const				{return m_eAHWeaponType;}
	void SetAHWeaponType(EWeaponType eWeaponType)	{m_eAHWeaponType = eWeaponType;}

	float	GetNormalAttackDistance();

private:
	CNormalAttackCfgClientSharedPtr		m_pNormalAttackCfg;
	CFighterDirector*			m_pAttacker;
	CFighterFollower*			m_pTarget;
	uint32						m_uTargetID;
	bool						m_bIsAutoNormalAttack;
	float						m_fAttackerMoveSpeed;
	bool						m_bIsNormalAttacking;
	float						m_fMainHandWeaponInterval;
	float						m_fAssistantWeaponInterval;
	EWeaponType					m_eWeaponType;			//主手武器类型
	EWeaponType					m_eAHWeaponType;		//副手武器类型
	EAttackType					m_eAttackType;			//武器类型
};
