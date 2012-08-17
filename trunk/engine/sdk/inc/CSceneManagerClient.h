#pragma once
#include "CSceneClassDefine.h"
#include "CDataScene.h"
#include "CDataChunk.h"
#include "IPathFinding.h"

namespace sqr
{
	class CMetaGridMgr;
	class CSceneClientCoder;
	class CSceneAmpCoder;
	class CSceneArgCoder;
	class CSceneArsCoder;
	class CVertexHardwareBuffer;
	class CNormalTerrainCoder;
	class CShaderTerrainCoder;
	class CBakedColorData;
	class CMetaRegion;
	struct GridColor;


	/*
	*	此场景的天气信息
	*/
	struct CWeatherInfo : public CSceneMallocObject
	{
		uint32		nWeatherParam;			// 天气参数，比如下雨的几率，下雪的几率
		uint32		nWindParam;				// 风力,会影响程序实现动画
	};

	class CSceneManagerClient : public sqr::CDataScene
	{
		friend class CSceneClientCoder;
		friend class CSceneAmpCoder;
		friend class CSceneArgCoder;
		friend class CSceneArsCoder;
		friend class CNormalTerrainCoder;
		friend class CShaderTerrainCoder;
		friend class CWaterCoder;
	public:
		CSceneManagerClient( const SString& SceneName, CTreeScene* pTreeScene , CRenderGroup* pGroup);
		CSceneManagerClient( const SString& SceneName, CTreeScene* pTreeScene , CRenderGroup* pGroup, uint32 Wdith , uint32 Height, uint32 RecycleSize );
		~CSceneManagerClient();
	public:
		uint32 GetSubRegionNum();
		uint32 GetRegionNum();
		uint32 GetWidthInSubRegion();
		uint8  GetGridPixel();

		IMap*					GetMapPath();

		CMetaRegion*			GetRegion(uint32 uGridX, uint32 uGridY);
		CMetaRegion*			GetRegion(uint32 RegionId);
		
		const GridColor*		GetGridColor(uint32 RegionId );
		CVertexHardwareBuffer*	GetColorBuffer(void);

		CTerrainSetUnit&		GetSetUnit( uint16 uIndex )
		{
			return m_SetUnit[uIndex];
		}	

		SString&					GetAudioUnit(uint16 uIndex)
		{
			return m_AudioUnit[uIndex];
		}

		//-------------------		OffsetHeight	-------------------
		int16	GetOffsetHeight( uint32 uX, uint32 uY );

		//-------------------		TerranHeight	-------------------
		int16	GetTerranHeight( uint32 uX, uint32 uY );

		//-------------------		TerranLogicHeight	-------------------
		int16	GetTerranLogicHeight( uint32 uX, uint32 uY );

		//-------------------		TerranNomal	-------------------
		int8	GetTerranNomal( uint32 uX, uint32 uY );

		//-------------------		TerranLogicNomal	-------------------
		CVector3f GetTerranLogicNomal( uint32 uX, uint32 uY );
	protected:
		float				m_fPercent;

		CBakedColorData*	m_pBakedColorData;
		CMetaGridMgr*		m_pGridMgr;

		
		IMap*				m_pMapPath;			// 寻路信息 //这个应该拆出去。

		uint32				m_nVersion;			// 版本
		uint32				m_uRegionID;
		char				m_szDesc[64];		// 地图的描述

		uint8				m_uGridWidth;			// 每个格子的的宽度有多少个象素
		uint32				m_uRegionNum;			// 场景中区域的个数
		uint32				m_uSubRegionNum;		// 场景中子区域的个数
		uint32				m_uWidthInSubRegion;	// 场景的宽度，以子区域为单位
		char				m_szTileSet[64];		// 地图相关的图素的名字

		//区域物件
		SVector<CTerrainSetUnit>		m_SetUnit;

		SVector<SString>				m_AudioUnit;

		//bool						m_bEnableBaking;
	protected:
		void	Initialize( void );
		void	DoProcess( float fPercent );
		void	CalculateNomal();

		//	-----------------------------
		void	OnPreLoad(void);
		void	OnLoaded(void);
	};

	inline uint8  
		CSceneManagerClient::GetGridPixel()
	{
		return m_uGridWidth;
	}


	inline uint32 
		CSceneManagerClient::GetSubRegionNum()
	{
		return m_uSubRegionNum;
	}

	inline uint32 
		CSceneManagerClient::GetRegionNum()
	{
		return m_uRegionNum;
	}

	inline uint32 
		CSceneManagerClient::GetWidthInSubRegion()
	{
		return m_uWidthInSubRegion;
	}

	inline IMap* 
		CSceneManagerClient::GetMapPath()
	{
		return m_pMapPath;
	}
}