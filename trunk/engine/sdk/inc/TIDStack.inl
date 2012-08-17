#pragma once
#include "TIDStack.h"
#include "ExpHelper.h"

namespace sqr
{


template<typename IDType, class IDContainer>
TIDStack<IDType, IDContainer>::TIDStack()
: m_CurIdlePos(0)
{
}

template<typename IDType, class IDContainer>
TIDStack<IDType, IDContainer>::TIDStack(size_t stCount)
: m_CurIdlePos(0)
{
	m_IDStack.resize(stCount);
	m_IDIndex.resize(stCount);
	for(size_t i = 0; i < stCount; ++i)
	{
		m_IDStack[i] = i;
		m_IDIndex[i] = i;
	}
}

template<typename IDType, class IDContainer>
TIDStack<IDType, IDContainer>::~TIDStack()
{
	
}

template<typename IDType, class IDContainer>
IDType TIDStack<IDType, IDContainer>::WithdrawIdleID()
{
	IDType ID;
	if(m_CurIdlePos == m_IDStack.size())
	{
		ID = (IDType)m_CurIdlePos;
		m_IDStack.push_back(IDSTACK_INVALID_INDEX);

		++m_CurIdlePos;

		m_IDIndex.push_back(ID);
		m_IDIndex[ID] = IDSTACK_INVALID_INDEX;
	}
	else
	{
		ID = m_IDStack[m_CurIdlePos];
		m_IDStack[m_CurIdlePos] = IDSTACK_INVALID_INDEX;

		++m_CurIdlePos;

		m_IDIndex[ID] = IDSTACK_INVALID_INDEX;
	}
	
	return ID;
}

template<typename IDType, class IDContainer>
void TIDStack<IDType, IDContainer>::RestoreUsedID(IDType ID)
{
	Ast(m_CurIdlePos > 0);

	--m_CurIdlePos;
	m_IDStack[m_CurIdlePos] = ID;

	m_IDIndex[ID] = m_CurIdlePos;
}

template<typename IDType, class IDContainer>
bool TIDStack<IDType, IDContainer>::IsIdleID(IDType ID)
{
	if(!IsStackID(ID))
		return false;

	return m_IDIndex[ID] != IDSTACK_INVALID_INDEX;	
}

template<typename IDType, class IDContainer>
void TIDStack<IDType, IDContainer>::RemoveIdleID(IDType ID)
{
	size_t stIndex = m_IDIndex[ID];
	
	if(m_CurIdlePos == stIndex)
	{
		//要移除的就是第一个idle
		m_IDIndex[ID] = IDSTACK_INVALID_INDEX;
		m_IDStack[m_CurIdlePos] = IDSTACK_INVALID_INDEX;
		++m_CurIdlePos;
	}
	else
	{
		IDType IdleID = m_IDStack[m_CurIdlePos];
		m_IDStack[stIndex] = IdleID;
		m_IDStack[m_CurIdlePos] = IDSTACK_INVALID_INDEX;

		++m_CurIdlePos;

		m_IDIndex[ID] = IDSTACK_INVALID_INDEX;
		m_IDIndex[IdleID] = stIndex;	
	}
}

template<typename IDType, class IDContainer>
void TIDStack<IDType, IDContainer>::ExpandIdleID(IDType MaxID)
{
	size_t stSize = m_IDStack.size();
	Ast(MaxID >= stSize);

	for(uint32 i = stSize; i <= MaxID; ++i)
	{
		m_IDStack.push_back(i);
		m_IDIndex.push_back(i);
	}
}

template<typename IDType, class IDContainer>
bool TIDStack<IDType, IDContainer>::IsStackID(IDType ID)
{
	size_t stSize = m_IDStack.size();
	return ID < stSize;
}

template<typename IDType, class IDContainer>
size_t TIDStack<IDType, IDContainer>::GetIdleIDSize()
{
	return (m_IDStack.size() - m_CurIdlePos);
}

template<typename IDType, class IDContainer>
IDType TIDStack<IDType, IDContainer>::GetIDSize()
{
	return m_IDIndex.size();
}

template<typename IDType, class IDContainer>
bool TIDStack<IDType, IDContainer>::Empty()
{
	return GetIdleIDSize() == 0;
}

}
