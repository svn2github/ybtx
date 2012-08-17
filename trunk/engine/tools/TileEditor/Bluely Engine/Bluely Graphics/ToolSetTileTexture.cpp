//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "ToolSetTileTexture.h"
#include "XFile.h"
#include "TerrainMesh.h"
#include "CmdMgr.h"
#include "CmdSetTexture.h"
#include "CmdSetVertexAlpha.h"
#include <GUI.h>
#include "d3dfont.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CToolSetTileTexture::CToolSetTileTexture()
{
}

CToolSetTileTexture::~CToolSetTileTexture()
{
}	

void CToolSetTileTexture::OnEvent(ip::CInputDevice::SEvent & event)
{
	COperator * pOperator = CXFile::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	switch ( event.eType )
	{
	case ip::EIET_KB_KEYUP :
		{
			if ( event.dwData == DIK_F )
			{
				if ( pOperator->GetLayerHot() == INVALID  ) 
				{
					// @
                  break;
				}

				DWORD dwGridCnt = pTerrainMesh->GetGridCount(); 
				CCmdSetTexture * pCmd = new CCmdSetTexture;
				for ( int i = 0; i < dwGridCnt; i++ )
				{
					pCmd->AddGrid(i);
				}
				CCmdMgr::GetInstance()->Do(pCmd);
				pTerrainMesh->RebuildRenderTree();
			}

			//// switch to layer one
			//if ( event.dwData == DIK_1 )
			//{
			//	ui::CCheckBox::SetHotID(0);
			//	::SendMessage(pOperator->GetResSetView(), WM_SETLAYERHOT, 1, 0);
			//	::SendMessage(pOperator->GetGroundView(), WM_SETLAYERHOT, 1, 0);
			//}

			//// switch to layer two
			//if ( event.dwData == DIK_2 )
			//{
			//	ui::CCheckBox::SetHotID(1);
			//	::SendMessage(pOperator->GetResSetView(), WM_SETLAYERHOT, 2, 0);
			//	::SendMessage(pOperator->GetGroundView(), WM_SETLAYERHOT, 2, 0);
			//}

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
			//		pOperator->SetAlphaType( (COperator::ESET_ALPHA_TYPE) ( (pOperator->GetAlphaType() + 1) % pOperator->GetAlphaTypeCount()) );
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

				CCmdSetTexture * pCmd = new CCmdSetTexture;
				CCmdSetVertexAlpha * pCmdSetVertexAlpha;

				if ( dwLayerHot == 1 )
				{
					pCmdSetVertexAlpha = new CCmdSetVertexAlpha;
				}

				if ( dwLayerHot == INVALID  )
				{
					// @
					break;
				}

				// set grids via brush size
				if ( pOperator->GetGridIndexHot() != INVALID && pOperator->GetGridIndexHot() < pTerrainMesh->GetDepth() * pTerrainMesh->GetWidth() )
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

						// add with no redundency
						if ( grid.nodeInfo.dwTextureHandles[dwLayerHot] != dwTextureHandleHot )
						{
							pCmd->AddGrid(dwGridIndex);
						}

						// add vertex for alpha manipulation
						if ( dwLayerHot == 1 )
						{
							for ( int i = 0; i < 4; i++)
							{
								pCmdSetVertexAlpha->AddVertex(grid.dwVertexIndex[i]);
							}
						}
					}
				}
				
				if ( pCmd->GetGridCnt() > 0 )
				{
					CCmdMgr::GetInstance()->Do(pCmd);
					pTerrainMesh->RebuildRenderTree();
				}
				else
				{
					SAFE_DELETE(pCmd);
				}

				if ( dwLayerHot == 1 )
				{
					CCmdMgr::GetInstance()->Do(pCmdSetVertexAlpha);
				}
			}
		}
		break;
	}
}

void CToolSetTileTexture::Render()
{

}

}
}