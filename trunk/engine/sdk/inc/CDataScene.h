#pragma once 
#include "RegionLightParam.h"
#include "CBaseScene.h"
#include "CCoder.h"

namespace sqr
{
	class CShadowedGrids;
	class CDataScene;
	class CCameraPath;
	class CTerrainMgr;
	class CWaterMgr;
	class CLogicDataMgr;
	class CSkyMgr;
	class CRenderGroup;
	class IMap;

	// 场景气氛
	struct CSceneInfo : public CSceneMallocObject
	{
		uint32		dwAmbientColor;			// 环境光颜色
		uint32		dwShadowColor;			// 阴影颜色
		uint32		dwSunColor;				// 太阳光颜色
		CVector3f	vSunDir;				// 太阳光方向
		uint32		dwPlayerSunColor;		// 主角太阳光
		uint32		dwPlayerAmbientColor;	// 主角环境光颜色
		uint32		dwPlayerLightColor;		// 主角灯颜色
		float		fAttenuation;			// 主角灯强度
		float		fRange;					// 主角灯范围

		// fog
		uint32		dwFogColor;				// 雾的颜色
		float		fFogStart;				// 雾的近点
		float		fFogEnd;				// 雾的远点

		// sky
		char		szSkyTexture[64];		// 天空贴图
	};

	enum  ESceneTime
	{
		EST_MORNING		= 0,
		EST_NOON		= 1,
		EST_AFTERNOON	= 2,
		EST_NIGHT		= 3,
		EST_COUNT		= 4,
	};

	/*
	*	此场景的特效信息
	*/
	struct CAmbientFxInfo : public CSceneMallocObject
	{
		struct FX : public CSceneMallocObject
		{
			int32	nProbability;	// 概率
			SString	szFxName;		// 特效名
			SString	szFxFileName;	// 特效文件名
		};
		SVector<FX>	Fx;
	};

	/*
	*	此场景区域环境特效信息
	*/
	struct CRgnAmbientFxInfo : public CSceneMallocObject
	{
		int32	nPlayMax;		// 播放时间上限
		int32	nPlayMin;		// 播放时间下限
		int32	nIntervalMax;	// 播放间隔上限
		int32	nIntervalMin;	// 播放间隔下限
		SString	szFxName;		// 特效名
	};

	///区域信息数据结构
	struct RegionsInfo : public CSceneMallocObject
	{
		RegionLightParam			lightParam;
		uint8						uFogMode;			 ///区域场雾类型
		SString						strMusicName;
		SVector<CRgnAmbientFxInfo*> rgnAmbientFxList;

		~RegionsInfo();
	};
	
	class CSceneCoder
		: public CCoder
		, public CCoderHelper
		, public CSceneMallocObject
	{
	public:
		CSceneCoder(void);
		virtual bool SceneDeCode( CDataScene* pSceneManager ) = 0;
	protected:
		bool Lock(CDataScene* pScene);
		void UnLock(void);
	protected:
		CDataScene* m_pLockScene;
	};

	class CDataScene : public CBaseScene
	{
	public:
		CDataScene( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pRenderGroup  );
		CDataScene( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pRenderGroup, uint32 Wdith , uint32 Height, uint32 RecycleSize );
		~CDataScene();
	public:
		virtual	void	Create( void );
		virtual void	Destroy( void );

		void			SetCoder(CSceneCoder* pCoder);
		
		CTerrainMgr*	GetTerrainMgr(void);
		CSkyMgr*		GetSkyMgr(void);
		CWaterMgr*		GetWaterMgr(void);
		CLogicDataMgr*	GetLogicDataMgr(void);
		CRenderGroup*	GetRenderGroup(void);
	public:	//获取全局场景信息
			uint32	GetWidthInRegion();
			uint32	GetHeightInRegion();
			uint32	GetWidthInGrid();
			uint32	GetHeightInGrid();
			uint32	GetWidthInPixel();
			uint32	GetHeightInPixel();

			///得到当前像素的高度 = TerrainHeight + LogicHeight
			float	GetOffsetHeightInPixel( float fPixelX, float fPixelY );
			bool	IsGridValid( uint32 uX, uint32 uY );

