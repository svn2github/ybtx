#pragma once
#include "MapInfoManager.h"
#include "ExpHelper.h"

inline uint32 CMapInfoManager::GetQuarWidth()
{
	return m_uQuarterWidth;
}

inline uint32 CMapInfoManager::GetQuarHeight()
{
	return m_uQuarterHeight;
}

inline uint32 CMapInfoManager::GetWidth()
{
	return m_uWidth;
}

inline uint32 CMapInfoManager::GetHeight()
{
	return m_uHeight;
}

inline uint32 CMapInfoManager::IndexError(uint32 x, uint32 y)
{
	if(x < m_uQuarterWidth && y < m_uQuarterHeight)
		return 0;
	else
		return 1;
}

inline uint32 CMapInfoManager::GetIndex(uint32 x, uint32 y)
{
	Ast(x < m_uQuarterWidth && y < m_uQuarterHeight)
	return (y * m_uQuarterWidth + x);
}

inline bool CMapInfoManager::CheckXYInRegion(int32 x, int32 y)
{
	if(x < 0 || x >= static_cast<int32>(m_uQuarterWidth) || y < 0 || y >= static_cast<int32>(m_uQuarterHeight))
		return false;
	else
		return true;
}

