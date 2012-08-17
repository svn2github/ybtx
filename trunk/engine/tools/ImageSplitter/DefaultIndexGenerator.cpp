#include "stdafx.h"
#include "DefaultIndexGenerator.h"

// ---- 类型实现 ------------------------------------------------------------------------------

	CDefaultIndexGenerator::CDefaultIndexGenerator()
		: m_uWidth(0)
		, m_uHeight(0)
		, m_uIndexCount(0)
		, m_uCurrIndex(0)
	{

	}

	void CDefaultIndexGenerator::SetRaginInfo( uint32 uWidth, uint32 uHeight )
	{
		m_uWidth		= uWidth;
		m_uHeight		= uHeight;
		m_uIndexCount	= uWidth * uHeight;
		m_uCurrIndex	= 0;
	}

	bool CDefaultIndexGenerator::GetNextIndex( IndexStruct *sIndex )
	{
		if ( m_uCurrIndex == m_uIndexCount )
		{
			return false;
		}

		uint32 x = m_uCurrIndex / m_uHeight;
		uint32 y = m_uCurrIndex % m_uHeight;

		string		 szName;
		stringstream ss;
		ss << "M_" << x << "_" << y;
		ss >> szName;

		sIndex->x	 = x;
		sIndex->y	 = m_uHeight - y - 1;
		sIndex->name = szName;

		++m_uCurrIndex;
		return true;
	}

	void CDefaultIndexGenerator::Release()
	{
		delete this;
	}