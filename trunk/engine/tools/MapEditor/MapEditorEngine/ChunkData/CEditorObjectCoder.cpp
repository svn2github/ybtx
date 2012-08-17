#include "stdafx.h"
#include "../ChunkData/CEditorObjectCoder.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "DepathGroupModelInfo.h"
#include "LoadMapMgr.h"
#include "LocalMapResMgr.h"

BEGIN_VERSION_MAP(CEditorObjectCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

///该类主要用于处理物体包围盒
bool sqr::CEditorObjectCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	size_t		  size = 0;

	vector<CTObjectPtr> vecSingleObjects;
	size = vecSingleObjects.size();
	wdi.Write(&size,		sizeof(size));

	DWORD dwGridIndex = 0;

	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecSingleObjects.end();
	for (beg = vecSingleObjects.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		if (pModelGroup->m_SL.m_bIAmbScenePointLight)  
			continue;
		if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight)  
			continue;
		if (pModelGroup->GetIsSubUnit())            
			continue;
		if(pModelGroup->m_pSceneAudio)
			continue;
		if(pModelGroup->m_bLoginModel)
			continue;
		if(pModelGroup->m_bReginEffect)
			continue;

		CLoadMapResInfo::EditorModelInfo emodelInfo;
		emodelInfo.vUserOffsetEx  = pModelGroup->GetUserOffsetEx();
		emodelInfo.bBakingColor   = pModelGroup->GetBakingColorIsNotGreaterShadow();
		emodelInfo.nMirrorType    = pModelGroup->GetMirrorType();
		emodelInfo.bTransState	  = pModelGroup->GetModelTransState(0);	
		emodelInfo.nAffectRatio   = pModelGroup->GetAffectRatio();

		//DebugOut("Load vOffsetOverall %f, %f, %f\n", emodelInfo.vOffsetOverall.x, emodelInfo.vOffsetOverall.y, emodelInfo.vOffsetOverall.z);
		//DebugOut("Load vUserOffsetEx  %f, %f, %f\n", emodelInfo.vUserOffsetEx.x, emodelInfo.vUserOffsetEx.y, emodelInfo.vUserOffsetEx.z);

		wdi.Write(&emodelInfo,		sizeof(emodelInfo));
	}

	return true;
}

