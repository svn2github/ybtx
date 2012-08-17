#include "stdafx.h"
#include "../ChunkData/CGameLightCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "CreateSceneObjectMgr.h"
#include "OffLineBaker/OfflineRender.h"
#include "LocalMapResMgr.h"

BEGIN_VERSION_MAP(CGamePointLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGamePointLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	int			  nGridIndex = 0;

	vector<CTObjectPtr>	vecPointLightObejts;
	size_t nSceneLightCnt = vecPointLightObejts.size();
	wdi.Write(&nSceneLightCnt, sizeof(nSceneLightCnt));
	
	// chunk raw data
	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecPointLightObejts.end();
	for (beg = vecPointLightObejts.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		// Grid Index within current region
		nGridIndex = pModelGroup->GetCenterGridIndex();
		wdi.Write(&nGridIndex, sizeof(nGridIndex));

		CMatrix mat;
		pModelGroup->GetFinalMatrix(mat);
		// world pos
		CVector3f worldPos = CVector3f(mat._41, mat._42, mat._43);
		wdi.Write(&worldPos, sizeof(worldPos));

		unsigned __int16 Rang =     short(pModelGroup->m_SL.m_Rang);
		byte B  = GetBValue(pModelGroup->m_SL.m_Color);
		byte G  = GetGValue(pModelGroup->m_SL.m_Color);
		byte R  = GetRValue(pModelGroup->m_SL.m_Color);
		byte dB = GetBValue(pModelGroup->m_SL.m_Dyn_Color);
		byte dG = GetGValue(pModelGroup->m_SL.m_Dyn_Color);
		byte dR = GetRValue(pModelGroup->m_SL.m_Dyn_Color);

		ScenePointLight_Gac temp =
		{
			pModelGroup->m_SL.m_Attenuation0,
			pModelGroup->m_SL.m_Attenuation1,
			pModelGroup->m_SL.m_Attenuation2,
			Rang,
			R,
			G,
			B,
			dR,
			dG,
			dB,
			pModelGroup->m_SL.m_Dyn_ChangeProbability,
			pModelGroup->m_SL.m_Dyn_BrightTime,
			pModelGroup->m_SL.m_Dyn_DarkTime,
			pModelGroup->m_SL.m_Dyn_ChangeTime
		};

		wdi.Write(&temp, sizeof(temp));
	}


	return true;
}

bool sqr::CGamePointLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CTerrainMesh *pTerrain =CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	int			  nGridIndex = 0;

	size_t pointlightsize = CLoadMapResInfo::GetInst()->vecEditorPointLightOffsetInfo.size();
	size_t size = 0, editorInfoIndex = 0;
	tData.SafeRead( &size, sizeof(size) );

	// chunk raw data
	for ( uint16 i = 0; i < size; ++i )
	{
		// Grid Index within current region
		nGridIndex = 0;
		tData.SafeRead( &nGridIndex, sizeof(nGridIndex) );

		// world pos
		CVector3f worldPos;
		tData.SafeRead( &worldPos, sizeof(worldPos) );

 		ScenePointLight_Gac temp;
		tData.SafeRead( &temp, sizeof(temp) );

		SceneLight light;

		light = temp;
		light.m_bIAmbScenePointLight = true;

		MoldeSaveProperty sModelSavePro;
		sModelSavePro.nCenterGridIndex = nGridIndex;
		sModelSavePro.vPosition	= worldPos;

		if( editorInfoIndex < pointlightsize )
		{
			CLoadMapResInfo::EditorOffsetInfo *pEditorOffsetInfo = CLoadMapResInfo::GetInst()->vecEditorPointLightOffsetInfo[editorInfoIndex];
			sModelSavePro.vUserOffsetEx = pEditorOffsetInfo->vUserOffsetEx;
			++editorInfoIndex;
		}

		//DebugOut("load point %f, %f, %f, grid %d\n", sModelSavePro.vUserOffsetEx.x, sModelSavePro.vUserOffsetEx.y, sModelSavePro.vUserOffsetEx.z, nGridIndex);

		if(CreateSceneObjectMgr::GetInst()->Rebirth(&sModelSavePro, "effectdummy", "tile/地编专用/model/effectdummy.mod", "", "场景光源\\Standar", 0, "", false, true, false, false))
		{
			///set color, ani, effect
			nGridIndex = sModelSavePro.nCenterGridIndex;
			SGrid &grid = pTerrain->GetGrid(nGridIndex);
			CEditModelGroup *pModelGroup = NULL;

			size_t objsize = grid.vecCenterObjects.size();
			CTObjectPtr p = grid.vecCenterObjects[objsize-1];
			pModelGroup = p->GetModelGroup();
			if( pModelGroup )
			{
				pModelGroup->m_SL = light;
				pModelGroup->SetIsLightEffect(true);
			
				OfflineRender::GetInst()->UpdateGridLights(nGridIndex);
				//insert
				pTerrain->m_DynamicLightGrids.insert(nGridIndex);
			}
		}
	}

	
	return true;
}

