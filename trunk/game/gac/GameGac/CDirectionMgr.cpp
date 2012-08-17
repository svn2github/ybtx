#include "stdafx.h"
#include "CDirectionMgr.h"
#include "CAppClient.h"
#include "CRenderObject.h"
#include "CCharacterFollower.h"
#include "CFighterFollower.h"

static const uint8 TURN_INTERVAL	= 15;		// 旋转单位时间(毫秒)
static const float FRICTION			= 0.5f;		// 摩擦阻力 (朝向/单位时间^2)

CDirectionMgr::CDirectionMgr(CCharacterFollower* pCharacter)
: m_pCharacter(pCharacter)
, m_bInitDirection(true)
, m_uDesDirection( 0 )
, m_uCurDirection( 160 )
, m_uStartUpSpeed(0)
, m_fFirstDexterity(0.0f)
, m_fDexterity(1.0f)
, m_uTurnCount(0)
{
}

CDirectionMgr::~CDirectionMgr()
{
	CancelTick();
	DetachAll();
}

void CDirectionMgr::SetDirection( uint8 uDir, bool bAtOnce)
{
	//立刻改变渲染方向，不需要旋转
	if( bAtOnce || m_bInitDirection)
	{
		// Variant回调如果没注销，切场景RenderObject会未初始就收到设置角度
		if (m_pCharacter->GetHorseRenderObj())
		{
			m_uDesDirection = m_uCurDirection = uDir;
			m_pCharacter->GetHorseRenderObj()->SetDirection( CDir(uDir) );
			m_bInitDirection = false;
		}
		return;
	}

	m_uDesDirection = uDir;
	uint8 uDiffDir = m_uDesDirection - m_uCurDirection;
	uDiffDir = uDiffDir > 128 ? 256 - uDiffDir : uDiffDir;
	m_uStartUpSpeed = uDiffDir/16 + 5;		// 启动转速 (估算的公式可调整)

	if (m_fFirstDexterity != 0.0f)
	{
		m_fDexterity = m_fFirstDexterity;
	}
	else
	{
		float fCurSpeed = m_pCharacter->GetSpeed();
		if (fCurSpeed == 0.0f)
			m_fDexterity = 1.0f;
		else
			m_fDexterity = (fCurSpeed > 1.2f) ? fCurSpeed / 3.0f : 0.4f;
	}

	CancelTick();
	CAppClient::Inst()->RegisterTick( this, TURN_INTERVAL );
}

void CDirectionMgr::CancelTick()
{
	m_uTurnCount = 0;
	CAppClient::Inst()->UnRegisterTick( this );
}

void CDirectionMgr::OnTick()
{
	if (!m_pCharacter->CppIsAlive())
	{
		CancelTick();
		return;
	}
	
	if( m_uCurDirection != m_uDesDirection )
	{
		bool bPlayPace = !m_pCharacter->GetIsPlayer() && !m_pCharacter->IsMoving() && (m_pCharacter->GetActor()->GetCurOnceActionLevel() == eAPP_NULL);
		TCHAR nDel = '0';
		nDel = (TCHAR)( m_uDesDirection - m_uCurDirection );	
		uint8 uTurnSpeed = uint8((m_uStartUpSpeed - FRICTION * m_uTurnCount) * m_fDexterity);
		if (uTurnSpeed <= 2) uTurnSpeed = 2;
		if( abs( nDel ) <= uTurnSpeed/* || 0 == uTurnSpeed */)
		{
			m_uCurDirection = m_uDesDirection;
			if (bPlayPace)
			{
				CFighterFollower* pTarget = m_pCharacter->GetFighter()->GetTarget();
				if (pTarget && pTarget->GetCharacter()->IsMoving())
				{
					EActionState eAction = nDel > 0 ? eAS_Pace_Right : eAS_Pace_Left;
					pTarget->Attach(this, eCE_MoveEnd);
					m_pCharacter->GetActor()->DoActionSimplify(eAction);
				}
				else
				{
					m_pCharacter->DoMoveState(eMS_Stop);
				}
			}
		}
		else if( nDel > 0 )
		{
			m_uCurDirection = (uint8)( m_uCurDirection + uTurnSpeed );
			if (bPlayPace)
				m_pCharacter->GetActor()->DoActionSimplify(eAS_Pace_Right);
		}
		else
		{
			m_uCurDirection = (uint8)( m_uCurDirection - uTurnSpeed );
			if (bPlayPace)
				m_pCharacter->GetActor()->DoActionSimplify(eAS_Pace_Left);
		}
		if (m_pCharacter->GetHorseRenderObj())
			m_pCharacter->GetHorseRenderObj()->SetDirection( CDir( m_uCurDirection ) );
		++ m_uTurnCount;
	}
	else
		CancelTick();
}

void CDirectionMgr::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent, void* pArg)
{
	CFighterFollower* pTarget = m_pCharacter->GetFighter()->GetTarget();
	if(pTarget && pSubject == pTarget)
	{
		switch(uEvent)
		{
		case eCE_MoveEnd:
			m_pCharacter->DoMoveState(eMS_Stop);
			break;
		}
		this->DetachAll();
	}
}

void CDirectionMgr::SetFirstDexterity(float fValue)
{
	m_fFirstDexterity = fValue;
}

bool CDirectionMgr::IsStopBeforeTurn(uint8 uDesDirection)
{
	uint8 uTemp = abs(m_uCurDirection - uDesDirection);
	if( uTemp > 68 && uTemp < 188)
		return true;
	else
		return false;
}

bool CDirectionMgr::IsTurnOver()
{
	if (m_uDesDirection == m_uCurDirection)
		return true;

	uint8 uTemp = abs(m_uCurDirection - m_uDesDirection);
	if( uTemp < 64 || uTemp > 192)
		return true;
	else
		return false;
}
