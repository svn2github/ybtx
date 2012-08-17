#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class CConnStress;
}

class CFighterStress
	:public virtual CDynamicObject
{
public:
	CFighterStress(){}
	~CFighterStress(){}
	static CFighterStress* InitFighterStress(CConnStress* pConn);
	void RemoveFighterStress();
	uint32 GetGlobalID(){return m_uGlobalID;}
	void	DoNormalSkill( uint32 uSkillID, uint8 uSkillLevel, float uPosFromX, float uPosFromY,uint32 uTargetID);
private:
	template<typename CmdType>
	bool						SendToGas(const CmdType* pCmd)const;
	uint32 m_uGlobalID;
	CConnStress* m_pConn;
};
