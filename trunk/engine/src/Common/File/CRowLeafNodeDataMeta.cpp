#include "stdafx.h"
#include "CRowLeafNodeDataMeta.h"
#include "TSqrAllocator.inl"
#include <algorithm>
#include "CRowTableFileKey.h"

using namespace std;

CLeafNodeDataMeta::CLeafNodeDataMeta(CRowTableFileField& RowField)
{
	int32 nNum = RowField.FieldNum();
	
	m_vecDataFieldInfo.reserve(RowField.DataNum());

	CLeafFieldInfo info;

	int32 nDataOffset = 0;
	for (int32 i = 0; i < nNum; i++)
	{
		ETableFileFieldType eType = RowField.GetFieldType(i);
		switch(eType)
		{
		case eTFFT_Empty:
			break;
		default:
			{
				info.eFieldType = eType;
				info.nDataOffset = nDataOffset++;
				const char* szKey = RowField.GetFieldKey(i);
				m_vecDataFieldInfo.push_back(make_pair(szKey, info));
			}
			break;
		}
	}

	sort(m_vecDataFieldInfo.begin(), m_vecDataFieldInfo.end(), LessField);
}

CLeafNodeDataMeta::~CLeafNodeDataMeta()
{

}


inline bool CLeafNodeDataMeta::LessField(const PairKey2Field_t& Left, const PairKey2Field_t& Right)
{
	return (size_t)Left.first < (size_t)Right.first;
}


CLeafFieldInfo& CLeafNodeDataMeta::GetFieldInfo(const char* szKey)
{
	static CLeafFieldInfo EmptyFiledInfo;

	if (m_vecDataFieldInfo.empty())
	{
		return EmptyFiledInfo;
	}

	if (!szKey)
	{
		if (m_vecDataFieldInfo[0].first != NULL)
		{
			return m_vecDataFieldInfo[0].second;
		}
		else
		{
			return EmptyFiledInfo;
		}
	}

	VecKey2Field_t::iterator iter = lower_bound(m_vecDataFieldInfo.begin(), m_vecDataFieldInfo.end(), 
		PairKey2Field_t(szKey, CLeafFieldInfo()), LessField);

	if (iter != m_vecDataFieldInfo.end())
	{
		if (iter->first == szKey)
		{
			return iter->second;
		}
	}

	return EmptyFiledInfo;
}


