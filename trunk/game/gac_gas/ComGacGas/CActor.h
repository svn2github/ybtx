#pragma once
#include "ActorDef.h"
#include "CDynamicObject.h"
#include "CDir.h"
#include "CActorMallocObject.h"

class CActor 
	: public virtual CDynamicObject
	, public CActorMallocObject
{
public:
	CActor();

	bool ChangeMoveState(EMoveState eMoveState, bool bForce = false);
	bool ChangeActionState(EActionState eActionState);
	
	EMoveState		GetPreMoveState() const				{ return m_ePreMoveState; }
	EMoveState		GetMoveState() const				{ return m_eMoveState; }
	EActionState	GetPreActionState() const			{ return m_ePreActionState; }
	EActionState	GetActionState() const 				{ return m_eActionState; }

	// 根据枚举值查询名字, 用于CONSOLE打印
	const TCHAR* GetMoveStateStr(EMoveState eMoveState) const;
	const TCHAR* GetActionStateStr(EActionState eActionState) const;

private:
	bool CheckMoveState(EMoveState eMoveState);
	bool CheckActionState(EActionState eActionState);

	EMoveState		m_ePreMoveState;		// 上个移动状态
	EMoveState		m_eMoveState;			// 移动状态
	EActionState	m_ePreActionState;		// 上个动作状态
	EActionState	m_eActionState;			// 动作状态
};
