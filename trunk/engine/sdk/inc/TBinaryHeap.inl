#pragma once
#include "TBinaryHeap.h"
#include "ExpHelper.h"

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::TBinaryHeap(uint32 nInitSize /*=4*/)
	:m_nNum(0)
	,m_nAllotSize(nInitSize + 2)
{
	if (m_nAllotSize < 4)
	{
		m_nAllotSize = 4;
	}

	m_vecKeyHeap.resize(m_nAllotSize);
	m_vecValueHeap.resize(m_nAllotSize);

}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::~TBinaryHeap()
{
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
void TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::re_allot()
{
	m_nAllotSize <<=1;

	m_vecKeyHeap.resize(m_nAllotSize);
	m_vecValueHeap.resize(m_nAllotSize);
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
const T_Value& TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::top()const
{
	Ast(m_nNum > 0);
	return m_vecValueHeap[1];
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
bool TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::empty()const
{
	return m_nNum == 0;
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
uint32 TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::size()const
{
	return m_nNum;
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
inline void TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::move(uint32 destIndex, uint32 srcIndex)
{
	m_vecKeyHeap[destIndex] = m_vecKeyHeap[srcIndex];
	m_vecValueHeap[destIndex] = m_vecValueHeap[srcIndex];
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
void TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::push(T_Key key, const T_Value& value)
{
	uint32 index = m_nNum + 1;
	uint32 fatherIndex = index >> 1;
	while (fatherIndex > 0 && m_Compare(key, m_vecKeyHeap[fatherIndex]))
	{
		move(index,fatherIndex);
		index = fatherIndex;
		fatherIndex = index >> 1;
	}
	m_vecKeyHeap[index] = key;
	m_vecValueHeap[index] = value;
	++m_nNum;
	if (m_nNum + 1 == m_nAllotSize) //前面空一个
	{
		re_allot();
	}
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
void TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::pop()
{
	Ast(m_nNum > 0);
	T_Key endKey = m_vecKeyHeap[m_nNum];
	--m_nNum;
	uint32 index = 1;
	uint32 leftChildIndex = index << 1;
	uint32 rightChildIndex = leftChildIndex + 1;
	while (leftChildIndex <= m_nNum)
	{
		if (leftChildIndex == m_nNum)//只有左子树
		{
			if (m_Compare(m_vecKeyHeap[leftChildIndex], endKey))
			{
				move(index,leftChildIndex);
				index = leftChildIndex;
			}
			break;
		}
		else if(m_Compare(m_vecKeyHeap[rightChildIndex], m_vecKeyHeap[leftChildIndex]))//右子树优先
		{
			if (m_Compare(m_vecKeyHeap[rightChildIndex], endKey))
			{
				move(index,rightChildIndex);
				index = rightChildIndex;
				leftChildIndex = index << 1;
				rightChildIndex = leftChildIndex + 1;
			}
			else
			{
				break;
			}
		}
		else
		{
			if (m_Compare(m_vecKeyHeap[leftChildIndex], endKey))
			{
				move(index,leftChildIndex);
				index = leftChildIndex;
				leftChildIndex = index << 1;
				rightChildIndex = leftChildIndex + 1;
			}
			else
			{
				break;
			}
		}
	}
	m_vecKeyHeap[index] = endKey;
	m_vecValueHeap[index] = m_vecValueHeap[m_nNum + 1];
}

template<typename T_Key, typename T_Value, typename T_CompFunc, typename KeyAlloc, typename ValueAlloc>
void TBinaryHeap<T_Key, T_Value, T_CompFunc, KeyAlloc, ValueAlloc>::clear()
{
	m_nNum = 0;
}
