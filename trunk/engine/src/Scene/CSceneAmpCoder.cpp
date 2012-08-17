#include "stdafx.h"
#include "CSceneAmpCoder.h"
#include "CSceneManagerClient.h"
#include "CGraphic.h"
#include "CSceneConfig.h"
#include "BaseHelper.h"
#include "CCameraPath.h"
#include "CRenderGroup.h"
#include "CBloomTarget.h"
#include "CSkyMgr.h"
#include "CWaterMgr.h"
#include "CMetaGridMgr.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------


BEGIN_VERSION_MAP(CSceneAmpCoder)
	ON_VERSION( 0x0000000B , DeCode_Ver_B )
	ON_VERSION( 0x0000000C , DeCode_Ver_C )
	ON_VERSION( 0x0000000D , DeCode_Ver_D )
	ON_VERSION( 0x0000000E , DeCode_Ver_E )
	ON_VERSION( 0x0000000F , DeCode_Ver_F )
	ON_VERSION( 0x00000010 , DeCode_Ver_G )
	ON_VERSION( 0x00000011 , DeCode_Ver_H )
END_VERSION_MAP()

bool CSceneAmpCoder::SceneDeCode( CDataScene* pSceneManager, CPkgFile& File )
{
	uint32 nMagicID,nVersion;
	bool isDeCode = false;
	File.Read(&nMagicID, sizeof(int32));
	File.Read(&nVersion, sizeof(int32));
	
	if( nMagicID == 'PAMA' )
	{
		CDataChunk TmpChunk((char*)&File, File.Size() - File.Tell());
		((CSceneManagerClient*)pSceneManager)->m_nVersion = nVersion;		
		TmpChunk.SetVersion(nVersion);
		isDeCode = DeCode(TmpChunk,pSceneManager);
	}
	return isDeCode;
}

void CSceneAmpCoder::InitScene(CSceneManagerClient* pScene)
{
	pScene->m_uWidth = pScene->m_uWidthInRegion / 4;
	pScene->m_uHeight = pScene->m_uHeightInRegion / 4;


	pScene->m_uWidthInGrid		= pScene->m_uWidthInRegion  * eRegionSpan;
	pScene->m_uHeightInGrid		= pScene->m_uHeightInRegion * eRegionSpan;

	pScene->m_uWidthInPixel		= pScene->m_uGridWidth * pScene->m_uWidthInGrid;
	pScene->m_uHeightInPixel	= pScene->m_uGridWidth * pScene->m_uHeightInGrid;

	pScene->m_uRegionNum		= pScene->m_uWidthInRegion * pScene->m_uHeightInRegion;
	pScene->m_uWidthInSubRegion = pScene->m_uWidthInRegion * eRegDivideNum;
	pScene->m_uSubRegionNum		= pScene->m_uRegionNum * eSubRegNum;

	//pScene->m_CameraInfo.fRotateX *= CMath::pi /180;
	//pScene->m_CameraInfo.fRotateY *= CMath::pi /180;
	//pScene->m_CameraInfo.fFov *= CMath::pi /180;
}

