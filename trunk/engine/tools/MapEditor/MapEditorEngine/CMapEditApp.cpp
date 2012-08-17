#include "stdafx.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "CEditDataScene.h"
#include "CEditContext.h"
#include "CCameraManager.h"
#include "CMapEditCamera.h"
#include "ToolMainPlayer.h"
#include "CDebugText.h"
#include "CMapContext.h"
#include "WaterOperator.h"

//ÖØ¹¹
#include "CMapMgr.h"

bool CMapEditApp::NEWOLD = false;

CMapEditApp::CMapEditApp()
: m_pOperator(NULL)
, m_pTerrainMesh(NULL)
, m_pRenderScene(NULL)
, m_pTransformTerrainMesh(NULL)
, savelog("")
, m_bSave(true)
{
	m_pOperator			= new COperator;
	m_pWaterOperator	= new CWaterOperator;
	m_Map				= new CMapMgr();
	
	m_eEditMesh = EEM_TERRAIN;
}

CMapEditApp::~CMapEditApp()
{
	SafeDelete(m_pOperator);
	SafeDelete(m_pWaterOperator);
}
void CMapEditApp::SetRenderScene(CEditContext* cont)
{
	m_pRenderScene=cont;
}

void CMapEditApp::SetTransformTerrainMesh(CTerrainMesh * pTerrainMesh)
{
	m_pTransformTerrainMesh = pTerrainMesh;
}

void CMapEditApp::SetTerrainMesh(CTerrainMesh * pTerrainMesh)
{
	m_pTerrainMesh = pTerrainMesh;
}

CTerrainMesh * CMapEditApp::GetEditingMesh()
{
	switch ( m_eEditMesh )
	{
	case sqr::EEM_TERRAIN:
		return 	m_pTerrainMesh;
		break;

	case sqr::EEM_WATER:
		return 	m_pTerrainMesh->GetWater();
		break;

	default:
		break;
	}

	return NULL;
}

CTerrainMesh * CMapEditApp::GetTransformEditingMesh()
{
	return m_pTransformTerrainMesh;
}

COperator * CMapEditApp::GetOperator()
{
	return m_pOperator;
}

CWaterOperator* CMapEditApp::GetWaterOperator()
{
	return m_pWaterOperator;
}

CTerrainMesh * CMapEditApp::GetWater()
{
	if ( m_pTerrainMesh )
		return 	m_pTerrainMesh->GetWater();
	else
		return NULL;
}

CTerrainMesh * CMapEditApp::GetTerrain()
{
	return 	m_pTerrainMesh;
}

CMapMgr* CMapEditApp::GetMapMgr()
{
	return m_Map;
}

bool CMapEditApp::InitNewRenderScene()
{
	if(m_pRenderScene !=NULL)
		return false;
	return m_pRenderScene != NULL;
}

CEditDataScene* CMapEditApp::GetDataScene( void )
{
	return m_pRenderScene->GetEditDataScene();
}

CEditContext* CMapEditApp::GetRenderScene( void )
{
	return m_pRenderScene;
}