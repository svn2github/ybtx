#pragma once
#include "CDynamicObject.h"
#include "CCoolDownInfo.h"
#include "TFighterAllocator.h"

typedef map<string, CCoolDownInfo*, less<string>, 
	TFighterAllocator<pair<const string, CCoolDownInfo*> > > CoolDownInfoMap_t;

template<typename App_t, typename Fight_t>
class TCoolDownMgr
	:public virtual CDynamicObject
	,public CFighterMallocObject
{
public:
	TCoolDownMgr(Fight_t* pFighter);
	~TCoolDownMgr(void);

	static uint32 ms_uFightSkillCommonCDTime;	// 所有技能的公共冷却时间
	static uint32 ms_uItemSkillCommonCDTime;	// 药品类1技能的公共冷却时间

	//触发一个新的技能或者物品cooldown调用这个函数
	//return true表示触发成功
	//return false表示该cooldown类型还没有cooldown完毕，触发失败
	CCoolDownInfo* IntNewCoolDown( const TCHAR* szName, ESkillCoolDownType eCoolDownType, uint32 uCoolDownTime, bool bSwitchEquipSkill = false);

	//获得某个cooldown类型的剩余cooldown时间，uCoolDownTime是技能的cooldown总时间
	uint32 GetCoolDownTime( const TCHAR* szName, ESkillCoolDownType eCoolDownType )const;

	//检查某个技能是否已经cooldown
	bool IsCoolDown( const TCHAR* szName, ESkillCoolDownType eCoolDownType)const;

	void ClearCommonCD();
	bool IsInCommonCD( const TCHAR* szName, ESkillCoolDownType eCoolDownType );
	bool IsSwitchEquipSkill( const TCHAR* szName );
	// 将所有技能CoolDownTime结束
	void ResetAllCoolDown();
	void ResetCoolDownByCoolDownType(ESkillCoolDownType eCoolDownType);

	CoolDownInfoMap_t& GetCoolDownInfoMap();
	Fight_t* GetFighter() {return m_pFighter;}

private:
	uint64 m_uComCoolDownBeginTime;			// 战斗技能公共冷却开始时刻
	uint64 m_uDrugItemSkillCDBeginTime;		// 恢复类冷却开始时刻
	uint64 m_uSpecialItemSkillCDBeginTime;	// 特殊类物品技能冷却开始时刻
	

	CoolDownInfoMap_t	m_mapCoolDownInfo;
	Fight_t*			m_pFighter;
};