bool CSceneAmpCoder::DeCoder_Com_B_D( CPkgFile& File,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	File.Read(pScene->m_szDesc, 64);

	// width depth in region
	uint16 u16tmp;
	File.Read(&u16tmp, sizeof(uint16));
	pScene->m_uWidthInRegion = u16tmp;
	File.Read(&u16tmp, sizeof(uint16));
	pScene->m_uHeightInRegion = u16tmp;

	// REGION_SPACE
	//File.read(&m_uRegionWidth, sizeof(uint8));
	uint8 u8tmp;
	File.Read(&u8tmp, sizeof(uint8));

	// GRID_SPACE
	File.Read(&pScene->m_uGridWidth, sizeof(uint8));

	// Tileset
	File.Read(pScene->m_szTileSet, 64);

	bool Cave;
	File.Read(&Cave,sizeof(bool));

	//////////////////////////////////////////////////////////////////////////
	// 随时间变化信息光参值
	File.Read(&pScene->m_SceneInfo, sizeof( pScene->m_SceneInfo ));
	//////////////////////////////////////////////////////////////////////////

	char szBackTexName[64];
	//szBackTexName.resize(64);
	File.Read(szBackTexName, 64);
	CSkyMgr* pSkyMgr = pScene->GetSkyMgr();
	pSkyMgr->SetSkyBoxTextureName(szBackTexName);
	

	// Wave
	int32 n = 0;
	char szBuf[256];
	File.Read(&pScene->GetWaterMgr()->m_WaveInfo.WavePhysicInfo, sizeof(CWaterWaveInfo::WaterPhysic));
	File.Read(&n,sizeof(int32));
	//Ast(n < 256);
	File.Read(szBuf, n);
	szBuf[n] = 0;
	pScene->GetWaterMgr()->m_WaveInfo.szWaveTexture = szBuf;
	File.Read(&n,sizeof(int32));
	File.Read(szBuf, n);
	szBuf[n] = 0;
	pScene->GetWaterMgr()->m_WaveInfo.szMoveFxFile = szBuf;
	File.Read(&n,sizeof(int32));
	File.Read(szBuf, n);
	szBuf[n] = 0;
	pScene->GetWaterMgr()->m_WaveInfo.szMoveFxName = szBuf;
	File.Read(&n,sizeof(int32));
	File.Read(szBuf, n);
	szBuf[n] = 0;
	pScene->GetWaterMgr()->m_WaveInfo.szStandFxFile = szBuf;
	File.Read(&n,sizeof(int32));
	File.Read(szBuf, n);
	szBuf[n] = 0;
	pScene->GetWaterMgr()->m_WaveInfo.szStandFxName = szBuf;

	// background music
	char	szMName[64];// 背景音乐的名字
	int		nMusicInterval;

	uint32 dwFileNameLen;
	File.Read(&dwFileNameLen,sizeof(uint32));
	File.Read(szMName,dwFileNameLen);
	File.Read(&nMusicInterval,sizeof(int));

	// Ambient Effect
	int32 nAmbientFXCnt = 0, nAmbientFXSetLength = 0, nProbability = 0; 
	File.Read(&nAmbientFXCnt,sizeof(int32));
	File.Read(&nAmbientFXSetLength,sizeof(int32));
	if( nAmbientFXSetLength )
	{
		File.Read(szBuf, nAmbientFXSetLength);
		szBuf[nAmbientFXSetLength] = 0;

		for( int i = 0; i < nAmbientFXCnt; i++ )
		{
			pScene->m_AmbientFxInfo.Fx.push_back( CAmbientFxInfo::FX() );

			///effect name
			File.Read(&n,sizeof(int32));
			File.Read(szBuf, sizeof(char)*n);
			pScene->m_AmbientFxInfo.Fx[i].szFxName = szBuf;

			///effect file name
			File.Read(&n,sizeof(int32));
			File.Read(szBuf, sizeof(char)*n);
			pScene->m_AmbientFxInfo.Fx[i].szFxFileName = szBuf;

			///概率
			File.Read(&nProbability, sizeof(int32));
			pScene->m_AmbientFxInfo.Fx[i].nProbability = nProbability;
		}
	}

	// Camera Info
	float CamerInfo;
	File.Read(&CamerInfo, sizeof(float));	//RotateX
	File.Read(&CamerInfo, sizeof(float));	//RotateY
	File.Read(&CamerInfo, sizeof(float));	//RotateF
	File.Read(&CamerInfo, sizeof(float));	//RotateB

	//m_CameraInfo.fRotateX=20.0f;
	//m_CameraInfo.fRotateY=-45.0f;
	//m_CameraInfo.fFov=6.18f;
	//m_CameraInfo.fBack=9000.0f;

	///背景图移动速度信息
	//pScene->m_bSetBackPic = false;
	bool bSetBackPic;
	File.Read(&bSetBackPic, sizeof(bool));
	pSkyMgr->SetMoveEnable(bSetBackPic);
	if( bSetBackPic)
	{
		float speed;
		File.Read(&speed, sizeof(float));
		pSkyMgr->SetMoveLeftSpeed(speed);
		File.Read(&speed, sizeof(float));
		pSkyMgr->SetMoveRightSpeed(speed);
		File.Read(&speed, sizeof(float));
		pSkyMgr->SetMoveTopSpeed(speed);
		File.Read(&speed, sizeof(float));
		pSkyMgr->SetMoveBottomSpeed(speed);
	}

	// LoadHDR
	float	fBrightPass; //光通量 0~1
	float	fGaussMultiplier; //泛光强度 0~2
	float	fGaussMean; //泛光偏移 -1~+1
	float	fGaussStdDev; //泛光融和程度  0~1
	float	fExposure;	//最终亮度0~2
	float	fSample;			//混合度	

	File.read(&fBrightPass, sizeof(float));
	File.read(&fGaussMultiplier, sizeof(float));
	File.read(&fGaussMean, sizeof(float));
	File.read(&fGaussStdDev, sizeof(float));
	File.read(&fExposure, sizeof(float));
	File.read(&fSample, sizeof(float));

	CBloomTarget* pTarget = CGraphic::GetInst()->GetActiveRenderTarget()->GetTargetPipeline();
	if( pTarget )
	{
		pTarget->setBrightPass(fBrightPass);
		pTarget->setGaussMultiplier(fGaussMultiplier);
		pTarget->setMean(fGaussMean);
		pTarget->setStdDev(fGaussStdDev);
		pTarget->setExposure(fExposure);
		pTarget->setSample(fSample);
		pTarget->InitParams();
		pTarget->SetTurnOn("Bloom", CSceneConfig::GetInst()->IsBloomEnabled());
		pTarget->SetTurnOn("Refract", CSceneConfig::GetInst()->IsRefractEnabled());
	}

	InitScene(pScene);
	return true;
}

