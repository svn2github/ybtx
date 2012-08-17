#pragma once
#include "CTick.h"
#include "TSingleton.h"

class CFightSkillProf 
	: public CTick
	, public TSingleton<CFightSkillProf>
{
	friend class TSingleton<CFightSkillProf>;
public:
	static CFightSkillProf* GetProf();
	void TurnOnFightSkillProf(uint32 uInterval);	//毫秒为单位
	void TurnOffFightSkillProf();

private:
	CFightSkillProf();
	virtual ~CFightSkillProf();
	virtual void OnTick();

	ofstream*		m_fstrmProf;
	vector<uint32>	m_vecLastData;	//保存上次tick时的数据，以便统计该次tick中函数调用的次数
};