BEGIN_VERSION_MAP(CGameSpotLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameSpotLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	int			  nGridIndex = 0;

	vector<CTObjectPtr> vecSpotLightObejts;
	size_t nSceneLightCnt = vecSpotLightObejts.size();
	wdi.Write(&nSceneLightCnt, sizeof(nSceneLightCnt));

	// chunk raw data
	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecSpotLightObejts.end();
	for (beg = vecSpotLightObejts.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		// Grid Index within current region
		nGridIndex = pModelGroup->GetCenterGridIndex();
		wdi.Write(&nGridIndex, sizeof(nGridIndex));

		CMatrix mat;
		pModelGroup->GetFinalMatrix(mat);
		// Y offset
		// world pos
		CVector3f worldPos = CVector3f(mat._41, mat._42, mat._43);
		wdi.Write(&worldPos, sizeof(worldPos));

		unsigned __int16 Rang	 =     short(pModelGroup->m_SpotLight.m_fRang);
		unsigned __int16 FallOff =     short(pModelGroup->m_SpotLight.m_fFallOff);
		byte B  = GetBValue(pModelGroup->m_SpotLight.m_Color);
		byte G  = GetGValue(pModelGroup->m_SpotLight.m_Color);
		byte R  = GetRValue(pModelGroup->m_SpotLight.m_Color);

		SceneSpotLight_Gac temp =
		{
			pModelGroup->m_SpotLight.m_fAttenuation0,
			pModelGroup->m_SpotLight.m_fAttenuation1,
			pModelGroup->m_SpotLight.m_fAttenuation2,
			pModelGroup->m_SpotLight.m_Direction.x,
			pModelGroup->m_SpotLight.m_Direction.y,
			pModelGroup->m_SpotLight.m_Direction.z,
			pModelGroup->m_SpotLight.m_TargetPos.x,
			pModelGroup->m_SpotLight.m_TargetPos.y,
			pModelGroup->m_SpotLight.m_TargetPos.z,
			pModelGroup->m_SpotLight.m_fTheta,
			pModelGroup->m_SpotLight.m_fPhi,
			Rang,
			FallOff,
			R,
			G,
			B,
		};

		wdi.Write(&temp, sizeof(temp));
	}


	return true;
}

bool sqr::CGameSpotLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CTerrainMesh *pTerrain =CMapEditApp::GetInst()->GetTerrain();
	int			  nGridIndex = 0;

	size_t size = CLoadMapResInfo::GetInst()->vecEditorSpotLightOffsetInfo.size();
	size_t nSceneLightCnt = 0, editorInfoIndex = 0;
	tData.SafeRead( &nSceneLightCnt, sizeof(nSceneLightCnt) );

	// chunk raw data
	for ( size_t i = 0; i < nSceneLightCnt; ++i )
	{
		// Grid Index within current region
		tData.SafeRead( &nGridIndex, sizeof(nGridIndex) );

		// Y offset
		CVector3f worldPos;
		tData.SafeRead( &worldPos, sizeof(worldPos) );

		SceneSpotLight_Gac temp;
		tData.SafeRead( &temp, sizeof(temp) );

		SceneSpotLight light;

		light = temp;
		light.m_bIAmbSceneSpotLight = true;

		MoldeSaveProperty sModelSavePro;
		sModelSavePro.nCenterGridIndex = nGridIndex;
		sModelSavePro.vPosition	= worldPos;

		if( editorInfoIndex < size )
		{
			CLoadMapResInfo::EditorOffsetInfo *pEditorOffsetInfo = CLoadMapResInfo::GetInst()->vecEditorSpotLightOffsetInfo[editorInfoIndex];
			sModelSavePro.vUserOffsetEx = pEditorOffsetInfo->vUserOffsetEx;
			++editorInfoIndex;
		}

		//DebugOut("load spot %f, %f, %f, grid %d\n", sModelSavePro.vUserOffsetEx.x, sModelSavePro.vUserOffsetEx.y, sModelSavePro.vUserOffsetEx.z, nGridIndex);
		if(CreateSceneObjectMgr::GetInst()->Rebirth(&sModelSavePro, "effectdummy", "tile/地编专用/model/effectdummy.mod", "", "场景光源\\Standar", 0, "", false, true, false, false))
		{
			///set color, ani, effect
			nGridIndex = sModelSavePro.nCenterGridIndex;
			SGrid &grid = pTerrain->GetGrid(nGridIndex);
			CEditModelGroup *pModelGroup = NULL;

			size_t objsize = grid.vecCenterObjects.size();
			CTObjectPtr p = grid.vecCenterObjects[objsize-1];
			pModelGroup = p->GetModelGroup();
			if( pModelGroup )
			{
				pModelGroup->m_SpotLight = light;
				pModelGroup->SetIsLightEffect(true);

				OfflineRender::GetInst()->UpdateGridLights(nGridIndex);
				//insert
				pTerrain->m_DynamicLightGrids.insert(nGridIndex);
			}
		}
	}

	return true;
}

CGameLightFormat::CGameLightFormat(void)
: CFileFormatMgr( 'TLFT', 0 )
{
	InitLightCode();
}

CGameLightFormat::~CGameLightFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameLightFormat::InitLightCode()
{
	RegisterCoder(new CGamePointLightCoder);
	RegisterCoder(new CGameSpotLightCoder);
}

bool sqr::CGameLightFormat::LoadLightFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CGameLightFormat::SaveLightFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) ); 
}