bool CSceneAmpCoder::DeCode_Ver_B( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	//---------------------------------------------------------------------------------------
	size_t size = 0, ajenctsize= 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		CColor color;
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = NULL;
			pRgnInfo = new RegionsInfo;

			if( pRgnInfo )
			{
				///scene
				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.globalAmbient = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.globalDiffuse = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.fogColor = color;

				///player
				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.playerAmbient = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.playerDiffuse = color;

				pRgnInfo->lightParam.fogStart = pScene->m_SceneInfo[2].fFogStart;
				pRgnInfo->lightParam.fogEnd   = pScene->m_SceneInfo[2].fFogEnd;

				pScene->m_RgnsList.push_back(pRgnInfo);
			}
		}
	}	
	return true;
}

bool CSceneAmpCoder::DeCode_Ver_C( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	size_t size = 0, ajenctsize= 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		CColor color;
		uint8  uFogMode = 3;
		float  fFogStart = 0.0f, fFogEnd = 0.0f, fFogDendsity = 0.0;

		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = NULL;
			pRgnInfo = new RegionsInfo;

			if( pRgnInfo )
			{
				///scene
				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.globalAmbient = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.globalDiffuse = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.fogColor = color;

				///player
				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.playerAmbient = color;

				File.read(&color.R, sizeof(uint8));
				File.read(&color.G, sizeof(uint8));
				File.read(&color.B, sizeof(uint8));
				pRgnInfo->lightParam.playerDiffuse = color;

				File.read(&uFogMode,	 sizeof(uFogMode));
				pRgnInfo->uFogMode     = uFogMode;

				File.read(&fFogStart,	 sizeof(fFogStart));
				pRgnInfo->lightParam.fogStart	   = fFogStart;

				File.read(&fFogEnd,		 sizeof(fFogEnd));
				pRgnInfo->lightParam.fogEnd	   = fFogEnd;

				File.read(&fFogDendsity, sizeof(fFogDendsity));
				pRgnInfo->lightParam.fogDensity  = fFogDendsity;

				pScene->m_RgnsList.push_back(pRgnInfo);
			}
		}
	}

	return true;
}

