#include "stdafx.h"
#include "CDoSkillCtrlMgr.h"
#include "ExpHelper.h"

CDoSkillCtrlMgr::CDoSkillCtrlMgr()
{
	m_vecSkillCtrlState.resize(eDSCS_Max, 0);
}

CDoSkillCtrlMgr::~CDoSkillCtrlMgr()
{
	m_vecSkillCtrlState.clear();
}

void CDoSkillCtrlMgr::SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet)
{
	Ast(eSkillCtrlState < eDSCS_Max);
	if (bSet)
	{
		m_vecSkillCtrlState[eSkillCtrlState] = m_vecSkillCtrlState[eSkillCtrlState] + 1;
	}
	else
	{
		if (m_vecSkillCtrlState[eSkillCtrlState] == 0)
		{
			if (eSkillCtrlState > eDSCS_CountBegin)
			{
				stringstream sErrArg;
				sErrArg << "½û¼¼ÄÜÖµ : " << eSkillCtrlState << " ";
				GenErr("CDoSkillCtrlMgr::SetDoSkillCtrlState´íÎó\n", sErrArg.str());
			}
			return;
		}
		m_vecSkillCtrlState[eSkillCtrlState] = m_vecSkillCtrlState[eSkillCtrlState] - 1;
	}
}

bool CDoSkillCtrlMgr::GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState)
{
	return m_vecSkillCtrlState[eSkillCtrlState]>0;
}

void CDoSkillCtrlMgr::RevertSkillCtrlState()
{
	m_vecSkillCtrlState.clear();
	m_vecSkillCtrlState.resize(eDSCS_Max, 0);
}
