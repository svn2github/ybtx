#include "stdafx.h"
#include "CSceneArgCoder.h"
#include "CSceneManagerClient.h"
#include "CSceneConfig.h"
#include "CMetaRegion.h"
#include "CMetaGridMgr.h"
#include "GraphicCommon.h"
#include "float.h"
#include "StringHelper.h"
#include "CWaterMgr.h"
#include "TSqrAllocator.inl"
#include "CPkgFile.h"

#define ROTATION_STEP		(3.141592654f*2.0f/256.0f)

BEGIN_VERSION_MAP(CSceneArgCoder)
	ON_VERSION( 0x0000000B , DeCode_Ver_C )
	ON_VERSION( 0x0000000C , DeCode_Ver_C )
	ON_VERSION( 0x0000000D , DeCode_Ver_C )
	ON_VERSION( 0x0000000E , DeCode_Ver_C )
END_VERSION_MAP()

enum
{
	AUDIO_ORIGIN,
	AUDIO_PLAY_LEN,
};

bool CSceneArgCoder::SceneDeCode( CDataScene* pSceneManager,CPkgFile& File  )
{
	m_strMapName = pSceneManager->GetSceneName();
	uint32 nMagicID,nVersion;
	bool isDeCode = false;
	File.Read(&nMagicID, sizeof(uint32));
	File.Read(&nVersion, sizeof(uint32));

	if( nVersion < CLIENT_VERSION )
		m_uModelVersion = DEFAULT_OLDMODELPRO;
	else
		File.Read(&m_uModelVersion, sizeof(uint32));

	if( nMagicID == 'NGER' )
	{
		CDataChunk temp;
		m_pScene = (CSceneManagerClient*)pSceneManager;
		m_pScene->m_pGridMgr->Alloc( m_pScene->m_uWidthInGrid, m_pScene->m_uHeightInGrid );
		m_pScene->m_pMapPath = m_pScene->CreateIMap( m_pScene->m_uWidthInRegion, m_pScene->m_uHeightInRegion );
		//SetDeCodeVersion(nVersion);
		temp.SetVersion(nVersion);
		temp.SetBuffer((char*)&File,sizeof(char*));
		isDeCode = DeCode(temp,pSceneManager);
	}
	return isDeCode;
}

bool CSceneArgCoder::DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle )
{
	m_pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());
	
	bool isDeCode = false;

	uint32 dwClient = 0,nCount = 0;
	File.read(&nCount, sizeof(uint32));
	File.read(&dwClient, sizeof(uint32));

	m_pScene->m_uRegionID = dwClient;
	int32 nOffset;
	uint32 nPos	= File.Tell();
	uint32 nMaxIdx = m_pScene->m_uHeightInRegion * m_pScene->m_uWidthInRegion;
	for( uint32 i = 0; i < m_pScene->m_uHeightInRegion; ++i )
	{
		for(uint32 j = 0; j < m_pScene->m_uWidthInRegion ; ++j )
		{		
			uint32 uIdx = i*m_pScene->m_uWidthInRegion + j;
			File.Seek( nPos + uIdx*sizeof( uint32 ), SEEK_SET );
			File.read( &nOffset, sizeof(uint32));
			if( nOffset >= 0 )
			{
				File.Seek( nOffset, SEEK_SET );
				LoadOneRegion( File, nCount, uIdx );
				m_pScene->DoProcess(  ((float)uIdx/(float)nMaxIdx)*10.0f + 10.0f );
			}
		}
	}
	return true;
}
void CSceneArgCoder::LoadOneRegion( CPkgFile& File, uint32 nChunk, uint32 uRegionId )
{
	m_pScene->m_pGridMgr->SetRegion( uRegionId, new CMetaRegion( static_cast<uint16>(uRegionId) ) );
	uint32 nPos	= File.Tell();
	// 载入每个区块
	RegionChunkHdr		rchdr;
	for(uint32 i = 0; i < nChunk; i++)
	{
		File.Seek(nPos, SEEK_SET);
		File.read(&rchdr.nMagicID, sizeof(uint32));
		File.read(&rchdr.nOffset, sizeof(uint32));
		File.read(&rchdr.nSize, sizeof(uint32));
		nPos = File.Tell();

		File.Seek(rchdr.nOffset, SEEK_SET);

		LoadOneChunk( File, uRegionId, rchdr.nMagicID ); // 分区域
	}
}

