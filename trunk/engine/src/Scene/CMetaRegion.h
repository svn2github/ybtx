#pragma once
#include "CWater.h"
#include "CAxisAlignedBox.h"

namespace sqr
{

	// 区域标志定义
#define			REGION_FLAG_SKY		0x01
	struct RegionWater : public CSceneMallocObject
	{
		RegionWater()
		{
			for ( int i = 0; i < eVertexNumPerRegion; i++ )
			{
				m_cWaterColor[i]  = 0;
				m_nWaterHeight[i] = SHRT_MIN;
			}
		}
		SVector<CWater>		m_Waters;
		short				m_nWaterHeight[eVertexNumPerRegion];
		uint32				m_cWaterColor [eVertexNumPerRegion];
		CAxisAlignedBox		aabb;	///< 包围盒, 用于可见性判断
	};

	struct CRegionLI : public CSceneMallocObject
	{
		float	fAttenuation0;
		float	fAttenuation1;
		float	fAttenuation2;
		uint16	fRang;
		uint8	rColor;
		uint8	gColor;
		uint8	bColor;
		uint8	rDyn_Color;
		uint8	gDyn_Color;
		uint8	bDyn_Color;
		uint8	Dyn_ChangeProbability;
		uint8	Dyn_BrightTime;
		uint8	Dyn_DarkTime;
		uint8	Dyn_ChangeTime;
	};

	struct CRegionSpotLight : public CSceneMallocObject
	{
		float				m_fAttenuation0;
		float				m_fAttenuation1;
		float				m_fAttenuation2;
		float				m_DirectionX;	 ///聚光灯方向
		float				m_DirectionY;
		float				m_DirectionZ;
		float				m_TargetPosX;	///聚光灯方向终点位置，也即目标位置
		float				m_TargetPosY;
		float				m_TargetPosZ;
		float				m_fTheta;		 ///指定内圆锥角度，单位是弧度
		float				m_fPhi;			 ///指定外圆锥角度，单位是弧度
		uint16				m_fRang;
		uint16				m_fFallOff;      ///从内圆锥到外圆锥之间的强度衰减
		uint8				m_rColor;
		uint8				m_gColor;
		uint8				m_bColor;
	};

	struct CRegionOldMI : public CSceneMallocObject
	{
		float   cX;
		float   cY;
		float   cZ;
		uint16  SetIndex;
		uint8   GridIndex;
		uint8   byRotation;//y
		uint8   bxRotation;//x
		uint8   bzRotation;//z
		int16	fScaleX;
		int16	fScaleY;
		int16	fScaleZ;
		uint8	nFlag;
		uint8	r;
		uint8	g;
		uint8	b;
	};

	struct CRegionMI : public CSceneMallocObject
	{
		float   cX;
		float   cY;
		float   cZ;
		float   fxRotation;//x
		float   fyRotation;//y
		float   fzRotation;//z
		int16	fScaleX;
		int16	fScaleY;
		int16	fScaleZ;
		uint16  SetIndex;
		uint8   GridIndex;
		uint8	nFlag;
		uint8	r;
		uint8	g;
		uint8	b;
		bool	bEffectByConfigur;
		bool	bCameraCollision;
	};


	struct CRegionAI : public CSceneMallocObject//AI Audio Info
	{
		float   cX;
		float   cY;
		float   cZ;
		uint32  uPlayTimeLenMin;
		uint32  uPlayTimeLenMax;
		uint32  uPlayIntervalMin;
		uint32  uPlayIntervalMax;
		uint16  SetIndex;
		uint8   GridIndex;
		bool	bPlayNotRepeat;
	};

	typedef SMap<int, SMap<int16, SString>>  CameraAniFrameMap;   
	struct CRegionLoginI : public CSceneMallocObject ///Login Info
	{
		CRegionMI			sMoldeInfo;
		SString				sModelLogicName;
		CameraAniFrameMap	loginAniCamAniMap; 
	};

	struct CRegionLIEx : public CSceneMallocObject
	{
		uint8		uGridIndex;
		int16		nHeight;
		CRegionLI	LightInfo;
	};

