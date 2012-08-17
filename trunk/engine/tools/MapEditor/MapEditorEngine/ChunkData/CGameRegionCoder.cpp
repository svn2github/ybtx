#include "stdafx.h"
#include "../ChunkData/CGameRegionCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "LoadMapMgr.h"

BEGIN_VERSION_MAP(CGameRegionCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameRegionCoder::Code( WirteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);

	return true;
}

bool sqr::CGameRegionCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CVector3f center;
	bool	bNeedCal = false;
	sqr::uint16 nSceneRgnIndex = 0;

	for ( int n = 0; n < TILE_WIDTH_IN_REGION; ++n )
	{
		for ( int m = 0; m < REGION_DIVIDE; ++m )
		{
			///该区域对应的区域名字所对应的索引
			tData.SafeRead( &nSceneRgnIndex, sizeof(nSceneRgnIndex) );
			tData.SafeRead( &center.x,		 sizeof(float) );
			tData.SafeRead( &center.z,		 sizeof(float) );
			tData.SafeRead( &bNeedCal,		 sizeof(bNeedCal) );
		}
	}

	return true;
}

CGameRegionFormat::CGameRegionFormat(void)
: CFileFormatMgr( 'TRFT', 0 )
{
	InitRegionCode();
}

CGameRegionFormat::~CGameRegionFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameRegionFormat::InitRegionCode()
{
	RegisterCoder(new CGameRegionCoder);
}

bool sqr::CGameRegionFormat::LoadRegionFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CGameRegionFormat::SaveRegionFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) ); 
}