void CSceneArgCoder::LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID )
{
	switch(uMagicID)
	{
	case SRGN:
		LoadSubRegionInfo( File, uRegionId );
		break;
	case VHGT:
		LoadTerranHeight( File, uRegionId );		//m_nTerranHeight
		break;
	case VDIF:
		LoadDiffuse( File, uRegionId );				//m_ncTerranColor
		break;
	case VSPE:
		LoadSpecular( File, uRegionId );				//m_ncTerranColor
		break;
	case VTEX:
		LoadTexture( File, uRegionId );				//m_Grids
		break;
	case MODL:
		LoadModel( File, uRegionId );				//m_pModelInfo
		break;
	case LHGT:
		LoadOffsetHeight( File, uRegionId );		//m_nLogicHeight
		break;
	case WATR:
		LoadWater( File, uRegionId );
		break;
	case SCLT:
		LoadScenePointLight( File, uRegionId );			//point light info
		break;
	case SPLT:
		LoadSceneSpotLight( File, uRegionId );			//point light info
		break;
	case TRAP:
		LoadSceneTrap( File, uRegionId );
		break;
	case GATR:
		LoadRegionBlock(File, uRegionId );
		break;
	case EBLK:
		LoadRegionExactBlock(File, uRegionId );
		break;
	case AUID:
		LoadRegionSceneAudio(File, uRegionId );
		break;
	case LOGI:
		LoadRegionLoginModel(File, uRegionId );
		break;
	case TGMI:
		LoadRegionMaterialIndex(File, uRegionId );
		break;
	case RGFG:
	case VNOR:
	case SBVT:
	case GHGT:
	case OBJT:
		break;
	}
}

void CSceneArgCoder::LoadOffsetHeight( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	uint16 LogicHeightNum = 0;
	File.read(&LogicHeightNum, sizeof(uint16));
	for( uint16 i = 0; i < LogicHeightNum; i++ )
	{
		uint16 VertexIndex;
		File.read(&VertexIndex, sizeof(uint16));

		if( VertexIndex >= eGridNumPerRegion )
		{
			string msg = format("地图: %s Region %d LoadOffsetHeight Load Invalid Vertex Index!", m_strMapName.c_str(), uRegionId);
			GenErr(GraphicErr::GetErrTypeStr(FileRead_Err),msg);
		}

		int16 nTmp;
		File.read(&nTmp, sizeof(int16));
		pRgn->SetOffsetHeight( VertexIndex, nTmp );
	}
}

void CSceneArgCoder::LoadSubRegionInfo( CPkgFile& File, uint32 uRegionId )
{
	int nDivideRegIndex = (uRegionId/m_pScene->m_uWidthInRegion) * m_pScene->m_uWidthInRegion * eSubRegNum + (uRegionId%m_pScene->m_uWidthInRegion) * eRegDivideNum;
	int nDivideIndex = 0, nWidthInSubRegion = m_pScene->m_uWidthInRegion * 2;
	uint32 index = 0, left = 0, right = 0;
	uint16	  nSetRegionIndex = 0;		///属于哪个划分区域索引
	CVector3f center;
	bool	  bNeedReCal = false;
	SVector<int> SList;
	for( int m = 0; m < eRegDivideNum; ++m )
	{
		for( int n = 0; n < eRegDivideNum; ++n )
		{
			SubRegionData *pSubRgn = NULL;
			pSubRgn = new SubRegionData;
			if( pSubRgn )
			{
				nDivideIndex = nDivideRegIndex + n + m * nWidthInSubRegion;

				///读取信息
				File.read(&nSetRegionIndex, sizeof(nSetRegionIndex));
				File.read(&center.x,		sizeof(float));
				File.read(&center.z,		sizeof(float));
				File.read(&bNeedReCal,		sizeof(bNeedReCal));
				center.y = 0.0f;

				//九宫格遍历法
				for ( int i = - nWidthInSubRegion; i <= nWidthInSubRegion; i += nWidthInSubRegion )
				{
					index = nDivideIndex + i;
					if ( index >= 0 && index < m_pScene->m_uSubRegionNum )
					{
						SList.push_back(index);
					}

					left = nDivideIndex % m_pScene->m_uWidthInSubRegion ? index-1 : -1;
					if ( left >= 0 && left < m_pScene->m_uSubRegionNum )
					{
						SList.push_back(left);
					}

					right = (nDivideIndex + 1) % m_pScene->m_uWidthInSubRegion ? index + 1 : -1;
					if ( right >= 0 && right < m_pScene->m_uSubRegionNum )
					{
						SList.push_back(right);
					}

				}

				pSubRgn->m_nSetRegionIndex = nSetRegionIndex;
				pSubRgn->m_vCenter = center;
				pSubRgn->m_bNeedRealCal = bNeedReCal;
				pSubRgn->m_vSubRgnAjenctList = SList;

				m_pScene->m_SubRegionsMap[nDivideIndex] = pSubRgn;

				SList.clear();
			}
		}
	}
}

