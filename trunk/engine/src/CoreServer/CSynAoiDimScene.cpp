#include "stdafx.h"
#include "CSynAoiScene.h"
#include "CSynAoiDimScene.h"
#include "CSynAoiDimObj.h"
#include "CRectangle.h"
#include "CPos.h"
#include "CAoiQuadTree.h"
#include "QuadTreeFun.h"

CSynAoiDimScene::CSynAoiDimScene(CSynAoiScene* pScene, uint32 uWidth, 
								 uint32 uHeight, uint32 uPrecision)
: CAoiQuadTree(uWidth, uHeight, 16 * uPrecision)
, m_pScene(pScene)
, m_uPrecison(uPrecision)
{
}

CSynAoiDimScene::~CSynAoiDimScene()
{
}

void CSynAoiDimScene::Release()
{
	delete this;
}

bool CSynAoiDimScene::IsInSyncAoi(const CFPos& OffsetGridPos) const
{
	return m_pScene->IsInSyncAoi(OffsetGridPos);
}

void CSynAoiDimScene::SetPrecision(uint32 uPrecision)
{
	m_uPrecison = uPrecision;
}

uint32 CSynAoiDimScene::GetPrecision() const
{
	return m_uPrecison;
}

