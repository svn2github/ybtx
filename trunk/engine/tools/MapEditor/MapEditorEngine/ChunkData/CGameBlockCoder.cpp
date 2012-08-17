#include "stdafx.h"
#include "../ChunkData/CGameBlockCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"

BEGIN_VERSION_MAP(CGameBlcokCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameBlcokCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	int nGridCnt = pTerrain->GetGridCount();

	for ( int i  = 0; i < nGridCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);

		wdi.Write(&grid.nBlockType,		sizeof(int));				///normal block
		wdi.Write(&grid.subGridTypes,	sizeof(grid.subGridTypes)); ///exact  block
	}

	return true;
}

bool sqr::CGameBlcokCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain =CMapEditApp::GetInst()->GetTerrain();
	int nBlockType = 0;
	int nGridCnt = pTerrain->GetGridCount();
	
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	for ( int i  = 0; i < nGridCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);

		tData.SafeRead( &grid.nBlockType,	sizeof(nBlockType) );
		tData.SafeRead( &grid.subGridTypes, sizeof(grid.subGridTypes));
	}
	
	return true;
}

class CAnyObject;


CGameBlcokFormat::CGameBlcokFormat(void)
: CFileFormatMgr( 'TBFT', 0 )//'TBFT' tgt block format
{
	InitBlockCode();
}

CGameBlcokFormat::~CGameBlcokFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameBlcokFormat::InitBlockCode()
{
	RegisterCoder(new CGameBlcokCoder);
}

bool sqr::CGameBlcokFormat::LoadBlcokFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Load( strFile, pObj );
}

bool sqr::CGameBlcokFormat::SaveBlcokFile( const string& strFile )
{
	CAnyObject* pObj = (CAnyObject*)(0xeefefeff); //指向要读取/存储的对象
	return Save( strFile, pObj ); 
}