bool CSceneAmpCoder::DeCode_Ver_D( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	size_t size = 0, ajenctsize= 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		locale loc = locale::global(locale(""));
		uint32 dwNameLen = 0;
		char szBuf[256];
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = new RegionsInfo;

			if( !pRgnInfo )
				return false;
			///scene
			File.read(&pRgnInfo->lightParam.globalAmbient.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.B, sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalDiffuse.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.B, sizeof(uint8));

#ifndef	AMBIENT_SHADOW_COLOR	
			File.read(&pRgnInfo->lightParam.globalShadow.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.B, sizeof(uint8));
#else
			uint8 dummy;
			File.read(&dummy, sizeof(uint8));
			File.read(&dummy, sizeof(uint8));
			File.read(&dummy, sizeof(uint8));
#endif

			File.read(&pRgnInfo->lightParam.fogColor.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.B, sizeof(uint8));

			///player
			File.read(&pRgnInfo->lightParam.playerAmbient.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.B, sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerDiffuse.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.B, sizeof(uint8));

			File.read(&pRgnInfo->uFogMode,	 sizeof(pRgnInfo->uFogMode));
			File.read(&pRgnInfo->lightParam.fogStart,	 sizeof(float));
			File.read(&pRgnInfo->lightParam.fogEnd,		 sizeof(float));
			File.read(&pRgnInfo->lightParam.fogDensity, sizeof(float));

			///music name
			File.Read(&dwNameLen,sizeof(dwNameLen));
			File.Read(szBuf, sizeof(char)*dwNameLen);
			pRgnInfo->strMusicName = szBuf;

			pScene->m_RgnsList.push_back(pRgnInfo);
		}

		locale::global(loc);//恢复全局locale	
	}

	return true;
}

bool sqr::CSceneAmpCoder::DeCode_Ver_E( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	size_t size = 0, ajenctsize= 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		locale loc = locale::global(locale(""));
		uint32 dwNameLen = 0;
		char szBuf[256];
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = new RegionsInfo;

			if( !pRgnInfo )
				return false;
			///scene
			File.read(&pRgnInfo->lightParam.globalAmbient.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.B, sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalDiffuse.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.B, sizeof(uint8));

#ifndef AMBIENT_SHADOW_COLOR
			File.read(&pRgnInfo->lightParam.globalShadow.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.B, sizeof(uint8));
#else
			uint8 dummy;
			File.read(&dummy, sizeof(uint8));
			File.read(&dummy, sizeof(uint8));
			File.read(&dummy, sizeof(uint8));
#endif

			File.read(&pRgnInfo->lightParam.fogColor.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.B, sizeof(uint8));

			///player
			File.read(&pRgnInfo->lightParam.playerAmbient.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.B, sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerDiffuse.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.B, sizeof(uint8));

			File.read(&pRgnInfo->uFogMode,	 sizeof(pRgnInfo->uFogMode));
			File.read(&pRgnInfo->lightParam.fogStart,	 sizeof(float));
			File.read(&pRgnInfo->lightParam.fogEnd,		 sizeof(float));
			File.read(&pRgnInfo->lightParam.fogDensity, sizeof(float));

			///music name
			File.Read(&dwNameLen,sizeof(dwNameLen));
			File.Read(szBuf, sizeof(char)*dwNameLen);
			pRgnInfo->strMusicName = szBuf;

			pScene->m_RgnsList.push_back(pRgnInfo);
		}

		// ----------------------------------------------------------------------------
		// 读取摄像机轨迹动画信息
		
		uint32 uAnimCount = 0;
		File.read( &uAnimCount, sizeof(uint32) );	
		pScene->m_vecCamPath.resize(uAnimCount, NULL);

		for ( uint i=0; i<uAnimCount; ++i )
		{
			int		pc		= 0;
			File.read( &pc, sizeof(int) );
			if ( pc != NULL )
			{
				CCameraPath	*pNewPath	= new CCameraPath;
				pScene->m_vecCamPath[i] = pNewPath; 

				uint32 uKeyCount = 0;
				File.read( &uKeyCount, sizeof(uint32) );	

				CVector3f	data;
				float		time;
				for ( uint32 i=0; i<uKeyCount; ++i )
				{
					// 读取 摄像机位置帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddPositionKey( data, time );

					// 读取 摄像机观察点帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddTargetkey( data, time );

					// 读取 摄像机视角帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddFovKey( data, time );
				}
			}
		}

		locale::global(loc);//恢复全局locale	
	}

	return true;
}

