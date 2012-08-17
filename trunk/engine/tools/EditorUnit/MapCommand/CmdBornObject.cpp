#include "stdafx.h"
#include "MapCommand/CmdBornObject.h"
#include "Brush/TerrainModelBrush.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CEditLogicDataMgr.h"


CCmdBornObject::CCmdBornObject( CEditContext* pContext )  : m_pEditContext(pContext), m_pModelBrush(NULL)
{

}

void CCmdBornObject::_Execute()
{
	string modelname("");
	size_t size = m_diff.vecEditModelFileNames.size();
	if( size == 1 )
	{
		modelname = m_diff.vecEditModelFileNames[0];
		m_diff.pParentModel = m_pEditContext->CreateEditModel(NULL);
		m_diff.pParentModel->AddPieceGroup(modelname.c_str());
	}
	else
	{
		for ( size_t i = 0; i < size; ++i )
		{
			modelname = m_diff.vecEditModelFileNames[i];
			if( modelname.empty() )
				continue;

			CEditModel* pChildModel = m_pEditContext->CreateEditModel(NULL);
			pChildModel->AddPieceGroup(modelname.c_str());

			m_diff.pParentModel->AddChild(pChildModel, LT_CENTER, "");
		}

	}

	m_diff.pParentModel->SetScale(m_diff.fScale);
	m_diff.pParentModel->RotateX(m_diff.vRotate.x);
	m_diff.pParentModel->RotateY(m_diff.vRotate.y);
	m_diff.pParentModel->RotateZ(m_diff.vRotate.z);
	m_diff.pParentModel->SetIsVisibleByConfigure(m_diff.bVisibleByConfigure);

	if( !m_diff.strAgpFileName.empty() )
		m_diff.pParentModel->AddAnimationGroup(m_diff.strAgpFileName.c_str());

	if( !m_diff.strEffectFileName.empty() && !m_diff.strEffectName.empty() )
		m_diff.pParentModel->AddEffect(m_diff.strEffectFileName.c_str(), m_diff.strEffectName.c_str(), CEffectPlayer::PLAY_LOOP, NULL);

	m_pEditContext->AddToRenderScene(m_diff.pParentModel);
	m_pModelBrush->SetActiveModelState(EES_MOVING);
	m_pModelBrush->SetActiveModel(m_diff.pParentModel);
}

void CCmdBornObject::_UnExecute()
{
	m_pEditContext->DestroyRenderObject(m_diff.pParentModel);
	m_pModelBrush->ClearActiveObject();
	m_pModelBrush->SetActiveModelState(EES_NONE);
}

void CCmdBornObject::SetEditModelFileNames( const vector<string>& names )
{
	m_diff.vecEditModelFileNames = names;
}

void CCmdBornObject::SetEditModelAgpName( const string& agpname )
{
	m_diff.strAgpFileName = agpname;
}

void CCmdBornObject::SetEditModelEffectInfo( const string& effectFile, const string& effectname )
{
	m_diff.strEffectFileName = effectFile;
	m_diff.strEffectName	 = effectname;
}

void CCmdBornObject::SetTerrainBrush( CTerrainModelBrush* pBrush )
{
	m_pModelBrush = pBrush;
}