	struct RegionTrap : public CSceneMallocObject
	{
		RegionTrap()
		{
			memset( aryTrap, 0xff, sizeof(aryTrap) );
		}
		uint8	aryTrap[eRegionSpan][eRegionSpan];
	};

	enum EModel_State
	{
		eMS_NotLoad,
		eMS_Loading,
		eMS_Loaded,
	};

	class CMetaRegion : public CSceneMallocObject
	{
	protected:
		/*可释放内容*/
		struct	RegionTempData : public CSceneMallocObject
		{
			RegionTempData();
			CColor				m_aryTerranBakingColor[eRegionSpan][eRegionSpan];	///顶点烘培色
			CColor				m_aryTerranSpecularColor[eRegionSpan][eRegionSpan];	///顶点高光色
			CVector2f			m_aryLayer1UV[eRegionSpan][eRegionSpan];			///第一层纹理坐标
			CVector2f			m_aryLayer2UV[eRegionSpan][eRegionSpan];			///第二层纹理坐标
			CVector2f			m_aryLayer3UV[eRegionSpan][eRegionSpan];			///第一层纹理坐标
			CVector2f			m_aryLayer4UV[eRegionSpan][eRegionSpan];			///第二层纹理坐标
			uint32				m_aryTextureIndex[eRegionSpan][eRegionSpan];		///< 纹理索引
			bool				m_aryHollowMask[eRegionSpan][eRegionSpan];			///网格镂空标志
		};

	public:
		CMetaRegion( uint16 uRegionId );
		virtual ~CMetaRegion();

		//void Release();
		inline uint32	GetWidth()
		{
			return eRegionSpan;
		}
		inline uint32	GetHeight()
		{
			return eRegionSpan;
		}
		//inline void		SetLoadState( EModel_State eState ) { m_eLoadState = eState; if(eMS_Loaded==m_eLoadState)m_RegionModelInfo.clear();	}
		inline uint16	GetRegionId()
		{
			return m_uRegionId;
		}

		//-------------------		WaterInfo	-------------------
		RegionWater*	GetWaterInfo();
		void	SetWaterInfo( RegionWater* pWater );

		//-------------------		OffsetHeight	-------------------
		int16	GetOffsetHeight( uint32 uX, uint32 uY );
		void	SetOffsetHeight( uint32 uId, int16 uVal );

		//-------------------		TerranHeight	-------------------
		int16	GetTerranHeight( uint32 uX, uint32 uY );
		void	SetTerranHeight( uint32 uId, int16 uVal );

		//-------------------		TerranNomal	-------------------
		int8	GetTerranNomal( uint32 uX, uint32 uY );
		void	SetTerranNomal( uint32 uId, int8 uVal );

		//-------------------		TerranLogicNomal	-------------------
		CVector3f GetTerranLogicNomal( uint32 uX, uint32 uY );
		void	  SetTerranLogicNomal( uint32 uId, const CVector3f& uVal );

		//-------------------		Terrain Material Index	-------------------
		int8	GetTerrainMaterialIndex( uint32 uX, uint32 uY )  ;
		void	SetTerrainMaterialIndex( uint32 uId, int8 uVal );

		//-------------------		Terran diffuse Color	-------------------
		const CColor&	GetTerranDiffuseColor( uint32 uX, uint32 uY );
		void	SetTerranDiffuseColor( uint32 uId, CColor& uVal );

		//-------------------		Terran specular Color	-------------------
		const CColor&	GetTerranSpecularColor( uint32 uX, uint32 uY );
		void	SetTerranSpecularColor( uint32 uId, CColor& uVal );

		//-------------------		Terran baking Color	-------------------
		const CColor&	GetTerranBakingColor( uint32 uX, uint32 uY );
		void	SetTerranBakingColor( uint32 uId, CColor& uVal );

		//-------------------		TextureIndex	-------------------
		uint32	GetTextureIndex( uint32 uX, uint32 uY );
		void	SetTextureIndex( uint32 uId, uint32 uVal );

		const CVector2f&	GetTerranLayer1UV( uint32 uX, uint32 uY );
		void	SetTerranLayer1UV( uint32 uId, CVector2f& uv );

		const CVector2f&	GetTerranLayer2UV( uint32 uX, uint32 uY );
		void	SetTerranLayer2UV( uint32 uId, CVector2f& uv );