bool sqr::CSceneAmpCoder::DeCode_Ver_F( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	//////////////////////////////////////////////////////////////////////////
	//read specular parameter 无效数据
	uint32 uColor = 0; 
	float  fPower = 0.0f;

	File.Read(&uColor, sizeof(uColor));
	File.Read(&uColor, sizeof(uColor));
	File.Read(&fPower, sizeof(fPower));
	File.Read(&fPower, sizeof(fPower));
	//////////////////////////////////////////////////////////////////////////

	size_t size = 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		locale loc = locale::global(locale(""));
		uint32 dwNameLen = 0;
		char szBuf[256];
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = new RegionsInfo;

			if( !pRgnInfo )
				return false;
			///scene
			File.read(&pRgnInfo->lightParam.globalAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.B, sizeof(uint8));

#ifndef AMBIENT_SHADOW_COLOR
			File.read(&pRgnInfo->lightParam.globalShadow.R,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.G,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.B,   sizeof(uint8));
#else
			uint8 dummy;
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
#endif

			File.read(&pRgnInfo->lightParam.fogColor.R,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.G,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.B,		  sizeof(uint8));

			///player
			File.read(&pRgnInfo->lightParam.playerAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.B, sizeof(uint8));

			File.read(&pRgnInfo->uFogMode,					  sizeof(pRgnInfo->uFogMode));
			File.read(&pRgnInfo->lightParam.fogStart,		  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogEnd,			  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogDensity,       sizeof(float));

			///music name
			File.Read(&dwNameLen,sizeof(dwNameLen));
			File.Read(szBuf, sizeof(char)*dwNameLen);
			pRgnInfo->strMusicName = szBuf;

			pScene->m_RgnsList.push_back(pRgnInfo);
		}

		// ----------------------------------------------------------------------------
		// 读取摄像机轨迹动画信息

		uint32 uAnimCount = 0;
		File.read( &uAnimCount, sizeof(uint32) );	
		pScene->m_vecCamPath.resize(uAnimCount, NULL);

		for ( uint i=0; i<uAnimCount; ++i )
		{
			int		pc		= 0;
			File.read( &pc, sizeof(int) );
			if ( pc != NULL )
			{
				CCameraPath	*pNewPath	= new CCameraPath;
				pScene->m_vecCamPath[i] = pNewPath; 

				uint32 uKeyCount = 0;
				File.read( &uKeyCount, sizeof(uint32) );	

				CVector3f	data;
				float		time;
				for ( uint32 i=0; i<uKeyCount; ++i )
				{
					// 读取 摄像机位置帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddPositionKey( data, time );

					// 读取 摄像机观察点帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddTargetkey( data, time );

					// 读取 摄像机视角帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddFovKey( data, time );
				}
			}
		}

		locale::global(loc);//恢复全局locale	
	}

	return true;
}

