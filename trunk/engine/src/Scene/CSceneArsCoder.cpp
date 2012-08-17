#include "stdafx.h"
#include "CGraphic.h"
#include "CSceneArsCoder.h"
#include "CSceneManagerClient.h"
#include "CWaterMgr.h"
#include "TSqrAllocator.inl"
#include "CPkgFile.h"

BEGIN_VERSION_MAP(CSceneArsCoder)
	ON_VERSION( 0x0000000B , DeCode_Ver_C )
	ON_VERSION( 0x0000000C , DeCode_Ver_C )
	ON_VERSION( 0x0000000D , DeCode_Ver_D )
	ON_VERSION( 0x0000000E , DeCode_Ver_E )
END_VERSION_MAP()

bool CSceneArsCoder::SceneDeCode( CDataScene* pSceneManager,CPkgFile& File )
{
	uint32 nVersion;
	bool isDeCode = false;
	File.Read(&nVersion, sizeof(int32));
	CDataChunk TmpChunk((char*)&File, File.Size() - File.Tell());
	((CSceneManagerClient*)pSceneManager)->m_nVersion = nVersion;		
	//SetDeCodeVersion(nVersion);
	TmpChunk.SetVersion(nVersion);
	isDeCode = DeCode(TmpChunk,pSceneManager);
	return isDeCode;
}

bool CSceneArsCoder::DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	int nSurfaceNum;
	File.read( &nSurfaceNum, sizeof(int));
	pScene->GetWaterMgr()->m_TextUnit.resize( nSurfaceNum + 1 );

	for( int i = 0; i < nSurfaceNum; ++i )
	{
		int nTextLen;
		File.read( &nTextLen, sizeof(int));
		string Buf(nTextLen, 0);
		File.read( &Buf[0], nTextLen);
		pScene->GetWaterMgr()->m_TextUnit[i].SetTextName( Buf.c_str() );
	}
	pScene->GetWaterMgr()->m_TextUnit[nSurfaceNum].SetTextName( pScene->GetWaterMgr()->m_WaveInfo.szWaveTexture.c_str() );

	int nSetUnitNum;
	File.read( &nSetUnitNum, sizeof(int));
	pScene->m_SetUnit.resize( nSetUnitNum );

	for( int i = 0; i < nSetUnitNum; i++ )
	{
		bool bARA = false;
		uint8 nUnitPartNum;
		uint8 nRSIndex;
		File.read( &nRSIndex, sizeof(uint8));
		File.read( &nUnitPartNum, sizeof(uint8));

		CREATE_ITEM AnimationItem;
		SVector<CREATE_ITEM> EffectItem;

		for( uint8 j = 0; j < nUnitPartNum; j++ )
		{
			int Len;
			CREATE_ITEM CreateItem;

			File.read( &CreateItem.dwResType, sizeof(uint32));
			File.read( &Len, sizeof(int) );
			string Buf(Len, 0);
			File.read( &Buf[0], Len);
			CreateItem.szFileName = Buf.c_str();

			switch( CreateItem.dwResType )
			{
			case MAKEFOURCC( 'A', 'R', 'P', 0 ):
				CreateItem.szClassName = "";
				CreateItem.szParam.assign( (char*)&nRSIndex, sizeof( nRSIndex ) );
				pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );
				pScene->m_SetUnit[i].m_UnitPart[ pScene->m_SetUnit[i].m_UnitPart.size() - 1 ].Items.push_back( CreateItem );
				break;
			case MAKEFOURCC( 'A', 'R', 'A', 0 ):
				CreateItem.szClassName = "RFT_ARA";
				bARA = true;
				AnimationItem = CreateItem;
				break;
			case MAKEFOURCC( 'A', 'R', 'E', 0 ):
				CreateItem.szClassName = "RFT_ARE";
				File.read( &Len, sizeof(int));
				Buf.resize(Len, 0);
				File.read( &Buf[0], Buf.size());
				CreateItem.szParam = Buf.c_str();
				EffectItem.push_back( CreateItem );
				break;
			default:
				break;
			}
		}

		if( pScene->m_SetUnit[i].m_UnitPart.size() == 0 )
			pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );

		if( bARA )
		{
			for( size_t nArp = 0; nArp < pScene->m_SetUnit[i].m_UnitPart.size(); ++nArp )
				pScene->m_SetUnit[i].m_UnitPart[nArp].Items.push_back( AnimationItem );
		}

		for( size_t nFx = 0; nFx < EffectItem.size(); ++nFx )
			pScene->m_SetUnit[i].m_UnitPart[0].Items.push_back( EffectItem[nFx] );
	}
	return true;
}