void CSceneArgCoder::LoadTerranHeight( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	for ( int i = 0; i < eGridNumPerRegion; i++ )
	{
		short nTmp;
		File.read(&nTmp, sizeof(short));
		pRgn->SetTerranHeight( i, nTmp );
	}
}

void CSceneArgCoder::LoadDiffuse( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	DWORD  dwColor = 0;

	for( int32 i = 0; i < eGridNumPerRegion; ++i )
	{
		File.read(&dwColor, sizeof(DWORD));
		pRgn->SetTerranDiffuseColor( i, CColor(dwColor) );

		File.read(&dwColor, sizeof(DWORD));
		pRgn->SetTerranBakingColor( i, CColor(dwColor) );
	}
}

void CSceneArgCoder::LoadSpecular( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	DWORD  dwColor = 0;

	for( int32 i = 0; i < eGridNumPerRegion; ++i )
	{
		File.read(&dwColor, sizeof(DWORD));
		pRgn->GetTerranSpecularColor( i, CColor(dwColor) );
	}
}

void CSceneArgCoder::LoadTexture( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );

	uint8 byFirst,bySecond, byThird, byFourth;
	WORD first = 0, second = 0;
	CVector2f uv;
	bool bGridHide = false;

	for ( uint32 i = 0; i < eGridNumPerRegion; i++ )
	{		
		File.read(&byFirst, sizeof(uint8));
		File.read(&bySecond, sizeof(uint8));
		File.read(&byThird, sizeof(uint8));
		File.read(&byFourth, sizeof(uint8));

		first = MAKEWORD(bySecond, byFirst);
		second = MAKEWORD(byFourth, byThird); 

		pRgn->SetTextureIndex( i, MAKELONG( second, first ) );

		//载入纹理坐标
		File.read(&uv.x, sizeof(float));
		File.read(&uv.y, sizeof(float));
		pRgn->SetTerranLayer1UV( i, uv );

		File.read(&uv.x, sizeof(float));
		File.read(&uv.y, sizeof(float));
		pRgn->SetTerranLayer2UV( i, uv );

		File.read(&uv.x, sizeof(float));
		File.read(&uv.y, sizeof(float));
		pRgn->SetTerranLayer3UV( i, uv );

		File.read(&uv.x, sizeof(float));
		File.read(&uv.y, sizeof(float));
		pRgn->SetTerranLayer4UV( i, uv );


		//grid hide mask
		File.read(&bGridHide, sizeof(bool));
		pRgn->SetTerrainGridIsHollow( i, bGridHide );
	}
}

