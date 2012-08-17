#include "stdafx.h"
#include "MapCommand/CmdDeleteObject.h"
#include "Brush/TerrainModelBrush.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CEditLogicDataMgr.h"


CCmdDeleteObject::CCmdDeleteObject( CEditContext* pContext )  : m_pEditContext(pContext), m_pModelBrush(NULL)
{

}

void CCmdDeleteObject::_Execute()
{
	string modelname("");
	size_t size = m_diff.vecEditModelFileNames.size();
	for ( size_t i = 0; i < size; ++i )
	{
		modelname = m_diff.vecEditModelFileNames[i];
		if( modelname.empty() )
			continue;

		CEditModel* pChildModel = m_pEditContext->CreateEditModel(NULL);
		pChildModel->AddPieceGroup(modelname.c_str());
		m_diff.pParentModel->AddChild(pChildModel, LT_CENTER, "");

		if( !m_diff.strAgpFileName.empty() )
			m_diff.pParentModel->AddAnimationGroup(m_diff.strAgpFileName.c_str());

		if( !m_diff.strEffectFileName.empty() && !m_diff.strEffectName.empty() )
			m_diff.pParentModel->AddEffect(m_diff.strEffectFileName.c_str(), m_diff.strEffectName.c_str(), CEffectPlayer::PLAY_LOOP, NULL);
	}

}

void CCmdDeleteObject::_UnExecute()
{
	
}

void CCmdDeleteObject::SetEditModelFileNames( const vector<string>& names )
{
	m_diff.vecEditModelFileNames = names;
}

void CCmdDeleteObject::SetEditModelAgpName( const string& agpname )
{
	m_diff.strAgpFileName = agpname;
}

void CCmdDeleteObject::SetEditModelEffectInfo( const string& effectFile, const string& effectname )
{
	m_diff.strEffectFileName = effectFile;
	m_diff.strEffectName	 = effectname;
}

void CCmdDeleteObject::SetBornParentModel( CEditModel* pParent )
{
	m_diff.pParentModel = pParent;
}

void CCmdDeleteObject::SetTerrainBrush( CTerrainModelBrush* pBrush )
{
	m_pModelBrush = pBrush;
}