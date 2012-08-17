//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Face.h"
#include "../TileEditorEngine/Console.h"
#include "../TileEditorEngine/CTileEditorModel.h"


#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CToolSetTileHeight::CToolSetTileHeight()
{
}

CToolSetTileHeight::~CToolSetTileHeight()
{
}	

void CToolSetTileHeight::OnEvent(ip::CInputDevice::SEvent & event)
{	
	COperator * pOperator = CXFile::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	switch ( event.eType )
	{
	case ip::EIET_KB_KEYUP:
		{
			// shrink brush
			if ( event.dwData == DIK_MINUS )
			{
				if ( pOperator->GetBrushSize() > 1 ) pOperator->DecBrushSize();
			}
			
			// amplify brush
			if ( event.dwData == DIK_EQUALS )
			{
				pOperator->IncBrushSize();
			}
			
			//switch(event.dwData)
			//{
			//case DIK_3:
			//	{
			//		pOperator->SetHeightType((COperator::ESET_HEIGHT_TYPE)((pOperator->GetHeightType() + 1) % pOperator->GetHeightTypeCount()));
			//	}
			//break;

			//case DIK_4:
			//	{
			//		pOperator->SetAxisType((COperator::EAXIS_TYPE)((pOperator->GetAxisType() + 1) % pOperator->GetAxisTypeCount()));
			//	}
			//break;

			//case DIK_5:
			//	{
			//		pOperator->SetWayType((COperator::ESET_WAY_TYPE)((pOperator->GetWayType() + 1) % pOperator->GetWayTypeCount()));
			//	}
			//break;
			//}
		}
	break;
	
	case ip::EIET_MS_BTNUP :
	case ip::EIET_MS_MOVE :	
		{
			if ( ip::CInputDevice::GetInstance()->GetButtonState(MK_LBUTTON) )
			{
				DWORD dwTextureHandleHot = pOperator->GetTextureHandleHot();
				DWORD dwLayerHot = pOperator->GetLayerHot();
	
				CCmdSetVertexHeight * pCmd = new CCmdSetVertexHeight;
				
				// set grids via brush size
				if ( pOperator->GetGridIndexHot() != INVALID )
				{
					DWORD dwCnt = 4*pOperator->GetBrushSize()*pOperator->GetBrushSize();
					
					DWORD dwGridIndex = pOperator->GetGridIndexHot() - pOperator->GetBrushSize() - pOperator->GetBrushSize() * pTerrainMesh->GetWidth();
	
					for ( DWORD j = 0; j < dwCnt; j++)
					{
						if ( j )
						{
							if ( !(j % (2 * pOperator->GetBrushSize())) ) 
								dwGridIndex += pTerrainMesh->GetWidth() - 2*pOperator->GetBrushSize() + 1;
							else
								++dwGridIndex;
						}

						// check for valid grid
						if ( dwGridIndex < 0 || dwGridIndex > pTerrainMesh->GetDepth() * pTerrainMesh->GetWidth() )
						{
							continue;
						}

						CTerrainMesh::SGrid & grid = pTerrainMesh->GetGrid(dwGridIndex);

						for ( int i = 0; i < 4; i++)
						{
							pCmd->AddVertex(grid.dwVertexIndex[i]);
						}
					}
				}

				CCmdMgr::GetInstance()->Do(pCmd);
			}
		}
		break;
	}
}

void CToolSetTileHeight::Render()
{

}

}
}