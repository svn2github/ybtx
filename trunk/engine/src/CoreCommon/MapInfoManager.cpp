
#include "stdafx.h"
#include "MapInfoManager.h"
#include "CoreCommon.h"


CMapInfoManager::CMapInfoManager(uint32 uWidthInRegion,uint32 uHeightInRegion)
{
	m_uWidth = uWidthInRegion * eRegionSpan;
	m_uHeight = uHeightInRegion * eRegionSpan;

	m_uQuarterWidth  = m_uWidth  * 2;
	m_uQuarterHeight = m_uHeight * 2;
}

CMapInfoManager::~CMapInfoManager()
{
}