		const CVector2f&	GetTerranLayer3UV( uint32 uX, uint32 uY );
		void	SetTerranLayer3UV( uint32 uId, CVector2f& uv );

		const CVector2f&	GetTerranLayer4UV( uint32 uX, uint32 uY );
		void	SetTerranLayer4UV( uint32 uId, CVector2f& uv );

		void    SetTerrainGridIsHollow( uint32 uId, bool& hollow );
		bool	GetTerrainGridIsHollow( uint32 uX, uint32 uY );
		//-------------------		Trap	-------------------
		void	NewTrap();
		uint8	GetTrap( uint32 uX, uint32 uY );
		void	SetTrap( uint32 uId, uint8 uVal );

		//-------------------		CRegionMI	-------------------
		SVector<CRegionMI>& GetModelInfo();

		//-------------------		CRegionAI	-------------------
		SVector<CRegionAI>& GetAudioInfo();

		//-------------------		CRegionLI	-------------------
		SVector<CRegionLIEx>& GetLightInfo();

		//-------------------		CRegionLoginI	-------------------
		SVector<CRegionLoginI>& GetLoginModelInfo();

		///区域光相关属性
		void    SetIsSetRegionEnv(const bool& bset);
		bool	GetIsSetRegionEnv() const;

		void	   SetRegionCenter(const CVector2f& center);
		const CVector2f&  GetRegionCenter() const;

		void	SetRgnAmbient(const CColor& color);
		const CColor&  GetRgnAmbient() const;

		void	SetRgnDiffuse(const CColor& color);
		const CColor&  GetRgnDiffuse() const;

		void	SetRgnFog(const CColor& color);
		const CColor&  GetRgnFog()	const;

		void	SetRgnPalyerAmbient(const CColor& color);
		const CColor&  GetRgnPalyerAmbient() const;

		void	SetRgnPalyerDiffuse(const CColor& color);
		const CColor&  GetRgnPalyerDiffuse() const;

		void	SetRgnMusicName(const SString& musicname);
		const SString& GetRgnMusicName() const;

	protected:
		inline uint32	_getXInRgnFromId( uint32 uId )
		{
			return uId >>4;
		}
		inline uint32	_getYInRgnFromId( uint32 uId )
		{
			return uId &0xf;
		}

	protected:
		int16						m_aryOffsetHeight[eRegionSpan][eRegionSpan];			///< 逻辑高
		int16						m_aryTerranHeight[eRegionSpan][eRegionSpan];			///< 地形高
		uint8						m_aryTerranNomal[eRegionSpan][eRegionSpan];				///< 地形法线
		CVector3f					m_aryTerranLogicNomal[eRegionSpan][eRegionSpan];		///< 地形Logic法线
		CColor						m_aryTerranDiffuseColor[eRegionSpan][eRegionSpan];		///< 地形顶点色
		int8						m_aryTerrainMatIndex[eRegionSpan][eRegionSpan];			///< 地形材质索引

		RegionTrap*				m_pTrapInfo;
		RegionTempData*			m_pTempData;
		RegionWater*			m_pWaterInfo;			///< 水
		uint16					m_uRegionId;			///< 区域

		//EModel_State			m_eLoadState;			///< 加载状态
		SVector<CRegionMI>		m_RegionModelInfo;		///< 模型信息
		SVector<CRegionAI>		m_RegionAudioInfo;		///< 音源信息
		SVector<CRegionLIEx>		m_RegionLightInfo;		///< 光源信息
		SVector<CRegionLoginI>	m_RegLoginModelInfo;	///< 登录模型信息

		bool					m_bSetRegionEnv;		///是否设置区域光
		CVector2f				m_RegionCenter;		 ///region center
		CColor					m_RegionAmbient;
		CColor					m_RegionDiffuse;
		CColor					m_RegionFog;
		CColor					m_RegionPlayerAmbient;
		CColor					m_RegionPlayerDiffuse;
		SString					m_strRegionMusicName;
	};
	//---------------------------------------------------------------------------------
	inline uint8	CMetaRegion::GetTrap( uint32 uX, uint32 uY )
	{
		return m_pTrapInfo ? m_pTrapInfo->aryTrap[uY][uX] : 0xff;
	}

