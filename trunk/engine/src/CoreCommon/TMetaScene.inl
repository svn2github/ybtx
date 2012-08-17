#pragma once
#include "TMetaScene.h"
#include "IPathFinding.h"
#include "CPixelPath.h"
#include "CBufFile.h"
#include "CMath.h"
#include "GraphicCommon.h"
#include "IDynamicBarrierInfo.h"
#include "CoreCommon.h"

#define  PATH_ALIAS_SCENE L"res"

template<typename Traits>
TMetaScene<Traits>::TMetaScene(const char* szSceneName,const uint32 uSceneVer,MetaSceneMgr_t* pMgr)
:ParentPart_t(pMgr)
,m_sSceneName(szSceneName)
,m_nVersionID(uSceneVer)
,m_pMapPath(NULL)
{
	m_pMapPath		= NULL;			// 寻路信息
	m_nSubRegionNum	= 0;
	m_uRegionNum	= 0;
	m_uSubWidthInRegion = 0;
	m_uWidthInRegion	= 0;
	m_uHeightInRegion	= 0;
	m_uWidthInGrid		= 0;
	m_uHeightInGrid		= 0;
	m_uWidthInPixel		= 0;
	m_uHeightInPixel	= 0;
}

template<typename Traits>
TMetaScene<Traits>::~TMetaScene()
{
	if(m_pMapPath)
		m_pMapPath->Release();
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetYInRegion( uint32 uRegionId )
{
	return uRegionId / m_uWidthInRegion;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetXInRegion( uint32 uRegionId )
{
	return uRegionId % m_uWidthInRegion;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
sqr::TMetaScene<Traits>::GetGridY( uint32 uGridId )
{
	return uGridId & 0xf;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetGridX( uint32 uGridId )
{
	return uGridId >> 4;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetHeightInPixel()
{
	return m_uHeightInPixel;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetWidthInPixel()
{
	return m_uWidthInPixel;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetHeightInGrid()const
{
	return m_uHeightInGrid;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetWidthInGrid()const
{
	return m_uWidthInGrid;
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetHeightInRegion() const
{
	return (m_uHeightInGrid / eRegionSpan);
}
//------------------------------------------------------------------------------
template<typename Traits>
uint32
TMetaScene<Traits>::GetWidthInRegion() const
{
	return (m_uWidthInGrid / eRegionSpan);
}

//LoadAll 不能放在 TMetaScene 的构造函数里，否则，所有的虚函数调用都只是TMetaScene的虚函数，而不是子类的虚函数
template<typename Traits>
void TMetaScene<Traits>::LoadAll()
{
	//载入.amp文件
	LoadSceneInfo();

	//载入地表纹理，场景模型信息
	PreLoadRegionInfo();		//创建 Mgr 对象，如 cFindPathMgr, CTrapMgr(CSceneServer)

	//载入区域信息
	LoadRegionInfo();

	LoadLinkRegionInfo();
}

template<typename Traits>
typename TMetaScene<Traits>::MetaSceneMgr_t* TMetaScene<Traits>::GetMetaSceneMgr()const
{
	return static_cast<MetaSceneMgr_t*>(ParentPart_t::GetWhole());
}

template<typename Traits>
const char* TMetaScene<Traits>::GetSceneName()const
{
	return m_sSceneName.c_str();
}

template<typename Traits>
void TMetaScene<Traits>::PreLoadRegionInfo()
{
	m_pMapPath = CreateMap( m_uWidthInRegion, m_uHeightInRegion );
}

template<typename Traits>
void TMetaScene<Traits>::LoadSceneInfo()
{
	//CPkgFile File;
	string sSceneFileName = "common/" + m_sSceneName+".amp";

	CPkgFile PkgFile;
	if( PkgFile.Open(PATH_ALIAS_SCENE, sSceneFileName.c_str()) != eFE_SUCCESS )
		GenErr( "Can not open " + sSceneFileName + "!\n" );
	char* pBuf = new char[PkgFile.Size()];
	PkgFile.read( pBuf, PkgFile.Size() );
	sqr::CBufFile File( pBuf, PkgFile.Size() );
	PkgFile.Close();

	// magicid
	File.Read(&m_nMagicID, sizeof(int32));

	// version
	File.Read(&m_nVersion, sizeof(int32));

	// desc
	char	m_szDesc[64];
	File.Read(m_szDesc, 64);

	// width depth in region
	uint16 u16tmp;
	File.Read(&u16tmp, sizeof(uint16));
	m_uWidthInRegion = u16tmp;
	File.Read(&u16tmp, sizeof(uint16));
	m_uHeightInRegion = u16tmp;

	// REGION_SPACE
	//File.read(&m_uRegionWidth, sizeof(uint8));
	uint8 u8tmp;
	File.Read(&u8tmp, sizeof(uint8));
	m_uWidthInGrid		= m_uWidthInRegion  * eRegionSpan;
	m_uHeightInGrid		= m_uHeightInRegion * eRegionSpan;

	// GRID_SPACE
	File.Read(&m_uGridWidth, sizeof(uint8));
	m_uWidthInPixel		= m_uGridWidth * m_uWidthInGrid;
	m_uHeightInPixel	= m_uGridWidth * m_uHeightInGrid;

	m_uRegionNum		= m_uWidthInRegion * m_uHeightInRegion;
	m_uSubWidthInRegion = m_uWidthInRegion * eRegDivideNum;
	m_nSubRegionNum		= m_uRegionNum * eSubRegNum;

	// Tileset
	//File.Read(m_szDesc, 64); //走一过场

	//File.Read(&m_bCave,sizeof(bool));

	// 随时间变化信息
	//CSceneInfo			m_SceneInfo[4];	// 气氛相关信息
	//CWaterWaveInfo		m_WaveInfo;		// 波浪相关信息
	//CAmbientFxInfo		m_AmbientFxInfo;// 环境特效信息
	//CWeatherInfo		m_WeatherInfo;		// 天气相关的信息
	//CBkMusicInfo		m_BkMusicInfo;		// 背景音乐的信息
	//CCameraSetting		m_CameraInfo;		// 摄像机的信息

	//File.Read(&m_SceneInfo, sizeof( m_SceneInfo ));
	//// Wave
	//int32 n = 0;
	//TChar szBuf[256];
	//File.Read(&m_WaveInfo.WavePhysicInfo, sizeof(CWaterWaveInfo::WaterPhysic));
	//File.Read(&n,sizeof(int32));
	////Ast(n < 256);
	//File.Read(szBuf, n);
	//szBuf[n] = 0;
	//m_WaveInfo.szWaveTexture = szBuf;
	//File.Read(&n,sizeof(int32));
	//File.Read(szBuf, n);
	//szBuf[n] = 0;
	//m_WaveInfo.szMoveFxFile = szBuf;
	//File.Read(&n,sizeof(int32));
	//File.Read(szBuf, n);
	//szBuf[n] = 0;
	//m_WaveInfo.szMoveFxName = szBuf;
	//File.Read(&n,sizeof(int32));
	//File.Read(szBuf, n);
	//szBuf[n] = 0;
	//m_WaveInfo.szStandFxFile = szBuf;
	//File.Read(&n,sizeof(int32));
	//File.Read(szBuf, n);
	//szBuf[n] = 0;
	//m_WaveInfo.szStandFxName = szBuf;

	//// background music
	//memset( m_BkMusicInfo.szName, 0, sizeof( m_BkMusicInfo.szName ) );
	//uint32 dwFileNameLen;
	//File.Read(&dwFileNameLen,sizeof(uint32));
	//File.Read(m_BkMusicInfo.szName,dwFileNameLen);
	//File.Read(&m_BkMusicInfo.nMusicInterval,sizeof(int));

	//// Ambient Effect
	//int32 nAmbientFXCnt = 0, nAmbientFXSetLength = 0, nProbability = 0; 
	//File.Read(&nAmbientFXCnt,sizeof(int32));
	//File.Read(&nAmbientFXSetLength,sizeof(int32));
	//if( nAmbientFXSetLength )
	//{
	//	File.Read(szBuf, nAmbientFXSetLength);
	//	szBuf[nAmbientFXSetLength] = 0;
	//	for( int i = 0; i < nAmbientFXCnt; i++ )
	//	{
	//		m_AmbientFxInfo.Fx.push_back( CAmbientFxInfo::FX() );
	//		///effect name
	//		File.Read(&n,sizeof(int32));
	//		File.Read(szBuf, sizeof(TChar)*n);
	//		m_AmbientFxInfo.Fx[i].szFxName = szBuf;
	//		///effect file name
	//		File.Read(&n,sizeof(int32));
	//		File.Read(szBuf, sizeof(TChar)*n);
	//		m_AmbientFxInfo.Fx[i].szFxFileName = szBuf;

	//		///概率
	//		File.Read(&nProbability, sizeof(int32));
	//		m_AmbientFxInfo.Fx[i].nProbability = nProbability;
	//	}
	//}

	//// Camera Info
	//File.Read(&m_CameraInfo.fRotateX, sizeof(float));
	//File.Read(&m_CameraInfo.fRotateY, sizeof(float));
	//File.Read(&m_CameraInfo.fFov, sizeof(float));
	//File.Read(&m_CameraInfo.fBack, sizeof(float));

	//m_CameraInfo.fRotateX=20.0f;
	//m_CameraInfo.fRotateY=-45.0f;
	//m_CameraInfo.fFov=6.18f;
	//m_CameraInfo.fBack=9000.0f;
	////m_CameraInfo.fFov=27.0f;//10.0f;
	////m_CameraInfo.fBack=3000.0f;//7000.0f;

	//m_CameraInfo.fRotateX *= CMath::pi /180;
	//m_CameraInfo.fRotateY *= CMath::pi /180;
	//m_CameraInfo.fFov *= CMath::pi /180;

	/////背景图移动速度信息
	//m_bSetBackPic = false;
	//File.Read(&m_bSetBackPic, sizeof(bool));
	//if( m_bSetBackPic )
	//{
	//	File.Read(&m_fBackLeftMove, sizeof(float));
	//	File.Read(&m_fBackRightMove, sizeof(float));
	//	File.Read(&m_fBackTopMove, sizeof(float));
	//	File.Read(&m_fBackBottonMove, sizeof(float));
	//}

	/////全屏泛光信息
	//LoadHDRInfo(File);

	///////区域光信息
	//if( m_nVersion < CLIENT_VERSION )
	//	LoadSceneOldRgnInfo(File);
	//else
	//	LoadSceneNewRgnInfo(File);

	delete []pBuf;
}

//template<typename Traits>
//void TMetaScene<Traits>::LoadHDRInfo( CBufFile& File )
//{
//
//}
//
//template<typename Traits>
//void TMetaScene<Traits>::LoadSceneOldRgnInfo( CBufFile& File )
//{
//
//}
//
//template<typename Traits>
//void TMetaScene<Traits>::LoadSceneNewRgnInfo( CBufFile& File )
//{
//
//}

template<typename Traits>
void TMetaScene<Traits>::LoadRegionInfo()
{
	//由于fread系列文件操作函数已经进行缓冲区处理，所以使用CBufFile来载入文件不会有性能提升，这里直接使用CPkgFile.
	string sRegionFile = "";
	sRegionFile = GetRegionFileName(sRegionFile);
	sRegionFile = "server/" + sRegionFile;
	CPkgFile File;
	if( File.Open(PATH_ALIAS_SCENE, sRegionFile.c_str()) != eFE_SUCCESS )
		GenErr(  "Can not open " + sRegionFile + "!\n" );

	//载入Region的头
	uint32 nMagicID,nVersion,nCount;
	File.Read(&nMagicID, sizeof(uint32));
	File.Read(&nVersion, sizeof(uint32));
	File.read(&nCount, sizeof(uint32));

	uint32 dwClient = 0;
 	File.read(&dwClient, sizeof(uint32));

	m_nVersionID = dwClient;

	int32 nOffset;
	uint32 nPos	= File.Tell();

	for( uint32 i = 0; i < m_uHeightInRegion; ++i )
	{
		for(uint32 j = 0; j < m_uWidthInRegion ; ++j )
		{		
			uint32 uIdx = i*m_uWidthInRegion + j;
			File.Seek( nPos + uIdx*sizeof( uint32 ), SEEK_SET );
			File.read( &nOffset, sizeof(uint32));
			if( nOffset >= 0 )
			{
				File.Seek( nOffset, SEEK_SET );
				LoadOneRegion( File, nCount, uIdx );
			}
		}
	}

	File.Close();
}

template<typename Traits>
void TMetaScene<Traits>::LoadLinkRegionInfo()
{
	string sLinkRegionFileName = "common/" + m_sSceneName + ".lrg";

	CPkgFile PkgFile;
	if( PkgFile.Open(PATH_ALIAS_SCENE, sLinkRegionFileName.c_str()) != eFE_SUCCESS )
	{
		sLinkRegionFileName = m_sSceneName + ".lrg";
		if( PkgFile.Open(PATH_ALIAS_SCENE, sLinkRegionFileName.c_str()) != eFE_SUCCESS )
		{
			ArtErr( "Can not open " + sLinkRegionFileName + "!\n" );
			return;
		}
	}
	char* pBuf = new char[PkgFile.Size()];
	PkgFile.read( pBuf, PkgFile.Size() );
	sqr::CBufFile File( pBuf, PkgFile.Size() );
	PkgFile.Close();
	
	m_pMapPath->LoadLinkRegionData(File);

	delete [] pBuf;
}

template<typename Traits>
void TMetaScene<Traits>::LoadOneRegion( CPkgFile& File, uint32 nChunk, uint32 uRegionId )
{
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

		LoadOneChunk( File, uRegionId, rchdr.nMagicID );
	}
}

template<typename Traits>
void TMetaScene<Traits>::LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID )
{
	switch(uMagicID)
	{
	case BARR:
	case GATR:
		LoadRegionBlock(File, uRegionId );
		break;
	case EBLK:
		LoadRegionExactBlock(File, uRegionId );
		break;
	default:
		GenErr( "[Client][Load chunck]: Unknow chunck" );
		break;
	}
}

template<typename Traits>
///客户端读取阻挡信息给自动寻路
void TMetaScene<Traits>::LoadRegionBlock( CPkgFile& File, uint32 uRegionId )
{
	m_pMapPath->LoadRegionData(File, GetXInRegion(uRegionId), GetYInRegion(uRegionId) );
}

template<typename Traits>
///客户端读取精确阻挡信息给自动寻路
void TMetaScene<Traits>::LoadRegionExactBlock( CPkgFile& File, uint32 uRegionId )
{
	m_pMapPath->LoadRgnExactBlockData(File, GetXInRegion(uRegionId), GetYInRegion(uRegionId) );
}

template<typename Traits>
CPixelPath* TMetaScene<Traits>::AssemblePixelPath(IPath* pPath, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, const IDynamicBarrierInfo* pBarrierInfo)
{
	if( !pPath )
		return NULL;

	if(PixelPosBegin == PixelPosEnd)
	{
		pPath->Release();
		return NULL;
	}

	CPos PathGridPosBegin,PathGridPosEnd;
	pPath->GetPos(PathGridPosBegin,0);
	pPath->GetPos(PathGridPosEnd,pPath->GetPathSize()-1);

	CPos GridPosEnd = GetQuarGridPos(PixelPosEnd);

	CFPos PathPixelPosEnd;

	if( GridPosEnd != PathGridPosEnd )
	{
		//寻路没有达到目标点，是接近点寻路，因此要检查起点和终点坐标是否一致。

		if( GetOfsInQuarGrid(PixelPosBegin.x)==0 && GetOfsInQuarGrid(PixelPosBegin.y)==0 )
		{
			//起点坐标恰好是某个1/4格子的中心点，也就是说，起点的像素坐标与寻出来的路径的格子坐标完全重合。

			if( PathGridPosBegin == PathGridPosEnd )
			{
				//如果此时路径的起点与终点的格子坐标重合，则像素坐标也必然重合，判定路径的起点与终点完全重合。
				pPath->Release();
				return NULL;
			}
		}

		if(PathGridPosBegin == PathGridPosEnd)
		{
			pPath->Release();
			return NULL;
		}
		else
		{
			PathPixelPosEnd.x = GetPixelOfQuarGrid( PathGridPosEnd.x, 0 );
			PathPixelPosEnd.y = GetPixelOfQuarGrid( PathGridPosEnd.y, 0 );
		}
	}
	else
	{
		PathPixelPosEnd = PixelPosEnd;
	}

	return new CPixelPath(pPath,PixelPosBegin,PathPixelPosEnd);
}

// 带步长限制
template<typename Traits>
CPixelPath* TMetaScene<Traits>::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType,
										   EBarrierType eBarrierType, int32 nMaxStep, const IDynamicBarrierInfo* pBarrierInfo)
{
	CFPos PixelPosEndRevised = PixelPosEnd;
	RevisePixelPosEnd(PixelPosBegin, PixelPosEndRevised);

	IPath *pPath=m_pMapPath->CreatePath(
		GetQuarGridPos(PixelPosBegin),
		GetQuarGridPos(PixelPosEndRevised),
		eFindPathType, eBarrierType, nMaxStep, pBarrierInfo  );

	return AssemblePixelPath(pPath, PixelPosBegin, PixelPosEndRevised, pBarrierInfo);
}

template<typename Traits>
CPixelPath* TMetaScene<Traits>::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType,
										   EBarrierType eBarrierType, const IDynamicBarrierInfo* pBarrierInfo)
{
	CFPos PixelPosEndRevised = PixelPosEnd;
	RevisePixelPosEnd(PixelPosBegin, PixelPosEndRevised);

	IPath *pPath=m_pMapPath->CreatePath(
		GetQuarGridPos(PixelPosBegin),
		GetQuarGridPos(PixelPosEndRevised),
		eFindPathType, eBarrierType, pBarrierInfo );

	return AssemblePixelPath(pPath, PixelPosBegin, PixelPosEndRevised, pBarrierInfo);
}


template<typename Traits>
CPixelPath* TMetaScene<Traits>::CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, const IDynamicBarrierInfo* pBarrierInfo)
{
	CFPos PixelPosEndRevised = PixelPosEnd;
	RevisePixelPosEnd(PixelPosBegin, PixelPosEndRevised);
	IPath* pPath = m_pMapPath->CreatePath(pPathData, uPathDataLen, GetQuarGridPos(PixelPosBegin), GetQuarGridPos(PixelPosEndRevised), pBarrierInfo);
	return AssemblePixelPath(pPath, PixelPosBegin, PixelPosEndRevised, pBarrierInfo);
}

template<typename Traits>
EBarrierType TMetaScene<Traits>::GetBarrier( const CPos& GridPos )const
{
	return m_pMapPath->GetBarrierType(GridPos.x,GridPos.y);
}

template<typename Traits>
EBarrierType TMetaScene<Traits>::GetBarrier( const CPos& GridPos, const IDynamicBarrierInfo* pBarrierInfo )const
{
	return max(GetBarrier(GridPos), pBarrierInfo->GetBarrier(GridPos));
}

template<typename Traits>
uint32 TMetaScene<Traits>::GetRegionLeftX( uint32 uRegionId )
{
	return (uRegionId % m_uWidthInRegion) * eRegionSpan;
}

template<typename Traits>
uint32 TMetaScene<Traits>::GetRegionDownY( uint32 uRegionId )
{
	return (uRegionId / m_uWidthInRegion) * eRegionSpan;
}

template<typename Traits>
void TMetaScene<Traits>::CalcCrossPoint(CFPos& PixelPosCross, const CFPos& PixelPosBegin1, const CFPos& PixelPosEnd1, const CFPos& PixelPosBegin2, const CFPos& PixelPosEnd2)
{
	float fA1 = PixelPosEnd1.y - PixelPosBegin1.y;
	float fB1 = PixelPosBegin1.x - PixelPosEnd1.x;
	float fC1 = -(PixelPosBegin1.x * fA1 + PixelPosBegin1.y * fB1);
	float fA2 = PixelPosEnd2.y - PixelPosBegin2.y;
	float fB2 = PixelPosBegin2.x - PixelPosEnd2.x;
	float fC2 = -(PixelPosBegin2.x * fA2 + PixelPosBegin2.y * fB2);
	PixelPosCross.x = (fB1*fC2 - fB2*fC1) / (fA1*fB2 - fA2*fB1);
	PixelPosCross.y = (fC1*fA2 - fC2*fA1) / (fA1*fB2 - fA2*fB1);
}

template<typename Traits>
void TMetaScene<Traits>::RevisePixelPosEnd(const CFPos& PixelPosBegin, CFPos& PixelPosEndRevised)const
{
	float fSceneWidthInPixel = float(GetWidthInGrid() * eGridSpan);
	float fSceneHeightInPixel = float(GetHeightInGrid() * eGridSpan);
	bool bInLeft = PixelPosEndRevised.x < 0.0f;
	bool bInRight = PixelPosEndRevised.x > fSceneWidthInPixel;
	bool bInDown = PixelPosEndRevised.y < 0.0f;
	bool bInUp = PixelPosEndRevised.y > fSceneHeightInPixel;
	CFPos PixelPosEnd1, PixelPosBegin1;
	CFPos PixelPosEnd = PixelPosEndRevised;
	if(bInLeft && !bInDown && !bInUp)
	{
		PixelPosBegin1.x = 0.0f;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = 0.0f;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
PixelPosEnd);
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInRight && !bInDown && !bInUp)
	{
		PixelPosBegin1.x = fSceneWidthInPixel;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = fSceneWidthInPixel;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, PixelPosEnd);
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInUp && !bInLeft && !bInRight)
	{
		PixelPosBegin1.x = 0.0f;
		PixelPosBegin1.y = fSceneHeightInPixel;
		PixelPosEnd1.x = fSceneWidthInPixel;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, PixelPosEnd);
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInDown && !bInLeft && !bInRight)
	{
		PixelPosBegin1.x = 0.0f;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = fSceneWidthInPixel;
		PixelPosEnd1.y = 0.0f;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, PixelPosEnd);
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInLeft && bInDown)
	{
		PixelPosBegin1.x = 0.0f;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = 0.0f;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
PixelPosEnd);
		if(PixelPosEndRevised.y < 0.0f)
		{
			PixelPosBegin1.x = 0.0f;
			PixelPosBegin1.y = 0.0f;
			PixelPosEnd1.x = fSceneWidthInPixel;
			PixelPosEnd1.y = 0.0f;
			CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
	PixelPosEnd);
		}
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInLeft && bInUp)
	{
		PixelPosBegin1.x = 0.0f;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = 0.0f;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
PixelPosEnd);
		if(PixelPosEndRevised.y > fSceneHeightInPixel)
		{
			PixelPosBegin1.x = 0.0f;
			PixelPosBegin1.y = fSceneHeightInPixel;
			PixelPosEnd1.x = fSceneWidthInPixel;
			PixelPosEnd1.y = fSceneHeightInPixel;
			CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, PixelPosEnd);
		}
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInRight && bInDown)
	{
		PixelPosBegin1.x = fSceneWidthInPixel;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = fSceneWidthInPixel;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
PixelPosEnd);
		if(PixelPosEndRevised.y < 0.0f)
		{
			PixelPosBegin1.x = 0.0f;
			PixelPosBegin1.y = 0.0f;
			PixelPosEnd1.x = fSceneWidthInPixel;
			PixelPosEnd1.y = 0.0f;
			CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
	PixelPosEnd);
		}
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
	else if(bInRight && bInUp)
	{
		PixelPosBegin1.x = fSceneWidthInPixel;
		PixelPosBegin1.y = 0.0f;
		PixelPosEnd1.x = fSceneWidthInPixel;
		PixelPosEnd1.y = fSceneHeightInPixel;
		CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, 
PixelPosEnd);
		if(PixelPosEndRevised.y > fSceneHeightInPixel)
		{
			PixelPosBegin1.x = 0.0f;
			PixelPosBegin1.y = fSceneHeightInPixel;
			PixelPosEnd1.x = fSceneWidthInPixel;
			PixelPosEnd1.y = fSceneHeightInPixel;
			CalcCrossPoint(PixelPosEndRevised, PixelPosBegin1, PixelPosEnd1, PixelPosBegin, PixelPosEnd);
		}
		//cout << "PixelPosEnd is illegal beforeRevised = " << PixelPosEnd.x << "," << PixelPosEnd.y << " afterRevised = " << PixelPosEndRevised.x << "," << PixelPosEndRevised.y << endl;
		return;
	}
}

