#include "stdafx.h"
#include "MapCommand/CmdPlaceObject.h"
#include "Brush/TerrainModelBrush.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CEditLogicDataMgr.h"


CCmdPlaceObject::CCmdPlaceObject( CEditContext* pContext )  : m_pEditContext(pContext), m_pModelBrush(NULL)
{
}

void CCmdPlaceObject::_Execute()
{
	m_diff.pParentModel->SetPosition(m_diff.vPosition);
	m_pEditContext->AddToTreeScene(m_diff.pParentModel);
	m_pModelBrush->SetActiveModelState(EES_PLACED);
	m_pModelBrush->SetActiveModel(NULL);
	m_pModelBrush->SetLockModel(m_diff.pParentModel);
}

void CCmdPlaceObject::_UnExecute()
{
	m_pEditContext->AddToRenderScene(m_diff.pParentModel);
	m_pModelBrush->SetActiveModelState(EES_MOVING);
	m_pModelBrush->SetActiveModel(m_diff.pParentModel);
}

void CCmdPlaceObject::SetTerrainBrush( CTerrainModelBrush* pBrush )
{
	m_pModelBrush = pBrush;
}

void CCmdPlaceObject::SetActiveModel( CEditModel* pModel )
{
	m_diff.pParentModel = pModel;
}

void CCmdPlaceObject::SetActiveModelPosition( const CVector3f& pos )
{
	m_diff.vPosition = pos;
}