bool sqr::CSceneAmpCoder::DeCode_Ver_G( const CDataChunk& inData, OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	//////////////////////////////////////////////////////////////////////////
	//read specular parameter 无效数据
	uint32 uColor = 0; 
	float  fPower = 0.0f;

	File.Read(&uColor, sizeof(uColor));
	File.Read(&uColor, sizeof(uColor));
	File.Read(&fPower, sizeof(fPower));
	File.Read(&fPower, sizeof(fPower));
	//////////////////////////////////////////////////////////////////////////

	locale loc = locale::global(locale(""));
	uint32 dwNameLen = 0;
	char szBuf[256];
	//////////////////////////////////////////////////////////////////////////
	//region ambient fx name
	SVector<SString>  ambientFxNameList;
	size_t size = 0;
	File.read(&size, sizeof(size_t));
	for ( size_t i = 0; i < size; ++i )
	{
		SString strEffectName = "";

// 		File.Read(&dwNameLen,sizeof(dwNameLen));
// 		File.Read(szBuf, sizeof(char)*dwNameLen);
// 		szBuf[dwNameLen] = 0;
// 		strEffectFileName = szBuf;

		File.Read(&dwNameLen,sizeof(dwNameLen));
		File.Read(szBuf, sizeof(char)*dwNameLen);
		szBuf[dwNameLen] = 0;
		strEffectName = szBuf;

//		strEffectFileName = "fx\\setting\\mapfx_ambient_yb\\" + strEffectName + ".efx";

		ambientFxNameList.push_back(strEffectName);
	}
	//////////////////////////////////////////////////////////////////////////

	size = 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = new RegionsInfo;

			if( !pRgnInfo )
				return false;
			///scene
			File.read(&pRgnInfo->lightParam.globalAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.B, sizeof(uint8));

#ifndef AMBIENT_SHADOW_COLOR
			File.read(&pRgnInfo->lightParam.globalShadow.R,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.G,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.B,   sizeof(uint8));
#else
			uint8 dummy;
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
#endif

			File.read(&pRgnInfo->lightParam.fogColor.R,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.G,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.B,		  sizeof(uint8));

			///player
			File.read(&pRgnInfo->lightParam.playerAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.B, sizeof(uint8));

			File.read(&pRgnInfo->uFogMode,					  sizeof(pRgnInfo->uFogMode));
			File.read(&pRgnInfo->lightParam.fogStart,		  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogEnd,			  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogDensity,       sizeof(float));

			///music name
			File.Read(&dwNameLen,sizeof(dwNameLen));
			File.Read(szBuf, sizeof(char)*dwNameLen);
			pRgnInfo->strMusicName = szBuf;

			///ambient effect
			size_t ambientFxSize = 0;
			File.read(&ambientFxSize, sizeof(ambientFxSize));
			for ( size_t m = 0; m < ambientFxSize; ++m )
			{
				int nIndex = 0;
				File.read(&nIndex, sizeof(nIndex));

				DWORD dwPlayMin = 0;
				File.read(&dwPlayMin, sizeof(dwPlayMin));

				DWORD dwPlayMax = 0;
				File.read(&dwPlayMax, sizeof(dwPlayMax));

				DWORD dwIntervalMin = 0;
				File.read(&dwIntervalMin, sizeof(dwIntervalMin));

				DWORD dwIntervalMax = 0;
				File.read(&dwIntervalMax, sizeof(dwIntervalMax));

				SString strEffectName	 = ambientFxNameList[nIndex];

				//SVector<CRgnAmbientFxInfo*> rgnAmbientFxList;
				CRgnAmbientFxInfo *pTemp = new CRgnAmbientFxInfo;
				pTemp->szFxName		= strEffectName;
				pTemp->nPlayMax		= dwPlayMax;
				pTemp->nPlayMin		= dwPlayMin;
				pTemp->nIntervalMax = dwIntervalMax;
				pTemp->nIntervalMin = dwIntervalMin;

				pRgnInfo->rgnAmbientFxList.push_back(pTemp);
			}

			pScene->m_RgnsList.push_back(pRgnInfo);
		}

		// ----------------------------------------------------------------------------
		// 读取摄像机轨迹动画信息

		uint32 uAnimCount = 0;
		File.read( &uAnimCount, sizeof(uint32) );	
		pScene->m_vecCamPath.resize(uAnimCount, NULL);

		for ( uint i=0; i<uAnimCount; ++i )
		{
			int		pc		= 0;
			File.read( &pc, sizeof(int) );
			if ( pc != NULL )
			{
				CCameraPath	*pNewPath	= new CCameraPath;
				pScene->m_vecCamPath[i] = pNewPath; 

				uint32 uKeyCount = 0;
				File.read( &uKeyCount, sizeof(uint32) );	

				CVector3f	data;
				float		time;
				for ( uint32 i=0; i<uKeyCount; ++i )
				{
					// 读取 摄像机位置帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddPositionKey( data, time );

					// 读取 摄像机观察点帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddTargetkey( data, time );

					// 读取 摄像机视角帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddFovKey( data, time );
				}
			}
		}
	}

	locale::global(loc);//恢复全局locale	

	return true;
}

