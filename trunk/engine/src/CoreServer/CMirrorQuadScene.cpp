#include "stdafx.h"
#include "CMirrorQuadScene.h"
#include "CCoreQuadScene.h"

CMirrorQuadScene::CMirrorQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision)
{
	m_pScene = new CCoreQuadScene(uWidth, uHeight, uPrecision);
}

CMirrorQuadScene::~CMirrorQuadScene()
{
	
}
