#include "stdafx.h"
#include "../ChunkData/CEditorLightCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "LocalMapResMgr.h"

BEGIN_VERSION_MAP(CEditorPointLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorPointLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	
	vector<CTObjectPtr> vecPointLightObejts;
	size_t		  size = vecPointLightObejts.size();
	wdi.Write(&size,		sizeof(size));

	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecPointLightObejts.end();
	for (beg = vecPointLightObejts.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		CLoadMapResInfo::EditorOffsetInfo emodelInfo;
		emodelInfo.vUserOffsetEx  = pModelGroup->GetUserOffsetEx();

		//DebugOut("Load vOffsetOverall %f, %f, %f\n", emodelInfo.vOffsetOverall.x, emodelInfo.vOffsetOverall.y, emodelInfo.vOffsetOverall.z);

		wdi.Write(&emodelInfo,		sizeof(emodelInfo));
	}
	return true;
}

bool sqr::CEditorPointLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
	for (size_t i = 0; i < size; ++i)
	{
		//tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
		CLoadMapResInfo::EditorOffsetInfo *pEditorOffsetInfo = new CLoadMapResInfo::EditorOffsetInfo;
		tData.SafeRead( pEditorOffsetInfo,	sizeof(CLoadMapResInfo::EditorOffsetInfo) );
		CLoadMapResInfo::GetInst()->vecEditorPointLightOffsetInfo.push_back(pEditorOffsetInfo);

		//DebugOut("Save vOffsetOverall %f, %f, %f\n", pEditorModelInfo->vOffsetOverall.x, pEditorModelInfo->vOffsetOverall.y, pEditorModelInfo->vOffsetOverall.z);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CEditorSpotLightCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorSpotLightCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	
	vector<CTObjectPtr> vecSpotLightObejts;
	size_t		  size = vecSpotLightObejts.size();
	wdi.Write(&size,		sizeof(size));

	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecSpotLightObejts.end();
	for (beg = vecSpotLightObejts.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		CLoadMapResInfo::EditorOffsetInfo emodelInfo;
		emodelInfo.vUserOffsetEx  = pModelGroup->GetUserOffsetEx();

		wdi.Write(&emodelInfo,		sizeof(emodelInfo));
	}
	return true;
}

bool sqr::CEditorSpotLightCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
	for (size_t i = 0; i < size; ++i)
	{
		//tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
		CLoadMapResInfo::EditorOffsetInfo *pEditorOffsetInfo = new CLoadMapResInfo::EditorOffsetInfo;
		tData.SafeRead( pEditorOffsetInfo,	sizeof(CLoadMapResInfo::EditorOffsetInfo) );
		CLoadMapResInfo::GetInst()->vecEditorSpotLightOffsetInfo.push_back(pEditorOffsetInfo);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


CEditorLightFormat::CEditorLightFormat(void)
: CFileFormatMgr( 'ELFT', 0 )
{
	InitLightCode();
}

CEditorLightFormat::~CEditorLightFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorLightFormat::InitLightCode()
{
	RegisterCoder(new CEditorPointLightCoder);
	RegisterCoder(new CEditorSpotLightCoder);
}

bool sqr::CEditorLightFormat::LoadLightFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CEditorLightFormat::SaveLightFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}