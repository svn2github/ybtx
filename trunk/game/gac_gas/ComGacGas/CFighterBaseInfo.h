#pragma once
#include "GameDef.h"
#include "CDynamicObject.h"

class CFighterBaseInfo : public virtual CDynamicObject
{
public:
	EClass	CppGetClass() const;
	uint32	CppGetLevel() const;
	ECamp	CppGetBirthCamp() const;
	ECamp	CppGetCamp() const;
	int32	CppGetGameCamp() const;
	bool	CppGetPKState() const;
	//EPKState	CppGetPKState() const;
	//EZoneType	CppGetZoneType() const;

protected:
	CFighterBaseInfo();
	void SetClass(EClass eClass);
	void SetLevel(uint32 uLevel);
	void SetBirthCamp(ECamp eCamp);
	void SetCamp(ECamp eCamp);
	void SetGameCamp(int32 iGameCamp);
	void SetPKState(bool bPKState);
	//void SetPKState(EPKState ePKState);
	//void SetZoneType(EZoneType eZoneType);

private:
	uint32		m_uLevel;		// 等级
	EClass		m_eClass;		// 职业
	ECamp		m_eBirthCamp;	// 出生阵营(选好后不变)
	ECamp		m_eCamp;		// 当前阵营
	int32		m_iGameCamp;	// 玩法活动阵营
	bool		m_bPKState;		// PK状态
	//EPKState	m_ePKState;		// PK状态
	//EZoneType	m_eZoneType;	// 所在区域类型及所属
};

