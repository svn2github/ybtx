#pragma once
#include "CTick.h"
#include "PatternCOR.h"

class CCharacterFollower;

class CDirectionMgr 
	: public CTick
	, public CPtCORHandler
{
public:
	CDirectionMgr(CCharacterFollower* pCharacter);
	~CDirectionMgr();

	void SetDirection( uint8 uDir, bool bAtOnce);
	void OnTick();
	void CancelTick();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void SetFirstDexterity(float fValue);
	void InitDirection() {m_bInitDirection = true;}

	// FOR LUA
	bool IsStopBeforeTurn(uint8 uDesDirection);			// 是否旋转前先站立 (目标超过90度)
	bool IsTurnOver();									// 是否转到半角度 (旋转前与目标)

private:
	CCharacterFollower*	m_pCharacter;
	bool			m_bInitDirection;	// 是否初始角度
	uint8			m_uCurDirection;	// 当前的角度
	uint8			m_uDesDirection;	// 将要旋转到的角度 (这个角度是人物下半身也就是站立时候的角度)
	uint8			m_uStartUpSpeed;	// 启动时转速 (朝向/单位时间)
	float			m_fFirstDexterity;	// 优先敏捷度 (影响转向的速度)
	float			m_fDexterity;		// 敏捷度 (无优先时，按移动速度计算)
	uint8			m_uTurnCount;		// 当前旋转次数计数
};
