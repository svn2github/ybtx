#include "stdafx.h"
#include "TerrainAdhereRenderer.h"
#include "CGraphic.h"
#include "Render.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "EditStateOpertiaonMgr.h"
#include "WireBox.h"

namespace sqr
{
	CTerrainAdhereRenderer::CTerrainAdhereRenderer() 
	{		 
		m_isRender = true;
		pWireBoxList.clear();
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->InsertRenderer(this);
	}


	CTerrainAdhereRenderer::~CTerrainAdhereRenderer()
	{
		size_t nWireBoxCnt = pWireBoxList.size();
		for ( size_t i = 0; i < nWireBoxCnt; ++i )
			pWireBoxList[i] = NULL;
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->DeleteRenderer(this);
	}

	void CTerrainAdhereRenderer::Render()
	{
		if(!m_isRender)
			return;

		size_t nWireBoxCnt = pWireBoxList.size();
		for ( size_t i = 0; i < nWireBoxCnt; ++i )
			pWireBoxList[i]->Render(CGraphic::GetInst(), true);

		CGraphic::GetInst()->DrawImmediate();

		//»­Ë¢×Ó
		CEditStateOpertiaonMgr::GetInst()->DrawStateBrush();
		//CGraphic::GetInst()->DrawImmediate();
// 		CMapEditApp::GetInst()->GetEditingMesh()->RenderLogicHeight();
// 		CGraphic::GetInst()->DrawImmediate();

		pWireBoxList.clear();
	}

	void CTerrainAdhereRenderer::AddRenderWireBox( CWireBox* pWireBox )
	{
		if( pWireBox == NULL )
			return;

		pWireBoxList.push_back(pWireBox);
	}
}