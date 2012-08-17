#include "stdafx.h"
#include "CAsyAoiDimScene.h"
#include "CAsyAoiDimObj.h"
#include "CAsyAoiScene.h"
#include "TSqrAllocator.inl"

CAsyAoiDimScene::CAsyAoiDimScene(CAsyAoiScene* pScene, uint32 uPrecision)
: m_pScene(pScene)
, m_uPrecision(uPrecision)
{

}

CAsyAoiDimScene::~CAsyAoiDimScene()
{

}

void CAsyAoiDimScene::Release()
{
	delete this;
}

bool CAsyAoiDimScene::IsInSyncAoi(const CPos& OffsetGridPos) const
{
	return m_pScene->IsInSyncAoi(OffsetGridPos);
}

uint32 CAsyAoiDimScene::GetPrecision() const
{
	return m_uPrecision;
}

void CAsyAoiDimScene::SetPrecision(uint32 uPrecision)
{
	m_uPrecision = uPrecision;
}