	inline void	CMetaRegion::SetTrap( uint32 uId, uint8 uVal )
	{
		m_pTrapInfo->aryTrap[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		CRegionMI	-------------------
	inline SVector<CRegionMI>& CMetaRegion::GetModelInfo()
	{
		return m_RegionModelInfo;
	}

	//-------------------		CRegionAI	-------------------
	inline SVector<CRegionAI>& CMetaRegion::GetAudioInfo()
	{
		return m_RegionAudioInfo;
	}

	//-------------------		CRegionLI	-------------------
	inline SVector<CRegionLIEx>& CMetaRegion::GetLightInfo()
	{
		return m_RegionLightInfo;
	}

	//-------------------		CRegionLoginI	-------------------
	inline SVector<CRegionLoginI>& CMetaRegion::GetLoginModelInfo()
	{
		return m_RegLoginModelInfo;
	}

	///区域光相关属性
	inline void    CMetaRegion::SetIsSetRegionEnv(const bool& bset)
	{
		m_bSetRegionEnv = bset;
	}

	inline bool	CMetaRegion::GetIsSetRegionEnv() const
	{
		return m_bSetRegionEnv;
	}

	inline void	   CMetaRegion::SetRegionCenter(const CVector2f& center)
	{
		m_RegionCenter = center;
	}
	inline const CVector2f&  CMetaRegion::GetRegionCenter() const
	{
		return m_RegionCenter;
	}

	inline void	CMetaRegion::SetRgnAmbient(const CColor& color)
	{
		m_RegionAmbient = color;
	}
	inline const CColor&  CMetaRegion::GetRgnAmbient() const
	{
		return m_RegionAmbient;
	}

	inline void	CMetaRegion::SetRgnDiffuse(const CColor& color)
	{
		m_RegionDiffuse = color;
	}
	inline const CColor&  CMetaRegion::GetRgnDiffuse() const
	{
		return m_RegionDiffuse;
	}

	inline void	CMetaRegion::SetRgnFog(const CColor& color)
	{
		m_RegionFog = color;
	}
	inline const CColor&  CMetaRegion::GetRgnFog() const
	{
		return m_RegionFog;
	}

	inline void	CMetaRegion::SetRgnPalyerAmbient(const CColor& color)
	{
		m_RegionPlayerAmbient = color;
	}
	inline const CColor&  CMetaRegion::GetRgnPalyerAmbient() const
	{
		return m_RegionPlayerAmbient;
	}

	inline void	CMetaRegion::SetRgnPalyerDiffuse(const CColor& color)
	{
		m_RegionPlayerDiffuse = color;
	}
	inline const CColor&  CMetaRegion::GetRgnPalyerDiffuse() const
	{
		return m_RegionPlayerDiffuse;
	}

	inline void CMetaRegion::SetRgnMusicName(const SString& musicname)
	{
		m_strRegionMusicName = musicname;
	}
	inline const SString& CMetaRegion::GetRgnMusicName() const
	{
		return m_strRegionMusicName;
	}

	//-------------------		WaterInfo	-------------------
	inline RegionWater*	CMetaRegion::GetWaterInfo()
	{
		return m_pWaterInfo;
	}

	inline void	CMetaRegion::SetWaterInfo( RegionWater* pWater )
	{
		m_pWaterInfo = pWater;
	}

	//-------------------		OffsetHeight(Logic Height)	-------------------
	inline int16	CMetaRegion::GetOffsetHeight( uint32 uX, uint32 uY )
	{
		return m_aryOffsetHeight[uY][uX];
	}

	inline void	CMetaRegion::SetOffsetHeight( uint32 uId, int16 uVal )
	{
		m_aryOffsetHeight[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		TerranHeight	-------------------
	inline int16	CMetaRegion::GetTerranHeight( uint32 uX, uint32 uY )
	{
		return m_aryTerranHeight[uY][uX];
	}

	inline void	CMetaRegion::SetTerranHeight( uint32 uId, int16 uVal )
	{
		m_aryTerranHeight[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		TerranNomal	-------------------
	inline int8	CMetaRegion::GetTerranNomal( uint32 uX, uint32 uY )
	{
		return m_aryTerranNomal[uY][uX];
	}

	inline void	CMetaRegion::SetTerranNomal( uint32 uId, int8 uVal )
	{
		m_aryTerranNomal[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		TerranLogicNomal	-------------------
	inline CVector3f CMetaRegion::GetTerranLogicNomal( uint32 uX, uint32 uY ) 
	{
		return m_aryTerranLogicNomal[uY][uX];
	}

	inline void CMetaRegion::SetTerranLogicNomal( uint32 uId, const CVector3f& uVal )
	{
		m_aryTerranLogicNomal[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		Terrain Material Index	-------------------
	inline int8 CMetaRegion::GetTerrainMaterialIndex( uint32 uX, uint32 uY )  
	{
		return m_aryTerrainMatIndex[uY][uX];
	}

	inline void	CMetaRegion::SetTerrainMaterialIndex( uint32 uId, int8 uVal )
	{
		m_aryTerrainMatIndex[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		Terran Diffuse Color	-------------------
	inline const CColor&	CMetaRegion::GetTerranDiffuseColor( uint32 uX, uint32 uY )
	{
		return m_aryTerranDiffuseColor[uY][uX];
	}

	inline void	CMetaRegion::SetTerranDiffuseColor( uint32 uId, CColor& uVal )
	{
		m_aryTerranDiffuseColor[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		Terran specular Color	-------------------
	inline const CColor& CMetaRegion::GetTerranSpecularColor( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryTerranSpecularColor[uY][uX];
	}

	inline void	CMetaRegion::SetTerranSpecularColor( uint32 uId, CColor& uVal )
	{
		m_pTempData->m_aryTerranSpecularColor[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		Terran baking Color	-------------------
	inline const CColor&	CMetaRegion::GetTerranBakingColor( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryTerranBakingColor[uY][uX];
	}

	inline void	CMetaRegion::SetTerranBakingColor( uint32 uId, CColor& uVal )
	{
		m_pTempData->m_aryTerranBakingColor[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	//-------------------		TextureIndex	-------------------
	inline uint32	CMetaRegion::GetTextureIndex( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryTextureIndex[uY][uX];
	}

	inline void	CMetaRegion::SetTextureIndex( uint32 uId, uint32 uVal )
	{
		m_pTempData->m_aryTextureIndex[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uVal;
	}

	inline const CVector2f&	CMetaRegion::GetTerranLayer1UV( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryLayer1UV[uY][uX];
	}

	inline void	CMetaRegion::SetTerranLayer1UV( uint32 uId, CVector2f& uv )
	{
		m_pTempData->m_aryLayer1UV[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uv;
	}

	inline const CVector2f&	CMetaRegion::GetTerranLayer2UV( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryLayer2UV[uY][uX];
	}

	inline void	CMetaRegion::SetTerranLayer2UV( uint32 uId, CVector2f& uv )
	{
		m_pTempData->m_aryLayer2UV[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uv;
	}

	inline const CVector2f&	CMetaRegion::GetTerranLayer3UV( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryLayer3UV[uY][uX];
	}

	inline void	CMetaRegion::SetTerranLayer3UV( uint32 uId, CVector2f& uv )
	{
		m_pTempData->m_aryLayer3UV[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uv;
	}

	inline const CVector2f&	CMetaRegion::GetTerranLayer4UV( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryLayer4UV[uY][uX];
	}

	inline void	CMetaRegion::SetTerranLayer4UV( uint32 uId, CVector2f& uv )
	{
		m_pTempData->m_aryLayer4UV[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = uv;
	}

	inline void    CMetaRegion::SetTerrainGridIsHollow( uint32 uId, bool& hollow )
	{
		m_pTempData->m_aryHollowMask[_getXInRgnFromId(uId)][_getYInRgnFromId(uId)] = hollow;
	}

	inline bool	CMetaRegion::GetTerrainGridIsHollow( uint32 uX, uint32 uY )
	{
		return m_pTempData->m_aryHollowMask[uY][uX];
	}

	//-------------------		Trap	-------------------
	inline void	CMetaRegion::NewTrap()
	{
		m_pTrapInfo = new RegionTrap;
	}
}