#include "stdafx.h"
#include "Cmd.h"
#include "CMapEditApp.h"
#include "CmdDeleteBlock.h"
#include "Picker.h"
#include "Face.h"


namespace bly
{
void CCmdDeleteBlock::Execute()
{
	CCmd::Execute();

	CTerrainMesh * pTerrainMesh = CMapEditApp::GetInstance()->GetTerrainMesh();

	COperator * pOperator = CMapEditApp::GetInstance()->GetOperator();

	for ( size_t i = 0; i < m_diff.vecBlockGridIndices.size(); ++i )
	{
		int gridIndex = m_diff.vecBlockGridIndices[i];

		CTerrainMesh::SGrid & gridBlock = pTerrainMesh->GetGrid( gridIndex );

		m_diff.vecSize.push_back(gridBlock.vecBlockTypes.size());
		m_diff.vecBlockType.push_back(gridBlock.nBlockType);

		// erase block type info
		std::vector< std::pair<int,CTerrainMesh::CObject*> >::iterator iterBlockType = gridBlock.vecBlockTypes.begin();

		CMapEditModel * pModel = NULL;
		pModel = iterBlockType->second->GetFace()->GetSelfPresentation(0);

		if ( pModel )
		{
			m_diff.pObject.push_back(iterBlockType->second);

			pModel->DeleteBlockGridIndex( gridIndex );

			pOperator->UpdatePixel(gridBlock.dwMiniBlockIndex,gridBlock.dwMGridIndexInMiniBlock,CMiniMap::COLOR_ERASE_BLOCK);

			gridBlock.vecBlockTypes.clear();
		}

	}
}

void CCmdDeleteBlock::UnExecute()
{
	CCmd::UnExecute();

	CTerrainMesh * pTerrainMesh = CMapEditApp::GetInstance()->GetTerrainMesh();

	COperator * pOperator = CMapEditApp::GetInstance()->GetOperator();

	for ( size_t i = 0; i < m_diff.vecBlockGridIndices.size(); ++i )
	{
		int gridIndex = m_diff.vecBlockGridIndices[i];

		CTerrainMesh::SGrid & gridBlock = pTerrainMesh->GetGrid( gridIndex );

		CMapEditModel * pModel = NULL;
		pModel = m_diff.pObject[i]->GetFace()->GetSelfPresentation(0);

		if ( pModel )
		{
			pModel->AddBlockGridIndex( gridIndex );

			for ( int j = 0; j < m_diff.vecSize[i]; ++j )
				gridBlock.vecBlockTypes.push_back(make_pair(m_diff.vecBlockType[i], m_diff.pObject[i].ptr()));

			gridBlock.nBlockType = m_diff.vecBlockType[i];

			pOperator->UpdatePixel(gridBlock.dwMiniBlockIndex,gridBlock.dwMGridIndexInMiniBlock,CMiniMap::COLOR_ADD_BLOCK);
		}
	}
}

}