bool sqr::CSceneAmpCoder::DeCode_Ver_H( const CDataChunk& inData,OBJ_HANDLE handle )
{
	CSceneManagerClient* pScene = (CSceneManagerClient*)handle;
	CPkgFile& File = *((CPkgFile*)inData.GetBuffer());

	DeCoder_Com_B_D(File,handle);

	//////////////////////////////////////////////////////////////////////////
	//read specular parameter 无效数据
	uint32 uColor = 0; 
	float  fPower = 0.0f;

	File.Read(&uColor, sizeof(uColor));
	File.Read(&uColor, sizeof(uColor));
	File.Read(&fPower, sizeof(fPower));
	File.Read(&fPower, sizeof(fPower));
	//////////////////////////////////////////////////////////////////////////

	locale loc = locale::global(locale(""));
	uint32 dwNameLen = 0;
	char szBuf[256];
	//////////////////////////////////////////////////////////////////////////
	//region ambient fx name
	SVector<SString>  ambientFxNameList;
	size_t size = 0;
	File.read(&size, sizeof(size_t));
	for ( size_t i = 0; i < size; ++i )
	{
		SString strEffectName = "";

		// 		File.Read(&dwNameLen,sizeof(dwNameLen));
		// 		File.Read(szBuf, sizeof(char)*dwNameLen);
		// 		szBuf[dwNameLen] = 0;
		// 		strEffectFileName = szBuf;

		File.Read(&dwNameLen,sizeof(dwNameLen));
		File.Read(szBuf, sizeof(char)*dwNameLen);
		szBuf[dwNameLen] = 0;
		strEffectName = szBuf;

		//		strEffectFileName = "fx\\setting\\mapfx_ambient_yb\\" + strEffectName + ".efx";

		ambientFxNameList.push_back(strEffectName);
	}
	//////////////////////////////////////////////////////////////////////////

	size = 0;
	File.read(&size, sizeof(size_t));
	if( size == 0 )
		return true;
	else
	{
		for( size_t i = 0; i < size; ++i )
		{
			RegionsInfo *pRgnInfo = new RegionsInfo;

			if( !pRgnInfo )
				return false;
			///scene
			File.read(&pRgnInfo->lightParam.globalAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.globalSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalSpecular.B, sizeof(uint8));

#ifndef AMBIENT_SHADOW_COLOR
			File.read(&pRgnInfo->lightParam.globalShadow.R,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.G,   sizeof(uint8));
			File.read(&pRgnInfo->lightParam.globalShadow.B,   sizeof(uint8));
#else
			uint8 dummy;
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
			File.read(&dummy,   sizeof(uint8));
#endif

			File.read(&pRgnInfo->lightParam.fogColor.R,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.G,		  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.fogColor.B,		  sizeof(uint8));

			///player
			File.read(&pRgnInfo->lightParam.playerAmbient.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerAmbient.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerDiffuse.R,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.G,  sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerDiffuse.B,  sizeof(uint8));

			File.read(&pRgnInfo->lightParam.playerSpecular.R, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.G, sizeof(uint8));
			File.read(&pRgnInfo->lightParam.playerSpecular.B, sizeof(uint8));

			File.read(&pRgnInfo->uFogMode,					  sizeof(pRgnInfo->uFogMode));
			File.read(&pRgnInfo->lightParam.fogStart,		  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogEnd,			  sizeof(float));
			File.read(&pRgnInfo->lightParam.fogDensity,       sizeof(float));

			///music name
			File.Read(&dwNameLen,sizeof(dwNameLen));
			File.Read(szBuf, sizeof(char)*dwNameLen);
			pRgnInfo->strMusicName = szBuf;

			///ambient effect
			size_t ambientFxSize = 0;
			File.read(&ambientFxSize, sizeof(ambientFxSize));
			for ( size_t m = 0; m < ambientFxSize; ++m )
			{
				int nIndex = 0;
				File.read(&nIndex, sizeof(nIndex));

				DWORD dwPlayMin = 0;
				File.read(&dwPlayMin, sizeof(dwPlayMin));

				DWORD dwPlayMax = 0;
				File.read(&dwPlayMax, sizeof(dwPlayMax));

				DWORD dwIntervalMin = 0;
				File.read(&dwIntervalMin, sizeof(dwIntervalMin));

				DWORD dwIntervalMax = 0;
				File.read(&dwIntervalMax, sizeof(dwIntervalMax));

				SString strEffectName	 = ambientFxNameList[nIndex];

				//SVector<CRgnAmbientFxInfo*> rgnAmbientFxList;
				CRgnAmbientFxInfo *pTemp = new CRgnAmbientFxInfo;
				pTemp->szFxName		= strEffectName;
				pTemp->nPlayMax		= dwPlayMax;
				pTemp->nPlayMin		= dwPlayMin;
				pTemp->nIntervalMax = dwIntervalMax;
				pTemp->nIntervalMin = dwIntervalMin;

				pRgnInfo->rgnAmbientFxList.push_back(pTemp);
			}

			pScene->m_RgnsList.push_back(pRgnInfo);
		}

		// ----------------------------------------------------------------------------
		// 读取摄像机轨迹动画信息

		uint32 uAnimCount = 0;
		File.read( &uAnimCount, sizeof(uint32) );	
		pScene->m_vecCamPath.resize(uAnimCount, NULL);

		for ( uint i=0; i<uAnimCount; ++i )
		{
			int		pc		= 0;
			File.read( &pc, sizeof(int) );
			if ( pc != NULL )
			{
				CCameraPath	*pNewPath	= new CCameraPath;
				pScene->m_vecCamPath[i] = pNewPath; 

				uint32 uKeyCount = 0;
				File.read( &uKeyCount, sizeof(uint32) );	

				CVector3f	data;
				float		time;
				for ( uint32 i=0; i<uKeyCount; ++i )
				{
					// 读取 摄像机位置帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddPositionKey( data, time );

					// 读取 摄像机观察点帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddTargetkey( data, time );

					// 读取 摄像机视角帧
					File.read( &data, sizeof(CVector3f) );
					File.read( &time, sizeof(float) );
					pNewPath->AddFovKey( data, time );
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// IsTileVisiable
		CMetaGridMgr& GridMgr = *pScene->m_pGridMgr;
		UINT Size = pScene->m_uWidth*pScene->m_uHeight;
		bool tileVisible = true;
		URegionID ID;
		for(size_t i = 0; i<Size; ++i)
		{
			ID.X = i % pScene->m_uWidth;
			ID.Z = i / pScene->m_uWidth;
			File.read( &tileVisible, sizeof(tileVisible) );

			GridMgr.SetRenderTileVisibleMark(ID, tileVisible);
		}

		//////////////////////////////////////////////////////////////////////////
	}

	locale::global(loc);//恢复全局locale	

	return true;
}