bool sqr::CEditorObjectCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{	
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
	CLoadMapResInfo::GetInst()->editorSingleModelCnt = size;
	//DebugOut("modelcnt %d\n", size);
	for (size_t i = 0; i < size; ++i)
	{
		//tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
		CLoadMapResInfo::EditorModelInfo *pEditorModelInfo = new CLoadMapResInfo::EditorModelInfo;
		tData.SafeRead( pEditorModelInfo,	sizeof(CLoadMapResInfo::EditorModelInfo) );
		CLoadMapResInfo::GetInst()->vecEditorSingleModelInfo.push_back(pEditorModelInfo);

		//DebugOut("Save vOffsetOverall %f, %f, %f\n", pEditorModelInfo->vOffsetOverall.x, pEditorModelInfo->vOffsetOverall.y, pEditorModelInfo->vOffsetOverall.z);
		//DebugOut("Save vUserOffsetEx  %f, %f, %f\n", pEditorModelInfo->vUserOffsetEx.x, pEditorModelInfo->vUserOffsetEx.y, pEditorModelInfo->vUserOffsetEx.z);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorCompositeObjectCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorCompositeObjectCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	vector<CTObjectPtr> vecCompositeObjects;
	size_t		  size = vecCompositeObjects.size();
	string		  strEffectName("");

	wdi.Write(&size,		sizeof(size));

//	DebugOut("save edit compose %d%d\n", size);
	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecCompositeObjects.end();
	for (beg = vecCompositeObjects.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		if (pModelGroup->GetIsSubUnit() && !pModelGroup->m_bLoginModel && !pModelGroup->m_bReginEffect )            
		{
			// effect bound?
			bool bEffectBased = pModelGroup->GetIsEffectBased();
			bool bEffecIsExit = pModelGroup->GetEffectName(strEffectName);
			if ( bEffectBased && bEffecIsExit )
			{
				bool b = true;
				wdi.Write(&b,		sizeof(b));
				
				DWORD dwLen = strEffectName.size();
				wdi.Write(&dwLen,		sizeof(dwLen));
				wdi.Write(strEffectName.c_str(), dwLen);
			}
			else
			{
				bool b = false;
				wdi.Write(&b,		sizeof(b));
			}

			string strItemName = pModelGroup->GetItemName();

			DWORD dwLen = strItemName.size();
			wdi.Write(&dwLen,		sizeof(dwLen));
			wdi.Write(strItemName.c_str(), dwLen);

			//////////////////////////////////////////////////////////////////////////
			///nAffectRadio
			int nAffectRatio = pModelGroup->GetAffectRatio();
			wdi.Write(&nAffectRatio,		sizeof(nAffectRatio));

			///save model property
			CMapEditObject *pMapEditObject = NULL;
			pMapEditObject = pModelGroup->GetSelfPresentation(0);
			if( pModelGroup == NULL )
			{
				MessageBox(NULL,"没有模型，不保存","",MB_OK);
			}

			wdi.Write(&(pMapEditObject->sModelSavePro),		sizeof(MoldeSaveProperty));
		}
	}
	return true;
}

bool sqr::CEditorCompositeObjectCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CTerrainMesh  *pTerrain  = CMapEditApp::GetInst()->GetTerrain();
	COperator	  *pOperator = CMapEditApp::GetInst()->GetOperator();
	int			  nGridIndex = 0;
	size_t		  nResSize   = CLoadMapResInfo::GetInst()->m_SetUnit.size();
	int			  editormodelInfoIndex = 0;

	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
//	DebugOut("load edit compose %d\n", size);
	for (size_t i = 0; i < size; ++i)
	{
		// effect bound?
		bool bEffectBound = false;
		tData.SafeRead( &bEffectBound,	  sizeof(bEffectBound) );
		
		string strEffectName = "";
		if ( bEffectBound )
		{
			DWORD dwItemNameLen = 0;
			tData.SafeRead( &dwItemNameLen,	  sizeof(dwItemNameLen) );
			strEffectName.resize(dwItemNameLen);
			tData.SafeRead( &strEffectName[0], dwItemNameLen );
		}

		//////////////////////////////////////////////////////////////////////////

		DWORD dwItemNameLen = 0;
		string strItemName;
		tData.SafeRead( &dwItemNameLen,	  sizeof(dwItemNameLen) );
		strItemName.resize(dwItemNameLen);
		tData.SafeRead( &strItemName[0], dwItemNameLen );

		//////////////////////////////////////////////////////////////////////////
		///nAffectRadio
		int nAffectRadio = 0;
		tData.SafeRead( &nAffectRadio, sizeof(nAffectRadio)  );

		MoldeSaveProperty sModelSavePro;
		tData.SafeRead( &sModelSavePro, sizeof(MoldeSaveProperty) );

		//DebugOut("Grid %d, %d, %s\n", i, sModelSavePro.nCenterGridIndex, strItemName.c_str());
		pOperator->Rebirth(&sModelSavePro, strItemName, "", nAffectRadio, strItemName);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CEditorLogicObjectCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorLogicObjectCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	vector<CTObjectPtr> vecLoginObejects;
	size_t		  size = vecLoginObejects.size();

	wdi.Write(&size,		sizeof(size));

	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecLoginObejects.end();
	for (beg = vecLoginObejects.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		CLoadMapResInfo::EditorModelInfo emodelInfo;
		emodelInfo.vUserOffsetEx  = pModelGroup->GetUserOffsetEx();
		emodelInfo.bBakingColor   = pModelGroup->GetBakingColorIsNotGreaterShadow();
		emodelInfo.nMirrorType    = pModelGroup->GetMirrorType();
		emodelInfo.bTransState	  = pModelGroup->GetModelTransState(0);
		emodelInfo.nAffectRatio   = pModelGroup->GetAffectRatio();

		//DebugOut("Load vOffsetOverall %f, %f, %f\n", emodelInfo.vOffsetOverall.x, emodelInfo.vOffsetOverall.y, emodelInfo.vOffsetOverall.z);
		//DebugOut("Load vUserOffsetEx  %f, %f, %f\n", emodelInfo.vUserOffsetEx.x, emodelInfo.vUserOffsetEx.y, emodelInfo.vUserOffsetEx.z);

		wdi.Write(&emodelInfo,		sizeof(emodelInfo));
	}

	return true;
}

bool sqr::CEditorLogicObjectCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nGridIndex = 0;

	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
	CLoadMapResInfo::GetInst()->editorLoginModelCnt = size;
	for (size_t i = 0; i < size; ++i)
	{
		//tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
		CLoadMapResInfo::EditorModelInfo *pEditorModelInfo = new CLoadMapResInfo::EditorModelInfo;
		tData.SafeRead( pEditorModelInfo,	sizeof(CLoadMapResInfo::EditorModelInfo) );
		CLoadMapResInfo::GetInst()->vecEditorLoginModelInfo.push_back(pEditorModelInfo);

		//DebugOut("Save vOffsetOverall %f, %f, %f\n", pEditorModelInfo->vOffsetOverall.x, pEditorModelInfo->vOffsetOverall.y, pEditorModelInfo->vOffsetOverall.z);
		//DebugOut("Save vUserOffsetEx  %f, %f, %f\n", pEditorModelInfo->vUserOffsetEx.x, pEditorModelInfo->vUserOffsetEx.y, pEditorModelInfo->vUserOffsetEx.z);
	}

	return true;
}


CEditorObjectFormat::CEditorObjectFormat(void)
: CFileFormatMgr( 'SOFT', 0 )
{
	InitObjectCode();
}

CEditorObjectFormat::~CEditorObjectFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorObjectFormat::InitObjectCode()
{
	RegisterCoder(new CEditorObjectCoder);
	RegisterCoder(new CEditorCompositeObjectCoder);
	RegisterCoder(new CEditorLogicObjectCoder);
}

bool sqr::CEditorObjectFormat::LoadObjectFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CEditorObjectFormat::SaveObjectFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}