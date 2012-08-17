#pragma once 

#include "CPos.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"
#include "TNpcAIDataAllocator.h"
#include "CSkillRuleDef.h"

class CSkillRule;
class CNpcAI;
enum ESRTargetType;
enum ERuleSkillType;
class CCharacterDictator;
class CSkillAroundRule;
class CNpcEnmityMember;

class CTargetFilter
	: public CNpcAIMallocObject
{
	typedef list<CNpcEnmityMember*, TNpcAIAllocator<CNpcEnmityMember*> >		EnemyListType;
	typedef vector<CCharacterDictator*, TNpcAIAllocator<CCharacterDictator*> >	CharacterVecType;
	friend class CSkillAroundRule;
	friend class CSkillRule;
public:
	CTargetFilter(CSkillRule* pSkillRule, ESRTargetType eTargetType, uint32 uTargetArg1, uint32 uTargetArg2);
	~CTargetFilter();
	CSkillRule* GetSkillRule();
	ERuleSkillType GetRuleSkillType();
	bool SetSkillRuleTarget();
	uint32 GetTargetID();
	void GetTargetPos(CFPos &pos);
private:
	CNpcAI* GetOwnerAI();
	bool GetPositionTarget(CFPos &pos);
	bool GetCharacterTarget(CCharacterDictator* &pCharacter);
	CCharacterDictator* GetMaxHpEnemy();
	CCharacterDictator* GetMinHpEnemy();
	CCharacterDictator* GetMaxMpEnemy();
	CCharacterDictator* GetMinMpEnemy();
	CCharacterDictator* GetMaxHpFriend(float fRadius);
	CCharacterDictator* GetMinHpFriend(float fRadius);
	CCharacterDictator* GetMaxMpFriend(float fRadius);
	CCharacterDictator* GetMinMpFriend(float fRadius);
	CCharacterDictator* GetRandomFriend(float fRadius);
	CCharacterDictator* GetNearestFirend(float fRadius);
	CCharacterDictator* GetFarthestFriend(float fRadius);
	CCharacterDictator* GetNearestEnemy();
	CCharacterDictator* GetFarthestEnemy();
	CCharacterDictator* GetRandomUnEnmity(float fRadius);
	CCharacterDictator* GetRandomUnServant(float fRadius);
	CCharacterDictator* GetRandomUnEnmityUnServant(float fRadius);
private:
	CSkillRule*					m_pSkillRule;
	ESRTargetType				m_eTargetType;
	uint32						m_uTargetArg1;
	uint32						m_uTargetArg2;
	ERuleSkillType				m_eSkillType;
	//下面两个变量在SkillRule每次CheckSkillDistance()的时候都可能会改变
	uint32						m_uTargetID;
	CFPos						m_TargetPos;
	static  CharacterVecType	ms_vecCharachter;
};
