#include "StdAfx.h"
#include "CEditBlockDebugNode.h"
#include "CRenderScene.h"
#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"

CEditBlockDebugNode::CEditBlockDebugNode(ERenderObjStyle style , CRenderScene* pScene)
	: CSceneBlockDebugNode(style,pScene)
{
	m_uRange = 128;//这里现在有个bug，由于m_pRenderScene->GetCameraDest()在地编的值始终是一样的，所以阻挡显示不跟随camera移动。
}

CEditBlockDebugNode::~CEditBlockDebugNode(void)
{
}

EBarrierType CEditBlockDebugNode::GetBarrierType( CPos pos )
{
	CEditLogicDataMgr* pEditLogicDataMgr = (CEditLogicDataMgr*)m_pRenderScene->GetDataScene()->GetLogicDataMgr();
	int8 type = pEditLogicDataMgr->GetBlockType(pos.x, pos.y);
	return (EBarrierType)type;
}