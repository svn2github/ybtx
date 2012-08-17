#include "stdafx.h"
#include "CBloodRenderDialogPool.h"
#include "CBloodTypeCfg.h"
#include "TSqrAllocator.inl"

CBloodRenderDialogPool::CBloodRenderDialogPool()
:m_PoolSize(15)
{
}

CBloodRenderDialogPool::~CBloodRenderDialogPool()
{
	Release();
}

void CBloodRenderDialogPool::CreatePool(SQRWnd* ParentWnd)
{
	for (uint32 i = 0; i< m_PoolSize; i++)
	{
		HeadBlood* m_HeadBlood = new HeadBlood();
		m_HeadBlood->CreateBloodRendlerDialog(ParentWnd, false, eBDT_Normal);
		m_mapHeadBloodDialog[eBDT_Normal].push_back( m_HeadBlood );

		HeadBlood* m_HeadBloodGA = new HeadBlood();
		m_HeadBloodGA->CreateBloodRendlerDialog(ParentWnd, false, eBDT_GAttack);
		m_mapHeadBloodDialog[eBDT_GAttack].push_back( m_HeadBloodGA );

		HeadBlood* m_HeadBloodSA = new HeadBlood();
		m_HeadBloodSA->CreateBloodRendlerDialog(ParentWnd, false, eBDT_SuperNpc);
		m_mapHeadBloodDialog[eBDT_SuperNpc].push_back( m_HeadBloodSA );
	}
}

HeadBlood* CBloodRenderDialogPool::GetHeadBloodDialog(SQRWnd* ParentWnd, EClass eClass)
{
	EBloodDialogType eBDType = CBloodTypeCfg::GetBloodType(eClass);
	HeadBlood* pHeadBlood = NULL;
	if( m_mapHeadBloodDialog[eBDType].empty() )
	{
		pHeadBlood = new HeadBlood();
		pHeadBlood->CreateBloodRendlerDialog(ParentWnd, false, eBDType);
		return pHeadBlood;
	}
	pHeadBlood = m_mapHeadBloodDialog[eBDType].back();
	m_mapHeadBloodDialog[eBDType].pop_back();
	return pHeadBlood;
}

void CBloodRenderDialogPool::ReleaseHeadBloodDialog(HeadBlood* pWnd, EClass eClass)
{
	EBloodDialogType eBDType = CBloodTypeCfg::GetBloodType(eClass);
	m_mapHeadBloodDialog[eBDType].push_back( pWnd );
}

void CBloodRenderDialogPool::Release()
{
	MapHeadBloodDialog::iterator itrBegin = m_mapHeadBloodDialog.begin();
	MapHeadBloodDialog::iterator itrEnd   = m_mapHeadBloodDialog.end();
	for (itrBegin; itrBegin != itrEnd; ++itrBegin)
	{
		ClearVector(itrBegin->second);
	}
	m_mapHeadBloodDialog.clear();
}