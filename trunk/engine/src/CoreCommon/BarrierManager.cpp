#include "stdafx.h"
#include "BarrierManager.h"
#include "MapInfoManager.inl"
#include "CPos.h"
#include "IDynamicBarrierInfo.h"
#include "TSqrAllocator.inl"

CBarrierManager::CBarrierManager(CMapInfoManager* pMapInfo, const VecBarrierMatrixSharedPtr* pVecBarrierMaxtrixSharedPtr)
	: m_MapInfo(pMapInfo)
	, m_eJumpBarrier(eBT_NoBarrier)
	, m_pBarrierWeight(NULL)
	, m_pBarrier(pVecBarrierMaxtrixSharedPtr ? *pVecBarrierMaxtrixSharedPtr : VecBarrierMatrixSharedPtr(new VecBarrierMatrix(pMapInfo->GetQuarWidth() * pMapInfo->GetQuarHeight(), eBT_NoBarrier)))
	, m_pDynamicBarrier(NULL)
{
}

CBarrierManager::~CBarrierManager()
{
}

void CBarrierManager::SetJumpBarrier(EBarrierType eBarrier,EFindPathType EFindType,uint32 uBeginIndex,uint32 uEndIndex)
{
	m_eJumpBarrier = eBarrier;
	m_eFindType    = EFindType;
	m_uBeginIndex  = uBeginIndex;
	m_uEndIndex    = uEndIndex;
	static float uBarrierWeight1[4] = {1.5f, 0.7f, 99999999999.f, 99999999999.f};	//与连同区域预处理时使用的 权值一致.
	static float uBarrierWeight2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	if(eFPT_HypoAStarOptiUseWeight == EFindType || eFPT_AStarUseWeight == EFindType)
		m_pBarrierWeight = uBarrierWeight1;
	else
		m_pBarrierWeight = uBarrierWeight2;
}

bool CBarrierManager::JumpBarrier(uint32 uIndex)const
{
	Ast( uIndex < m_MapInfo->GetQuarWidth() * m_MapInfo->GetQuarHeight() )
	if ( uIndex == m_uBeginIndex )
		return true;

	if ( uIndex == m_uEndIndex && (m_eFindType == eFPT_LineIgnoreEnd || m_eFindType == eFPT_AStarIgnoreEnd))
		return true;

	// 当前Grid的障碍等级小于或等于可忽略的障碍等级
	if (m_pDynamicBarrier != NULL)
		return (max(static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03), GetDynamicBarrierType(uIndex))) <= m_eJumpBarrier;
	else
		return ((*m_pBarrier)[uIndex]&0x03) <= m_eJumpBarrier;
}

//只用在了A*里
bool CBarrierManager::JumpBarrierAndGetWeight(uint32 uIndex, float& uBarrierWeight)const
{
	EBarrierType eBarrierType;
	if (m_pDynamicBarrier != NULL)
		eBarrierType= max(static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03), GetDynamicBarrierType(uIndex)) ;
	else
		eBarrierType = static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03);


	if ( uIndex == m_uBeginIndex )
	{
		uBarrierWeight = m_pBarrierWeight[eBT_NoBarrier];
		return true;
	}

	if ( uIndex == m_uEndIndex &&  m_eFindType == eFPT_AStarIgnoreEnd)
	{
		uBarrierWeight = m_pBarrierWeight[eBT_NoBarrier];
		return true;
	}

	uBarrierWeight = m_pBarrierWeight[eBarrierType];

	if (eBarrierType <= m_eJumpBarrier)
	{
		return true;
	}
	return false;
}

bool CBarrierManager::IsBarrier(int32 x, int32 y)const
{
	if ( x < 0
		|| x >= static_cast<int32>(m_MapInfo->GetQuarWidth())
		|| y < 0
		|| y >= static_cast<int32>(m_MapInfo->GetQuarHeight()) )
		return true;

	uint32 uIndex = m_MapInfo->GetIndex(x, y);
	if (m_pDynamicBarrier != NULL)
		return (max(static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03), GetDynamicBarrierType(uIndex))) > m_eJumpBarrier;
	else
		return ((*m_pBarrier)[uIndex]&0x03) > m_eJumpBarrier;
}