void CSceneArgCoder::LoadModel( CPkgFile& File, uint32 uRegionId )
{
	uint16 uModelNum = 0;
	File.read( &uModelNum, sizeof(uint16) );
	if( uModelNum == 0 )
		return;

	CMetaRegion* pRegion = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	pRegion->GetModelInfo().resize( uModelNum );

	switch (m_uModelVersion)
	{
	case DEFAULT_OLDMODELPRO:
		{
			CRegionOldMI sRegOldModelInfo;
			for( uint16 i = 0; i < uModelNum; ++i )
			{
				File.read( &sRegOldModelInfo, sizeof(CRegionOldMI) );

				CRegionMI &RegModelInfo = pRegion->GetModelInfo()[i];

				RegModelInfo.cX = sRegOldModelInfo.cX;
				RegModelInfo.cY = sRegOldModelInfo.cY;
				RegModelInfo.cZ = sRegOldModelInfo.cZ;
				
				RegModelInfo.SetIndex = sRegOldModelInfo.SetIndex;
				RegModelInfo.GridIndex = sRegOldModelInfo.GridIndex;

				RegModelInfo.fyRotation = -sRegOldModelInfo.byRotation * ROTATION_STEP;//y
				RegModelInfo.fxRotation = -sRegOldModelInfo.bxRotation * ROTATION_STEP;//x
				RegModelInfo.fzRotation = -sRegOldModelInfo.bzRotation * ROTATION_STEP;//z
				
				RegModelInfo.fScaleX = sRegOldModelInfo.fScaleX;
				RegModelInfo.fScaleY = sRegOldModelInfo.fScaleY;
				RegModelInfo.fScaleZ = sRegOldModelInfo.fScaleY;
				
				RegModelInfo.nFlag = sRegOldModelInfo.nFlag;
			
				RegModelInfo.r = sRegOldModelInfo.r;
				RegModelInfo.g = sRegOldModelInfo.g;
				RegModelInfo.b = sRegOldModelInfo.b;

				RegModelInfo.bEffectByConfigur = false;
				RegModelInfo.bCameraCollision  = false;
			}
		}
		break;

	case DEFAULT_NEWMODELPRO:
		{
			for( uint16 i = 0; i < uModelNum; ++i )
			{
				File.read( &pRegion->GetModelInfo()[i], sizeof(CRegionMI) );
			}
		}
		break;

	default:
		break;
	}

	
}

