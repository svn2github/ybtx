#include "stdafx.h"
#include "Picker.h"
#include "Engine.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "TerrainMeshGrid.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "EditStateOpertiaonMgr.h"
#include "CEditContext.h"
#include "CMapEditObject.h"
#include "CEventState.h"

namespace sqr
{

	CPicker::CPicker()
	{ 
		m_pLastSelected  = NULL;
		m_pMapEditObject = NULL;
	}

	CPicker::~CPicker() 
	{ 
	}

	int CPicker::GetPickPos(CTObject ** pp, CVector3f * pvHitPoint, vector< CTObject * > * pVecObjectOlds)
	{	
		m_vecGridIndices.clear();
		
		const DWORD SEARCH_TOLERANCE = 100;
		
		//LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
		
		POINT pt;
		MSG_MOUSE_INF inf = CEventState::GetInst()->GetMousePosition();
		pt.x = inf.X;
		pt.y = inf.Y;

		CVector3f vec;
		CMatrix matProj, matViewInvert;

		CCamera *pCamera = CMainWindowTarget::GetInst()->GetCamera();
		matProj = pCamera->getProjectionMatrix();
		pCamera->GetCameraInvert( matViewInvert );

		// 将鼠标选中的点转换到世界空间
		RECT rc;
		HWND hWnd;
		hWnd = CEngine::GetInst()->GetD3DWindow();
		GetClientRect(hWnd,&rc);

		DWORD		dwWndWidth, dwWndHeight;
		dwWndWidth	= rc.right;
		dwWndHeight	= rc.bottom;

		vec.z = 1.0f;
		vec.x = (2.0f*static_cast<float>(pt.x)/static_cast<float>(dwWndWidth) - 1.0f) / matProj._11;
		vec.y = (1.0f-2.0f*static_cast<float>(pt.y)/static_cast<float>(dwWndHeight)) / matProj._22;
		vec *= matViewInvert;

		//////////////////////////////////////////////////////////////////////////

		CVector3f vStart = CMainWindowTarget::GetInst()->GetCamera()->GetPosition();

		CVector3f vEnd =  vec;

		float tg = (vEnd.z - vStart.z)/(vEnd.x - vStart.x);

		CVector3f vNew(vStart);

		vNew.x = vNew.x/GRID_SPACE;
		vNew.z = vNew.z/GRID_SPACE;

		int dwGrid = 0;

		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		
		if( pTerrain == NULL )
			return INVALID;

		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if( pOperator == NULL )
			return INVALID;

		int TerrainWidth = CMapEditApp::GetInst()->GetEditingMesh()->GetWidth();

		if ( pTerrain->IsValid(vNew) )
		{
			dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
			m_vecGridIndices.insert(dwGrid);
		}

		if ( vStart.z < vEnd.z )
		{
			if ( 0.0f < tg )
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg <= 1.0f )
					{
						vNew.x += 1.0f;
						vNew.z += tg;
					}
					else
					{
						vNew.z += 1.0f;
						vNew.x += 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}
						
						// right
						dwGrid += 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}
						