bool CBarrierManager::CheckRangeBarrier(const CPos& grid)const
{
	static int32 nOffset[8][2] = {{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}};

	if ( grid.x == 0 || grid.y == 0 || grid.x == (int32)m_MapInfo->GetQuarWidth() || grid.y == (int32)m_MapInfo->GetQuarHeight())
		return true;

	uint32 uIndex = 0;
	int i = 0;
	for (i = 0; i < 8; ++i)
	{
		uIndex = m_MapInfo->GetIndex(grid.x + nOffset[i][0], grid.y + nOffset[i][1]);
		if (m_pDynamicBarrier != NULL)
		{
			if ( (max(static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03), GetDynamicBarrierType(uIndex)) ) <= m_eJumpBarrier )
				return true;
		}
		else
		{
			if ( ((*m_pBarrier)[uIndex]&0x03) <= m_eJumpBarrier )
				return true;
		}
	}

	return false;
}

void CBarrierManager::SetDynamicBarrier(const IDynamicBarrierInfo* pDynamicBarrier)
{
	m_pDynamicBarrier = pDynamicBarrier;
}

void CBarrierManager::SetBarrier(uint32 x, uint32 y, uint8 bVal)
{
	Ast(x < m_MapInfo->GetWidth() && y < m_MapInfo->GetHeight())
	uint32 DoubleX = x << 1;
	uint32 DoubleY = y << 1;
	//VecBarrierMatrixSharedPtr* pMutableBarrierMatrix = const_cast<VecBarrierMatrixSharedPtr*>(m_pBarrier);
	(*m_pBarrier)[m_MapInfo->GetIndex(DoubleX, DoubleY)]     = bVal;
	(*m_pBarrier)[m_MapInfo->GetIndex(DoubleX+1, DoubleY)]   = bVal;
	(*m_pBarrier)[m_MapInfo->GetIndex(DoubleX, DoubleY+1)]   = bVal;
	(*m_pBarrier)[m_MapInfo->GetIndex(DoubleX+1, DoubleY+1)] = bVal;
}

EBarrierType CBarrierManager::GetBarrierType(int32 x, int32 y)const
{
	if(x < 0 || x >= static_cast<int32>(m_MapInfo->GetWidth()) || y < 0 || y >= static_cast<int32>(m_MapInfo->GetHeight()))
		return eBT_OutRange;

	uint32 uIndex = m_MapInfo->GetIndex(x<<1,  y<<1);
	if (m_pDynamicBarrier != NULL)
		return max(static_cast<EBarrierType>((*m_pBarrier)[uIndex]&0x03), GetDynamicBarrierType(uIndex));
	else
		return static_cast<EBarrierType>( (*m_pBarrier)[uIndex]&0x03 );
}

EBarrierType CBarrierManager::GetQuarBarrierType(int32 x, int32 y)const
{
	if(x < 0 || x >= static_cast<int32>(m_MapInfo->GetQuarWidth()) || y < 0 || y >= static_cast<int32>(m_MapInfo->GetQuarHeight()))
		return eBT_OutRange;

	uint32 uIndex = m_MapInfo->GetIndex(x,  y);
	return static_cast<EBarrierType>( (*m_pBarrier)[uIndex]&0x03 );
}

EBarrierType CBarrierManager::GetDynamicBarrierType(uint32 uIndex)const
{
	if(m_pDynamicBarrier != NULL)
	{
		return m_pDynamicBarrier->GetBarrier(uIndex);
	}
	return eBT_NoBarrier;
}

const CBarrierManager::VecBarrierMatrixSharedPtr* CBarrierManager::GetBarrierMatrix()const
{
	return &m_pBarrier;
}