			CShadowedGrids*		GetShadowedGrids(void);
			RegionsInfo*		GetRegionInfo( const int index );
			SubRegionData*		GetSubRegion( const int index );
			  CCameraPath*		GetCameraPath( uint32 Index );
		const CSceneInfo&		GetSceneInfo( ESceneTime eStime );
		const CAmbientFxInfo&	GetAmbientFxInfo(void);
	protected:
		virtual void	OnPreLoad(void) {};
		virtual void	OnLoaded(void) {};
		virtual void	Initialize( void );;
	protected:
		virtual IMap*		CreateIMap(uint32 uWidthInRegion, uint32 uHeightInRegion) { return NULL; }; //这个接口要多搓有多搓
		CTerrainMgr*		m_pTerrainMgr;
		CSkyMgr*			m_pSkyMgr;
		CWaterMgr*			m_pWaterMgr;
		CLogicDataMgr*		m_pLogicDataMgr;

		CSceneCoder*		m_pClientCoder;
		CShadowedGrids*		m_pShadowedGrids;
		CRenderGroup*		m_pRenderGroup;
	
	protected: //Data
		SVector<RegionsInfo*>			m_RgnsList;			//区域光信息列表
		SMap<int,SubRegionData*>		m_SubRegionsMap;	//一个场景中所有设置了区域光信息列表
		CSceneInfo						m_SceneInfo[EST_COUNT];	// 全局气氛相关信息
		SVector<CCameraPath*>			m_vecCamPath;			// 摄像机轨迹动画
		CAmbientFxInfo					m_AmbientFxInfo;		// 环境特效信息

	protected:
		uint32				m_uWidthInRegion;		// 场景的宽度，以1/4区域为单位
		uint32				m_uHeightInRegion;		// 场景的高度，以1/4区域为单位
		uint32				m_uWidthInGrid;			// 场景的宽度，以格子为单位
		uint32				m_uHeightInGrid;		// 场景的高度，以格子为单位
		uint32				m_uWidthInPixel;		// 场景的宽度，以象素为单位
		uint32				m_uHeightInPixel;		// 场景的高度，以象素为单位
	};


	inline CTerrainMgr*		CDataScene::GetTerrainMgr(void)		{ return m_pTerrainMgr;		}
	inline CSkyMgr*			CDataScene::GetSkyMgr(void)			{ return m_pSkyMgr;			}
	inline CWaterMgr*		CDataScene::GetWaterMgr(void)		{ return m_pWaterMgr;		}
	inline CLogicDataMgr*	CDataScene::GetLogicDataMgr(void)	{ return m_pLogicDataMgr;	}
	inline CRenderGroup*	CDataScene::GetRenderGroup(void)	{ return m_pRenderGroup;	}

	inline uint32 CDataScene::GetWidthInRegion()	{ return m_uWidthInRegion;	}
	inline uint32 CDataScene::GetHeightInRegion()	{ return m_uHeightInRegion;	}
	inline uint32 CDataScene::GetWidthInGrid()		{ return m_uWidthInGrid;	}
	inline uint32 CDataScene::GetHeightInGrid()		{ return m_uHeightInGrid;	}
	inline uint32 CDataScene::GetWidthInPixel()		{ return m_uWidthInPixel;	}
	inline uint32 CDataScene::GetHeightInPixel()	{ return m_uHeightInPixel;	}

	inline			CShadowedGrids*	CDataScene::GetShadowedGrids(void)				{ return m_pShadowedGrids;		}
	inline const	CSceneInfo&		CDataScene::GetSceneInfo( ESceneTime eStime )	{ return m_SceneInfo[eStime];	}
	inline const	CAmbientFxInfo&	CDataScene::GetAmbientFxInfo(void)				{ return m_AmbientFxInfo;		}
	inline			CCameraPath*	CDataScene::GetCameraPath( uint32 Index )		
	{
		if(Index<m_vecCamPath.size())
			return m_vecCamPath[Index];
		return NULL;
	}



}