#include "stdafx.h"
#include "StateMgr.h"

EMainState CStateMgr::m_eCurState = eMS_NONE;
EMainState CStateMgr::m_eFrontState = eMS_NONE;
int		   CStateMgr::m_iAssistantState = eAS_NONE;


bool CStateMgr::IsState(EMainState eState)
{
	return m_eCurState == eState;
}

bool CStateMgr::IsFronState(EMainState eState)
{
	return m_eFrontState == eState;
}

bool CStateMgr::IsAssistState(int mark)
{
	return (m_iAssistantState & mark) == mark;
}

EMainState CStateMgr::GetCurState()
{
	return m_eCurState;
}


CStateMgr::CStateMgr(void)
{
	SetState(eMS_NONE);
	REGISTER_STATER_TABLE
}

CStateMgr::~CStateMgr(void)
{
	UNREGISTER_STATER_TABLE
}

CStateMgr& CStateMgr::Inst()
{
	static CStateMgr instance;
	return instance;
}



void CStateMgr::DoEvent(EEvent eEvent)
{
	UpdateAssistantState(eEvent);
	EMainState distState = GetDistState(eANY_STATE, eEvent);//先到所有状态的共同事件处理中查找
	if (distState == eANY_STATE)//未找到再到但前状态中查找
	{
		distState = GetDistState(m_eCurState, eEvent);
	}
	if (distState == eANY_STATE)
	{
		cout << "eMS_ANY_STATE 只是概念性的 所有状态, 不能作为 ADD_TRANSIT_STATE 的跳转状态" << endl;
		return;
	}
	if (distState == m_eCurState)
	{
		return;
	}
	SetState(distState);
}

void CStateMgr::UpdateAssistantState(EEvent eEvent)
{
	if (eEvent == eEVENT_CLEAR_AS || eEvent == eEVENT_EIXT)
	{
		RemoveAssistantState(eAS_ALL);
		return;
	}
	if(ppStateTable[eASSISTANT_STATE] == NULL)
	{
		cout << "辅助状态未关联事件  " << endl;
		return;
	}
	int n = ppStateTable[eASSISTANT_STATE][0] -1;//第一个元素 存放数组的个数
	int *pStransitTable = ppStateTable[eASSISTANT_STATE]+1;// 偏移第一个元素 
	for ( int i = 0; i < n; i+=3)
	{
		if (pStransitTable[i] == eEvent)//找到事件 i+1 则为该事件对应的状态
		{
			if (pStransitTable[i+2] == 1)
			{
				SetAssistantState(EAssistantState(pStransitTable[i+1]));
			}
			else if (pStransitTable[i+2] == 0)
			{
				RemoveAssistantState(EAssistantState(pStransitTable[i+1]));
			}
			else
			{
				cout << "ATTACH_ASSISTANT_STATE 中 " << eEvent << "  事件第3个参数 1代表添加, 0代表移除, 其他未定义" << endl;
			}
			return;
		}
	}

}



void CStateMgr::SetState(EMainState eState)
{
	if(eMS_BACK == eState)//返回之前状态
	{
		if(m_stackState.size() <= 1)
		{
			cout << " 只剩一个状态了, 无法返回状态(m_stackState 必须保留一个 eMS_NONE状态)" << endl;
			return;
		}
		m_stackState.pop_back();
		m_eCurState = m_stackState.back();
		if (m_stackState.size() < 2)
		{
			m_eFrontState = eMS_NONE;
		}
		else
		{
			m_eFrontState = m_stackState[m_stackState.size()-2];
		}
		return;
	}
	if (IsStateExisted(eState))
	{
		while (eState != m_stackState.back())
		{
			m_stackState.pop_back();
		}
	}
	else
	{
		m_stackState.push_back(eState);
	}
	m_eCurState = m_stackState.back();
	if (m_stackState.size() < 2)
	{
		m_eFrontState = eMS_NONE;
	}
	else
	{
		m_eFrontState = m_stackState[m_stackState.size()-2];
	}
}

EMainState CStateMgr::GetDistState(EMainState curState, EEvent eEvent) const
{
	if(ppStateTable[curState] == NULL)
	{
		cout << "状态  " << curState << " 没有设置任何跳转关系, 将永远停在此状态!!!" << endl;
		return curState;
	}
	int n = ppStateTable[curState][0] -1;//第一个元素 存放数组的个数  -1就是 StransitTable 的个数
	int *pStransitTable = ppStateTable[curState]+1;// 偏移第一个元素 
	for ( int i = 0; i < n; i+=2)
	{
		if (pStransitTable[i] == eEvent || pStransitTable[i] == eEVENT_THE_OTHERS)//找到事件 i+1 则为该事件对应的状态
		{
			return (EMainState)(pStransitTable[i+1]);
		}
	}
	// cout << "状态  " << curState << " 没有设置对 事件 " << eEvent << " 的处理" << endl;
	return curState;
}


bool CStateMgr::IsStateExisted(EMainState eState) const
{
	for (int i = m_stackState.size() - 1; i >=0; i--)
	{
		if (m_stackState[i] == eState)
		{
			return true;
		}
	}
	return false;
}



void CStateMgr::SetAssistantState(EAssistantState eAssistantState)
{
	if (eAssistantState == eAS_NONE)
	{
		m_iAssistantState = eAS_NONE;
		return;
	}
	m_iAssistantState |= eAssistantState;
}

void CStateMgr::RemoveAssistantState(EAssistantState eAssistantState)
{
	m_iAssistantState &= ~eAssistantState;
}