bool CSceneArsCoder::DeCode_Ver_D( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	int nSurfaceNum;
	File.read( &nSurfaceNum, sizeof(int));
	pScene->GetWaterMgr()->m_TextUnit.resize( nSurfaceNum + 1 );

	for( int i = 0; i < nSurfaceNum; ++i )
	{
		int nTextLen;
		File.read( &nTextLen, sizeof(int));
		string Buf(nTextLen, 0);
		File.read( &Buf[0], nTextLen);
		pScene->GetWaterMgr()->m_TextUnit[i].SetTextName( Buf.c_str() );
	}
	pScene->GetWaterMgr()->m_TextUnit[nSurfaceNum].SetTextName( pScene->GetWaterMgr()->m_WaveInfo.szWaveTexture.c_str() );

	int nSetUnitNum = 0;
	File.read( &nSetUnitNum, sizeof(int));
	pScene->m_SetUnit.resize( nSetUnitNum );

	for( int i = 0; i < nSetUnitNum; ++i )
	{
		bool bARA = false;
		uint8 nUnitPartNum;
		uint8 nRSIndex;
		File.read( &nRSIndex, sizeof(uint8));
		File.read( &nUnitPartNum, sizeof(uint8));

		CREATE_ITEM AnimationItem;
		SVector<CREATE_ITEM> EffectItem;

		for( uint8 j = 0; j < nUnitPartNum; ++j )
		{
			int Len;
			CREATE_ITEM CreateItem;

			File.read( &CreateItem.dwResType, sizeof(uint32));
			File.read( &Len, sizeof(int) );
			string Buf(Len, 0);
			File.read( &Buf[0], Len);
			CreateItem.szFileName = Buf.c_str();

			switch( CreateItem.dwResType )
			{
			case MAKEFOURCC( 'A', 'R', 'P', 0 ):
				CreateItem.szClassName = "";
				CreateItem.szParam.assign( (char*)&nRSIndex, sizeof( nRSIndex ) );
				pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );
				pScene->m_SetUnit[i].m_UnitPart[ pScene->m_SetUnit[i].m_UnitPart.size() - 1 ].Items.push_back( CreateItem );
				break;
			case MAKEFOURCC( 'A', 'R', 'A', 0 ):
				CreateItem.szClassName = "RFT_ARA";
				bARA = true;
				AnimationItem = CreateItem;
				break;
			case MAKEFOURCC( 'A', 'R', 'E', 0 ):
				CreateItem.szClassName = "RFT_ARE";
				File.read( &Len, sizeof(int));
				Buf.resize(Len, 0);
				File.read( &Buf[0], Buf.size());
				CreateItem.szParam = Buf.c_str();
				EffectItem.push_back( CreateItem );
				break;
			default:
				break;
			}
		}

		if( pScene->m_SetUnit[i].m_UnitPart.size() == 0 )
			pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );

		if( bARA )
		{
			size_t size = pScene->m_SetUnit[i].m_UnitPart.size();
			for( size_t nArp = 0; nArp < size; ++nArp )
				pScene->m_SetUnit[i].m_UnitPart[nArp].Items.push_back( AnimationItem );
		}

		for( size_t nFx = 0; nFx < EffectItem.size(); ++nFx )
			pScene->m_SetUnit[i].m_UnitPart[0].Items.push_back( EffectItem[nFx] );
	}

	int nAudioUnitNum = 0;
	File.read( &nAudioUnitNum, sizeof(nAudioUnitNum));
	pScene->m_AudioUnit.resize( nAudioUnitNum );

	for( int i = 0; i < nAudioUnitNum; ++i )
	{
		int   nLen = 0;

		File.read( &nLen, sizeof(int) );
		SString Buf(nLen, 0);
		File.read( &Buf[0], Buf.size());
		pScene->m_AudioUnit[i] = Buf.c_str();
	}

	return true;
}