void CSceneArgCoder::LoadWater( CPkgFile& File, uint32 uRegionId )
{
	uint8 nRivers = 0;
	File.read(&nRivers,sizeof(uint8)); // river count of this region
	if( nRivers == 0 )
		return;
	//m_pScene->m_vecWaterRegionIndex.push_back(uRegionId);

	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	uint32 xGrid = (uRegionId % m_pScene->m_uWidthInRegion) * eRegionSpan;
	uint32 yGrid = (uRegionId / m_pScene->m_uWidthInRegion) * eRegionSpan;//GetRegionDownY( uRegionId );

	SVector< SVector< uint8 > > RiverEdges;
	RegionWater* pWaterInfo = new RegionWater;
	pWaterInfo->m_Waters.resize( nRivers,uRegionId );
	RiverEdges.resize( nRivers );
	pRgn->SetWaterInfo( pWaterInfo );

	for( uint8 i = 0; i < nRivers; i++ )
	{
		uint32 nTextBegin, nTextCount;
		File.read(&nTextBegin ,sizeof(uint32));
		File.read(&nTextCount ,sizeof(uint32));
		pWaterInfo->m_Waters[i].SetTextureRange( nTextBegin, nTextCount );

		uint32 dwGridCnt;
		File.read(&dwGridCnt,sizeof(uint32));

		for ( uint32 n = 0; n < dwGridCnt; ++n  )
		{
			uint8 byLocalGridIndex;
			File.read(&byLocalGridIndex,sizeof(uint8));
			pWaterInfo->m_Waters[i].SetGridValid( byLocalGridIndex );
		}

		//////////////////////////////////////////////////////////////////////////

		// river side
		File.read(&dwGridCnt,sizeof(uint32));

		RiverEdges[i].resize(dwGridCnt);
		for ( uint32 n = 0; n < dwGridCnt; ++n  )
		{
			File.read(&RiverEdges[i][n],sizeof(uint8));
		}
	}

	uint32 dwVertexCnt;
	File.read(&dwVertexCnt,sizeof(uint32));
	if( dwVertexCnt > eVertexNumPerRegion )
	{
		string msg = format("地图: %s Region %d LoadWater Load Invalid VertexCnt!", m_strMapName.c_str(), uRegionId);
		GenErr(GraphicErr::GetErrTypeStr(FileRead_Err),msg);
	}

	// compute bounding box
	CVector3f minpos(FLT_MAX, FLT_MAX, FLT_MAX);
	CVector3f maxpos(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for ( uint32 i = 0; i < dwVertexCnt; ++i )
	{
		// convert to local
		uint16 wVertexIndex;
		File.read(&wVertexIndex,sizeof(uint16));
		if( wVertexIndex >= eVertexNumPerRegion )
		{
			string msg = format("地图: %s Region %d LoadWater Load Invalid VertexCnt!", m_strMapName.c_str(), uRegionId);
			GenErr(GraphicErr::GetErrTypeStr(FileRead_Err),msg);
		}

		File.read(&pWaterInfo->m_nWaterHeight[wVertexIndex],sizeof(int16));
		File.read(&pWaterInfo->m_cWaterColor[wVertexIndex],sizeof(uint32));

		uint x = (xGrid + wVertexIndex % (eRegionSpan + 1)) * eGridSpan;
		uint z = (yGrid + wVertexIndex / (eRegionSpan + 1)) * eGridSpan;

		minpos.x = min(x, minpos.x);
		maxpos.x = max(x, maxpos.x);
		minpos.y = min(pWaterInfo->m_nWaterHeight[wVertexIndex], minpos.y);
		maxpos.y = max(pWaterInfo->m_nWaterHeight[wVertexIndex], maxpos.y);
		minpos.z = min(z, minpos.z);
		maxpos.z = max(z, maxpos.z);
	}
	pWaterInfo->aabb.setExtents(minpos, maxpos);

	CMetaGridMgr& GridMgr = *m_pScene->m_pGridMgr;
	// 计算波浪方向(这里可以改成水面边缘检测生成一个mesh带)
	CVector2f vecDir[2][2] = { { CVector2f( -1, -1 ), CVector2f( 1, -1 ) }, { CVector2f( -1, 1 ), CVector2f( 1, 1 ) } };
	for( uint32 i = 0; i < nRivers; i++ )
	{
		for( SVector<uint8>::iterator it = RiverEdges[i].begin(); it != RiverEdges[i].end(); it++ )
		{
			CVector2f Dir( 0, 0 );
			CVector3f Pos;
			int32 uGridX = (*it)%eRegionSpan + xGrid;
			int32 uGridY = (*it)/eRegionSpan + yGrid;
			Pos.x = GetFloatPixelOfGrid( uGridX );
			Pos.z = GetFloatPixelOfGrid( uGridY ); 
			Pos.y = 0;
			for( int32 j = 0; j < 2; ++j )
			{
				for( int32 k = 0; k < 2; ++k )
				{
					int32 nVertexGridX = uGridX + k - 1;
					int32 nVertexGridY = uGridY + j - 1;
					if( GridMgr.IsGridValid( nVertexGridX, nVertexGridY ) )
					{
						int32 nVertexIndex = GetRegionVertexIndex( *it, k, j );
						Pos.y += pWaterInfo->m_nWaterHeight[ nVertexIndex ]/4.0f;
						if(	GridMgr.GetTerranHeight( nVertexGridX, nVertexGridY ) >= pWaterInfo->m_nWaterHeight[ nVertexIndex ] )
							Dir += vecDir[j][k];
					}
				}
			}
			pWaterInfo->m_Waters[i].SetGridDir( &( m_pScene->GetWaterMgr()->m_WaveInfo ), (*it), Dir.x, Dir.y, Pos );
		}
	}
}

void CSceneArgCoder::LoadScenePointLight( CPkgFile& File, uint32 uRegionId )
{
	uint8 uLightNum;
	File.read( &uLightNum, sizeof(uint8) );
	CMetaRegion* pRegion = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	pRegion->GetLightInfo().resize( uLightNum );
	for( uint8 i = 0; i < uLightNum; i++ )
	{
		File.read( &pRegion->GetLightInfo()[i].uGridIndex, sizeof(uint8) );
		File.read( &pRegion->GetLightInfo()[i].nHeight, sizeof(int16) );
		File.read( &pRegion->GetLightInfo()[i].LightInfo, sizeof(CRegionLI) );
	}
}

void CSceneArgCoder::LoadSceneSpotLight( CPkgFile& File, uint32 uRegionId )
{
	uint8 uLightNum;
	File.read( &uLightNum, sizeof(uint8) );
	CMetaRegion* pRegion = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	pRegion->GetLightInfo().resize( uLightNum );
	for( uint8 i = 0; i < uLightNum; ++i )
	{
		uint8 uGridIndex = 0;
		int16 nHeight = 0;
		CRegionSpotLight temp;

		File.read( &uGridIndex, sizeof(uint8) );
		File.read( &nHeight, sizeof(int16) );
		File.read( &temp, sizeof(temp) );
	}
}

void CSceneArgCoder::LoadSceneTrap( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	uint16 uTrapNum;
	File.read(&uTrapNum, sizeof(uint16));
	if( uTrapNum )
		pRgn->NewTrap();
	for( uint16 i = 0; i < uTrapNum; i++ )
	{
		uint8 uIndex;
		File.read(&uIndex, sizeof(uint8));
		if( uIndex >= eGridNumPerRegion )
			GenErr( "CRegionClient::LoadSceneTrap Load Invalid Grid Index!" );

		uint8 uTrap;
		File.read(&uTrap, sizeof(uint8));
		pRgn->SetTrap( uIndex, uTrap );
	}
}

void CSceneArgCoder::LoadRegionBlock( CPkgFile& File, uint32 uRegionId )
{
	if(m_pScene->m_pMapPath)
		m_pScene->m_pMapPath->LoadRegionData(File, uRegionId%m_pScene->m_uWidthInRegion, uRegionId/m_pScene->m_uWidthInRegion );
}

void CSceneArgCoder::LoadRegionExactBlock( CPkgFile& File, uint32 uRegionId )
{
	if( m_pScene->m_pMapPath )
		m_pScene->m_pMapPath->LoadRgnExactBlockData(File, uRegionId%m_pScene->m_uWidthInRegion, uRegionId/m_pScene->m_uWidthInRegion );
}

void CSceneArgCoder::LoadRegionSceneAudio( CPkgFile& File, uint32 uRegionId )
{
	uint8   uAudioNum = 0;
	float   cX;
	float   cY;
	float   cZ;
	uint16  SetIndex = 0;
	uint8   GridIndex = 0;

	File.read( &uAudioNum, sizeof(uAudioNum) );
	CMetaRegion* pRegion = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	pRegion->GetAudioInfo().resize( uAudioNum );

	for( uint8 i = 0; i < uAudioNum; ++i )
	{
		CRegionAI &audioInfo = pRegion->GetAudioInfo()[i];
		uint8 Version = 0;

		File.read( &Version,		sizeof(Version) );

		switch (Version)
		{
		case AUDIO_PLAY_LEN:

			File.read( &audioInfo.bPlayNotRepeat, sizeof(bool) );
			if( audioInfo.bPlayNotRepeat )
			{
				File.read( &audioInfo.uPlayTimeLenMin,  sizeof(uint32) );
				File.read( &audioInfo.uPlayTimeLenMax,  sizeof(uint32) );
				File.read( &audioInfo.uPlayIntervalMin, sizeof(uint32) );
				File.read( &audioInfo.uPlayIntervalMax, sizeof(uint32) );
			}

		case AUDIO_ORIGIN:

			File.read( &audioInfo.cX,		  sizeof(cX) );
			File.read( &audioInfo.cY,		  sizeof(cY) );
			File.read( &audioInfo.cZ,		  sizeof(cZ) );
			File.read( &audioInfo.SetIndex,	  sizeof(SetIndex) );
			File.read( &audioInfo.GridIndex,  sizeof(GridIndex) );

			break;
		default:
			break;
		}
	}
}

void CSceneArgCoder::LoadRegionLoginModel( CPkgFile& File, uint32 uRegionId )
{
	uint16 uLoginModelNum = 0;
	File.read( &uLoginModelNum, sizeof(uint16) );

	if( uLoginModelNum == 0)
		return;

	CMetaRegion* pRegion = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	pRegion->GetLoginModelInfo().resize( uLoginModelNum );
	
	SString strLogicName = "";
	DWORD  dwLen = 0;
	SString strAniName;

	for( uint16 i = 0; i < uLoginModelNum; ++i )
	{
		CRegionLoginI &loginInfo = pRegion->GetLoginModelInfo()[i];

		switch (m_uModelVersion)
		{
		case DEFAULT_OLDMODELPRO:
			{
				CRegionOldMI sRegOldModelInfo;

				File.read( &sRegOldModelInfo, sizeof(CRegionOldMI) );

				CRegionMI &RegModelInfo = loginInfo.sMoldeInfo;

				RegModelInfo.cX = sRegOldModelInfo.cX;
				RegModelInfo.cY = sRegOldModelInfo.cY;
				RegModelInfo.cZ = sRegOldModelInfo.cZ;

				RegModelInfo.SetIndex = sRegOldModelInfo.SetIndex;
				RegModelInfo.GridIndex = sRegOldModelInfo.GridIndex;

				RegModelInfo.fyRotation = -sRegOldModelInfo.byRotation * ROTATION_STEP;//y
				RegModelInfo.fxRotation = -sRegOldModelInfo.bxRotation * ROTATION_STEP;//x
				RegModelInfo.fzRotation = -sRegOldModelInfo.bzRotation * ROTATION_STEP;//z

				RegModelInfo.fScaleX = sRegOldModelInfo.fScaleX;
				RegModelInfo.fScaleY = sRegOldModelInfo.fScaleY;
				RegModelInfo.fScaleZ = sRegOldModelInfo.fScaleY;

				RegModelInfo.nFlag = sRegOldModelInfo.nFlag;

				RegModelInfo.r = sRegOldModelInfo.r;
				RegModelInfo.g = sRegOldModelInfo.g;
				RegModelInfo.b = sRegOldModelInfo.b;

				RegModelInfo.bEffectByConfigur = false;
				RegModelInfo.bCameraCollision  = false;
			}
			break;

		case DEFAULT_NEWMODELPRO:
			{
				File.read( &loginInfo.sMoldeInfo, sizeof(CRegionMI) );
			}
			break;

		default:
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		File.read( &dwLen, sizeof(dwLen) );
		strLogicName.resize(dwLen);

		/// logic name
		File.read( &strLogicName[0], strLogicName.size());

		loginInfo.sModelLogicName = strLogicName;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///frame SList
		//SMap<int, SMap<int16, SString>>  CameraAniFrameMap;
		CameraAniFrameMap allAniLoginFrameList;
		size_t allAniSize = 0, aniSize = 0, frameSize = 0;
		int16  frm = 0;
		int	   nCameraPathIndex = 0;

		File.read( &allAniSize, sizeof(allAniSize) );
		for( size_t n = 0; n < allAniSize; ++n )
		{
			SMap<int16, SString>   aniFrameList;
			File.read( &nCameraPathIndex, sizeof(nCameraPathIndex) );

			File.read( &aniSize, sizeof(aniSize) );
			for ( size_t m = 0; m < aniSize;  ++m )
			{
				File.read( &dwLen, sizeof(dwLen) );
				strAniName.resize(dwLen);

				File.read( &strAniName[0], strAniName.size());

				File.read( &frameSize, sizeof(frameSize) );
				for( size_t n = 0; n < frameSize; ++n )
				{
					File.read( &frm, sizeof(frm) );
					aniFrameList[frm] = strAniName;
				}
			}

			allAniLoginFrameList[nCameraPathIndex] = aniFrameList;
		}

		loginInfo.loginAniCamAniMap = allAniLoginFrameList;
		//////////////////////////////////////////////////////////////////////////
	}
}

void CSceneArgCoder::LoadRegionMaterialIndex( CPkgFile& File, uint32 uRegionId )
{
	CMetaRegion* pRgn = m_pScene->m_pGridMgr->GetRegion( uRegionId );
	int8  nMaterialIndex = -1;

	for( int32 i = 0; i < eGridNumPerRegion; ++i )
	{
		File.read(&nMaterialIndex, sizeof(nMaterialIndex));
		pRgn->SetTerrainMaterialIndex( i, nMaterialIndex );
	}
}