						// upright
						dwGrid += TerrainWidth;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}


						// up
						dwGrid -= 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

					}
				}
			}
			else // tg < 0.0f
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg >= -1.0f )
					{
						vNew.x -= 1.0f;
						vNew.z += -tg;

					}
					else  // < -1.0f
					{
						vNew.z += 1.0f;
						vNew.x -= -1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = int(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// left
						dwGrid -= 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// upleft
						dwGrid += TerrainWidth;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// up
						dwGrid += 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}
					}
				}
			}
		}
		else  // inverse direction
		{
			if ( 0.0f > tg )
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg >= -1.0f )
					{
						vNew.x += 1.0f;
						vNew.z += tg;
					}
					else
					{
						vNew.z -= 1.0f;
						vNew.x -= 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = int(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// left
						dwGrid += 1;
						
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// upleft
						dwGrid -= TerrainWidth;
						
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// up
						dwGrid -= 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}
					}
				}
			}
			else // tg > 0.0f
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg < 1.0f )
					{
						vNew.x -= 1.0f;
						vNew.z -= tg;
					}
					else  // > 1.0f
					{
						vNew.z -= 1.0f;
						vNew.x -= 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// right
						dwGrid -= 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// upright
						dwGrid -= TerrainWidth;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}

						// up
						dwGrid += 1;
						if ( pTerrain->IsValid(dwGrid) )
						{
							m_vecGridIndices.insert(dwGrid);
						}
					}
				}
			}
		}

		CVector3f vDir = vEnd - vStart;
		
		DWORD dwGridIndexHot = INVALID;
		
		if ( pp ) 
		{
			*pp = NULL;
		}

		int gridindex = 0;
		for ( set<int>::iterator iter = m_vecGridIndices.begin(); iter != m_vecGridIndices.end(); ++iter )
		{	
			gridindex = *iter;

			SGrid &grid = pTerrain->GetGrid(gridindex);

			//////////////////////////////////////////////////////////////////////////

			if ( pp && !grid.vecCenterObjects.empty() && *pp == NULL )
			{
				for ( vector< CTObjectPtr>::iterator iter = grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr& object = (*iter);
					CTObject* p = object.get();
					
					if ( pVecObjectOlds != NULL && find(pVecObjectOlds->begin(),pVecObjectOlds->end(),p) != pVecObjectOlds->end() )
						continue;

					CEditModelGroup *pModelGroup =  p->GetModelGroup();
					if ( pModelGroup->Intersected(vDir,vStart) )
					{
						//pModelGroup->SetSelected(true);
						*pp = p;
						
						if( pModelGroup->m_SL.m_bIAmbScenePointLight)
						{
							pOperator->RefreshEffectViewSceneLightSliderState(
								pModelGroup->m_SL.m_Rang,
								pModelGroup->m_SL.m_Attenuation0,
								pModelGroup->m_SL.m_Attenuation1,
								pModelGroup->m_SL.m_Attenuation2,
								pModelGroup->m_SL.m_Dyn_BrightTime ,
								pModelGroup->m_SL.m_Dyn_DarkTime,
								pModelGroup->m_SL.m_Dyn_ChangeTime,
								pModelGroup->m_SL.m_Dyn_ChangeProbability,
								pModelGroup->m_SL.m_bDynamic );
						}
						else if( pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
						{
							int nTheta = (int)(pModelGroup->m_SpotLight.m_fTheta * 180.0f / CMath::pi + 0.5f);
							int nPhi = (int)(pModelGroup->m_SpotLight.m_fPhi * 180.0f / CMath::pi + 0.5f);

							pOperator->RefreshEffectViewSpotLightSliderState(
								pModelGroup->m_SpotLight.m_fRang,
								pModelGroup->m_SpotLight.m_fAttenuation0,
								pModelGroup->m_SpotLight.m_fAttenuation1,
								pModelGroup->m_SpotLight.m_fAttenuation2,
								pModelGroup->m_SpotLight.m_fFallOff,
								float(nTheta), float(nPhi));
						}

						if ( m_pLastSelected && m_pLastSelected.get() != p )
						{
							m_pLastSelected->GetModelGroup()->SetSelected(false);
						}

						m_pLastSelected = p;

						break;
					}
				}
			}
			
			SVertex & vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
			SVertex & vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
			SVertex & vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
			SVertex & vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[3]);


			CVector3f v0,v1,v2,v3;

			CEditStateOpertiaonMgr::EEDIT_STATE eState = CEditStateOpertiaonMgr::GetInst()->GetEditState();
			if ( eState == CEditStateOpertiaonMgr::EES_RUN || eState == CEditStateOpertiaonMgr::EES_SET_TILE_BLOCK )
			{
				v0 = vertex0.vPosition + CVector3f(0.0f,vertex0.fLogicHeight,0.0f);
				v1 = vertex1.vPosition + CVector3f(0.0f,vertex1.fLogicHeight,0.0f);
				v2 = vertex2.vPosition + CVector3f(0.0f,vertex2.fLogicHeight,0.0f);
				v3 = vertex3.vPosition + CVector3f(0.0f,vertex3.fLogicHeight,0.0f);
			}
			else
			{
				v0 = vertex0.vPosition;
				v1 = vertex1.vPosition;
				v2 = vertex2.vPosition;
				v3 = vertex3.vPosition;
			}
					
			float u,v,d;
			
			if ( CMath::IntersectTri( v0, v1, v3, vStart, vDir ,u,v,d ) )
			{
				if ( pvHitPoint )
				{
					*pvHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
				}
				
				dwGridIndexHot = gridindex;

				if ( pp && m_pLastSelected && (*pp) == NULL )
				{
					m_pLastSelected->GetModelGroup()->SetSelected(false);
				}
				return dwGridIndexHot;
			}
			else if ( CMath::IntersectTri( v1, v2, v3, vStart, vDir ,u,v,d ) )
			{
				if ( pvHitPoint )
				{
					*pvHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
				}

				dwGridIndexHot = gridindex;
				
				if ( pp && m_pLastSelected && (*pp) == NULL )
				{
					m_pLastSelected->GetModelGroup()->SetSelected(false);
				}
				return dwGridIndexHot;
			}
		}	
		
		return dwGridIndexHot;
	}

	float CPicker::GetHeight(float fX, float fZ)
	{
		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return 0.0f;

		int nGridIndexInX = int(fX/GRID_SPACE);
		int nGridIndexInZ = int(fZ/GRID_SPACE);
		int nGridIndex = nGridIndexInZ * pTerrain->GetWidth() + nGridIndexInX;
		
		 SGrid & grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nGridIndex);
		
		SVertex & vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
		SVertex & vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
		SVertex & vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
		SVertex & vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[3]);
		
		CVector3f v0 = vertex0.vPosition + CVector3f(0.0f,vertex0.fLogicHeight,0.0f);
		CVector3f v1 = vertex1.vPosition + CVector3f(0.0f,vertex1.fLogicHeight,0.0f);
		CVector3f v2 = vertex2.vPosition + CVector3f(0.0f,vertex2.fLogicHeight,0.0f);
		CVector3f v3 = vertex3.vPosition + CVector3f(0.0f,vertex3.fLogicHeight,0.0f);
			
		float y;
		
		if ( v1.x - fX > fZ - v1.z )
		{
			y = v0.y + (v1.y - v0.y)*(fX-v0.x)/GRID_SPACE;
			y = y + (v3.y - y)*(fZ - v0.z)/GRID_SPACE; 
		}
		else
		{
			y = v3.y + (v2.y - v3.y)*(fX-v0.x)/GRID_SPACE;
			y = y + (v3.y - y)*(fZ - v0.z)/GRID_SPACE; 
		}
			
		return y;
	}

	bool CPicker::GetIntersected(CVector3f & vHitPoint, int * pnGridIndexIntersected)
	{
		const DWORD SEARCH_TOLERANCE = 100;
		
		/*CVector3f vStart = CCameraManager::GetInst()->GetMapEditCamera()->GetPosition();
		CVector3f vEnd   = CCameraManager::GetInst()->GetMapEditCamera()->GetTarget();*/
		CVector3f vStart = CMainWindowTarget::GetInst()->GetCamera()->GetPosition();
		CVector3f vEnd	 = vStart + CMainWindowTarget::GetInst()->GetCamera()->getDIRECTION();

		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return 0;

		int TerrainWidth = CMapEditApp::GetInst()->GetEditingMesh()->GetWidth();
		
		float fx = vEnd.x - vStart.x; 
		
		if ( fabs(fx) < FLT_MIN  )
		{
			fx += 0.0001f;
		}
		
		float tg = (vEnd.z - vStart.z)/fx;
		
		CVector3f vNew(vStart);
		
		vNew.x = vNew.x/GRID_SPACE;
		vNew.z = vNew.z/GRID_SPACE;

		int dwGrid = 0;

		vector<int> vecGridIndices;

		if ( pTerrain->IsValid(vNew) )
		{
			dwGrid = int(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
			vecGridIndices.push_back(dwGrid);
		}

		if ( vStart.z < vEnd.z )
		{
			if ( 0.0f < tg )
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg <= 1.0f )
					{
						vNew.x += 1.0f;
						vNew.z += tg;
					}
					else
					{
						vNew.z += 1.0f;
						vNew.x += 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);

						// right
						dwGrid += 1;
						vecGridIndices.push_back(dwGrid);

						// upright
						dwGrid += TerrainWidth;
						vecGridIndices.push_back(dwGrid);

						// up
						dwGrid -= 1;
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
			else // tg < 0.0f
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg >= -1.0f )
					{
						vNew.x -= 1.0f;
						vNew.z += -tg;

					}
					else  // < -1.0f
					{
						vNew.z += 1.0f;
						vNew.x -= -1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);

						// left
						dwGrid -= 1;
						vecGridIndices.push_back(dwGrid);

						// upleft
						dwGrid += TerrainWidth;
						vecGridIndices.push_back(dwGrid);

						// up
						dwGrid += 1;
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
		else  // inverse direction
		{
			if ( 0.0f > tg )
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg >= -1.0f )
					{
						vNew.x += 1.0f;
						vNew.z += tg;
					}
					else
					{
						vNew.z -= 1.0f;
						vNew.x -= 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);

						// left
						dwGrid += 1;
						vecGridIndices.push_back(dwGrid);

						// upleft
						dwGrid -= TerrainWidth;
						vecGridIndices.push_back(dwGrid);

						// up
						dwGrid -= 1;
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
			else // tg > 0.0f
			{
				for(size_t i = 0; i < SEARCH_TOLERANCE; i++)
				{
					if ( tg < 1.0f )
					{
						vNew.x -= 1.0f;
						vNew.z -= tg;
					}
					else  // > 1.0f
					{
						vNew.z -= 1.0f;
						vNew.x -= 1/tg;
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);

						// right
						dwGrid -= 1;
						vecGridIndices.push_back(dwGrid);

						// upright
						dwGrid -= TerrainWidth;
						vecGridIndices.push_back(dwGrid);

						// up
						dwGrid += 1;
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
		
		CVector3f vDir = vEnd - vStart;

		for ( vector<int>::iterator iter = vecGridIndices.begin(); iter != vecGridIndices.end(); iter++ )
		{	
			if ( pTerrain->IsValid(*iter) )
			{
				 SGrid & grid = pTerrain->GetGrid(*iter);

				//////////////////////////////////////////////////////////////////////////

				SVertex & vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
				SVertex & vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
				SVertex & vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
				SVertex & vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[3]);

				CVector3f v0 = vertex0.vPosition + CVector3f(0.0f,vertex0.fLogicHeight,0.0f);
				CVector3f v1 = vertex1.vPosition + CVector3f(0.0f,vertex1.fLogicHeight,0.0f);
				CVector3f v2 = vertex2.vPosition + CVector3f(0.0f,vertex2.fLogicHeight,0.0f);
				CVector3f v3 = vertex3.vPosition + CVector3f(0.0f,vertex3.fLogicHeight,0.0f);

				float u,v,d;

				if ( CMath::IntersectTri(v0,v1,v3,vStart,vDir, u,v,d) )
				{
					vHitPoint = v0 + u*(v1 - v0) + v*(v3 - v0);
					if ( pnGridIndexIntersected ) *pnGridIndexIntersected = *iter;
					return true;
				}
				else if ( CMath::IntersectTri(v1,v2,v3,vStart,vDir, u,v,d) )
				{
					vHitPoint = v1 + u*(v2 - v1) + v*(v3 - v1);
					if ( pnGridIndexIntersected ) *pnGridIndexIntersected = *iter;
					return true;
				}
			}
		}	
		
		return false;
	}

	void CPicker::Evaluate(CVector3f vStart, CVector3f vEnd, DWORD dwSearchCnt, vector<DWORD> & vecGridIndices)
	{	
		CTerrainMesh *pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return ;

		int TerrainWidth = CMapEditApp::GetInst()->GetEditingMesh()->GetWidth();

		float fx = vEnd.x - vStart.x; 
		
		if ( fabs(fx) < FLT_MIN  )
		{
			fx += 0.0001f;
		}
		
		float tg = (vEnd.z - vStart.z)/fx;
		
		CVector3f vNew(vStart);
		CVector3f vNewAdded;
		
		vNew.x = vNew.x/GRID_SPACE;
		vNew.z = vNew.z/GRID_SPACE;
		
		int dwGrid = 0;
		
		if ( pTerrain->IsValid(vNew) )
		{
			dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
			vecGridIndices.push_back(dwGrid);
		}

		dwSearchCnt--;

		//////////////////////////////////////////////////////////////////////////
		
		if ( fabs(vEnd.z - vStart.z) < FLT_MIN )
		{
			if ( vStart.x < vEnd.x )
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					vecGridIndices.push_back(++dwGrid);
				}
			}
			else
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					vecGridIndices.push_back(--dwGrid);
				}
			}

			return;
		}

		//////////////////////////////////////////////////////////////////////////

		if ( fabs(vEnd.x - vStart.x) < FLT_MIN )
		{
			if ( vStart.z < vEnd.z )
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					dwGrid += TerrainWidth;
					vecGridIndices.push_back(dwGrid);
				}
			}
			else
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					dwGrid -= TerrainWidth;
					vecGridIndices.push_back(dwGrid);
				}
			}

			return;
		}

		//////////////////////////////////////////////////////////////////////////
		
		bool fNewAdded = false;
		
		if ( vStart.z < vEnd.z )
		{
			if ( tg > 0.0f )
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					if ( tg <= 1.0f )
					{
						vNewAdded = vNew;

						vNew.x += 1.0f;
						vNew.z += tg;

						if ( int(vNew.z) > int(vNewAdded.z) )
						{
							vNewAdded = vNew + CVector3f(0.0f,0.0f,-1.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}
					else
					{
						vNewAdded = vNew;

						vNew.z += 1.0f;
						vNew.x += 1/tg;

						if ( int(vNew.x) > int(vNewAdded.x) )
						{
							vNewAdded = vNew + CVector3f(-1.0f,0.0f,0.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}

					if ( CMapEditApp::GetInst()->GetEditingMesh()->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}
					
					if ( fNewAdded && pTerrain->IsValid(vNewAdded) )
					{
						dwGrid = (int)(floor(vNewAdded.x) + floor(vNewAdded.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
			else // tg <= 0.0f
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					if ( tg >= -1.0f )
					{
						vNewAdded = vNew;

						vNew.x -= 1.0f;
						vNew.z += -tg;

						if ( int(vNew.z) > int(vNewAdded.z) )
						{
							vNewAdded = vNew + CVector3f(0.0f,0.0f,-1.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}
					else  // < -1.0f
					{
						vNewAdded = vNew;

						vNew.z += 1.0f;
						vNew.x -= -1/tg;

						if ( int(vNew.x) < int(vNewAdded.x) )
						{
							vNewAdded = vNew + CVector3f(1.0f,0.0f,0.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}

					if ( fNewAdded && pTerrain->IsValid(vNewAdded) )
					{
						dwGrid = (int)(floor(vNewAdded.x) + floor(vNewAdded.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
		else  // inverse direction
		{
			if ( 0.0f > tg )
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					if ( tg >= -1.0f )
					{
						vNewAdded = vNew;

						vNew.x += 1.0f;
						vNew.z += tg;

						if ( int(vNew.z) < int(vNewAdded.z) )
						{
							vNewAdded = vNew + CVector3f(0.0f,0.0f,1.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}
					else
					{
						vNewAdded = vNew;

						vNew.z -= 1.0f;
						vNew.x -= 1/tg;

						if ( int(vNew.x) > int(vNewAdded.x) )
						{
							vNewAdded = vNew + CVector3f(-1.0f,0.0f,0.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}

					if ( fNewAdded && pTerrain->IsValid(vNewAdded) )
					{
						dwGrid = (int)(floor(vNewAdded.x) + floor(vNewAdded.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
			else // tg >= 0.0f
			{
				for(size_t i = 0; i < dwSearchCnt; i++)
				{
					if ( tg < 1.0f )
					{
						vNewAdded = vNew;

						vNew.x -= 1.0f;
						vNew.z -= tg;

						if ( int(vNew.z) < int(vNewAdded.z) )
						{
							vNewAdded = vNew + CVector3f(0.0f,0.0f,1.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}
					else  // > 1.0f
					{
						vNewAdded = vNew;

						vNew.z -= 1.0f;
						vNew.x -= 1/tg;

						if ( int(vNew.x) < int(vNewAdded.x) )
						{
							vNewAdded = vNew + CVector3f(1.0f,0.0f,0.0f);
							fNewAdded = true;
						}
						else
						{
							fNewAdded = false;
						}								
					}

					if ( pTerrain->IsValid(vNew) )
					{
						dwGrid = (int)(floor(vNew.x) + floor(vNew.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}

					if ( fNewAdded && pTerrain->IsValid(vNewAdded) )
					{
						dwGrid = (int)(floor(vNewAdded.x) + floor(vNewAdded.z) * TerrainWidth);
						vecGridIndices.push_back(dwGrid);
					}
				}
			}
		}
	}

	void CPicker::PickSceneObject( const DWORD& gridindex )
	{
		return;

		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		DWORD dwWidthInTileRegion = pTerrainMesh->GetWidthInTileRegion();
		SGrid &grid = pTerrainMesh->GetGrid(gridindex);
		int nSubRgnIndex = grid.dwBufferIndex;
		int	j = nSubRgnIndex % dwWidthInTileRegion;
		int	i = nSubRgnIndex / dwWidthInTileRegion;
		URegionID id = URegionID(j,i);

		POINT pt;
		MSG_MOUSE_INF mouse_pos = CEventState::GetInst()->GetMousePosition();
		pt.x = mouse_pos.X;
		pt.y = mouse_pos.Y;

		CVector3f vec;
		CMatrix matProj, matViewInvert;

		CCamera *pCamera = CMainWindowTarget::GetInst()->GetCamera();
		matProj = pCamera->getProjectionMatrix();
		pCamera->GetCameraInvert( matViewInvert );

		// 将鼠标选中的点转换到世界空间
		RECT rc;
		HWND hWnd;
		hWnd = CEngine::GetInst()->GetD3DWindow();
		GetClientRect(hWnd,&rc);

		DWORD		dwWndWidth, dwWndHeight;
		dwWndWidth	= rc.right;
		dwWndHeight	= rc.bottom;

		float	x	= float(pt.x) / float(dwWndWidth);
		float	y	= float(pt.y) / float(dwWndHeight);
		CRenderNode* pNode = CMapEditApp::GetInst()->GetRenderScene()->SelectModelByCursor(x, y);
		CRenderObject *pSelectObject = NULL;
		if(pNode->GetNodeType() == CRenderObject::st_RenderObjectType)
			pSelectObject = (CRenderObject*)pNode;

		if( pSelectObject )
		{
			CMapEditObject *pMapEditObject = (CMapEditObject *)pSelectObject;
			pMapEditObject->SetSelected(true);
			string str = pMapEditObject->GetName();

			if( m_pMapEditObject )
				m_pMapEditObject->SetSelected(false);

			m_pMapEditObject = pMapEditObject;
		}
	}
}