bool CSceneArsCoder::DeCode_Ver_E( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	int nSurfaceNum;
	File.read( &nSurfaceNum, sizeof(int));
	pScene->GetWaterMgr()->m_TextUnit.resize( nSurfaceNum + 1 );

	for( int i = 0; i < nSurfaceNum; ++i )
	{
		int nTextLen;
		File.read( &nTextLen, sizeof(int));
		string Buf(nTextLen, 0);
		File.read( &Buf[0], Buf.size());
		pScene->GetWaterMgr()->m_TextUnit[i].SetTextName( Buf.c_str() );
	}
	pScene->GetWaterMgr()->m_TextUnit[nSurfaceNum].SetTextName( pScene->GetWaterMgr()->m_WaveInfo.szWaveTexture.c_str() );

	int nSetUnitNum = 0;
	File.read( &nSetUnitNum, sizeof(int));
	pScene->m_SetUnit.resize( nSetUnitNum );

	for( int i = 0; i < nSetUnitNum; ++i )
	{
		bool bAgp = false, bSke = false;
		uint8 nUnitPartNum;
		uint8 nRSIndex;
		File.read( &nRSIndex, sizeof(uint8));
		File.read( &nUnitPartNum, sizeof(uint8));

		CREATE_ITEM AnimationGroupItem, AnimationSkeItem;
		SVector<CREATE_ITEM> EffectItem;

		for( uint8 j = 0; j < nUnitPartNum; ++j )
		{
			int Len;
			CREATE_ITEM CreateItem;

			File.read( &CreateItem.dwResType, sizeof(uint32));
			File.read( &Len, sizeof(int) );
			string Buf(Len, 0);
			File.read( &Buf[0], Buf.size());
			CreateItem.szFileName = Buf.c_str();

			switch( CreateItem.dwResType )
			{
			case MAKEFOURCC( 'A', 'R', 'P', 0 ):
				CreateItem.szClassName = "";
				CreateItem.szParam.assign( (char*)&nRSIndex, sizeof( nRSIndex ) );
				pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );
				pScene->m_SetUnit[i].m_UnitPart[ pScene->m_SetUnit[i].m_UnitPart.size() - 1 ].Items.push_back( CreateItem );
				break;
			
			case MAKEFOURCC( 'A', 'G', 'P', 0 ):
				CreateItem.szClassName = "RFT_AGP";
				bAgp = true;
				AnimationGroupItem = CreateItem;
				break;

			case MAKEFOURCC( 'S', 'K', 'E', 0 ):
				CreateItem.szClassName = "RFT_SKE";
				bSke = true;
				AnimationSkeItem = CreateItem;
				break;
			
			case MAKEFOURCC( 'A', 'R', 'E', 0 ):
				CreateItem.szClassName = "RFT_ARE";
				File.read( &Len, sizeof(int));
				Buf.resize(Len, 0);
				File.read( &Buf[0], Buf.size());
				CreateItem.szParam = Buf.c_str();
				EffectItem.push_back( CreateItem );
				break;
			
			default:
				break;
			}
		}

		if( pScene->m_SetUnit[i].m_UnitPart.size() == 0 )
			pScene->m_SetUnit[i].m_UnitPart.push_back( OBJECT_CREATE_PARAM() );

		if( bAgp && bSke )
		{
			size_t size = pScene->m_SetUnit[i].m_UnitPart.size();
			for( size_t nArp = 0; nArp < size; ++nArp )
			{
				pScene->m_SetUnit[i].m_UnitPart[nArp].Items.push_back( AnimationGroupItem );
				pScene->m_SetUnit[i].m_UnitPart[nArp].Items.push_back( AnimationSkeItem );
			}
		}

		for( size_t nFx = 0; nFx < EffectItem.size(); ++nFx )
			pScene->m_SetUnit[i].m_UnitPart[0].Items.push_back( EffectItem[nFx] );
	}

	int nAudioUnitNum = 0;
	File.read( &nAudioUnitNum, sizeof(nAudioUnitNum));
	pScene->m_AudioUnit.resize( nAudioUnitNum );

	for( int i = 0; i < nAudioUnitNum; ++i )
	{
		int   nLen = 0;

		File.read( &nLen, sizeof(int) );
		string Buf(nLen, 0);
		File.read( &Buf[0], Buf.size());
		pScene->m_AudioUnit[i] = Buf.c_str();
	}

	return true;
}
