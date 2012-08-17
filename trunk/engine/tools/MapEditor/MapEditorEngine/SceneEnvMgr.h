#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"
#include "CVector3.h"
#include "CVector2.h"
#include "CColor4.h"

namespace sqr
{
	/*
	场景环境（包括了场景光）
	*/

	class CClockSystem;
	class CShadowedGrids;	
	class CBloomTarget;

	class CSceneEnvMgr: public Singleton<CSceneEnvMgr>
	{
	public:
		enum FogModeType
		{
			FMT_NONE	= 0,
			FMT_EXP		= 1,
			FMT_EXP2	= 2,
			FMT_LINEAR	= 3,
			FMT_FORCE_DWORD = 0x7fffffff,
		};

		struct SEnv
		{
			DWORD		m_dwSceneAmbient;
			DWORD		m_dwSceneDirColor;
			CVector3f	m_vDirDir;
			DWORD		m_dwShadowColor;

			DWORD		m_dwPlayerAmbientColor;
			DWORD		m_dwPlayerDirColor;

			float		m_fFogStart;
			float		m_fFogEnd;
			float		m_fFogDensity;

			DWORD		m_dwFogColor;

			DWORD		m_dwSceneSpecularColor;
			DWORD		m_dwPlayerSpecularColor;

			string		m_strTextureSkyName;

			SEnv()
				: m_dwSceneAmbient(0), m_dwSceneDirColor(0), m_dwShadowColor(0)
				, m_dwPlayerAmbientColor(0), m_dwPlayerDirColor(0)
				, m_fFogStart(0), m_fFogEnd(0), m_fFogDensity(0), m_dwFogColor(0){}
		};

	private:

		// ----------------------------------------------------------------------------
		// 环境
		CShadowedGrids*	m_pShadowedGrids;

		SEnv			m_envs;

		///specular light parameter
		float			m_fSReflectionPower[2]; ///specular power 只有material 可以设置该参数
	
		float			m_fBackLeftMove;
		float			m_fBackRightMove;
		float			m_fBackTopMove;
		float			m_fBackBottonMove;
		CVector2f		m_UVCenter;
		float			m_fSkyBoxScale;
		string			m_strSkyboxTexture;
		float			m_fLightMapZOffset;

		FogModeType		m_fogMode; // fog公式计算类型

		bool			m_bOnAutoSaveMap;
		bool			m_bEnableFog;
		bool			m_bSetBackPicMove;
		bool			m_bLightMapShadow;
		bool			m_bSetLightByScreenRange;

		string			m_strBackgroundMusic;

		BYTE			m_byStarVisibility;

		bool			m_bIsPlaySceneCues;///是否switch play场景音效
		bool			m_bGenerateMinMap;

		//全屏泛光
		CBloomTarget    *m_pTarget;
		// 全屏泛光的一些参数
		bool			m_bTurnOnGassTarget;// 开启全屏泛光
		float			m_fGaussBrightPass;		// 光通量 0~1
		float			m_fGaussMultiplier; // 泛光强度 0~2
		float			m_fGaussMean;		// 泛光偏移 -1~+1
		float			m_fGaussStdDev;		// 泛光融和程度  0~1
		float			m_fGaussExposure;		// 最终亮度0~2
		float			m_fGaussSam;				// 混合度
		// ----------------------------------------------------------------------------

		DWORD			m_dwLastAutoSaveTime;

	private:
		void		UpdateSceneFog();

	public:

		CSceneEnvMgr();
		~CSceneEnvMgr();

		void		Initialize();
		void		InitiaRgnDefault();

		void		UpdateClock(const DWORD dwTimeElapsed);

		void		Update();
		void		UpdateSkyBoxFPSLimited();
		//////////////////////////////////////////////////////////////////////////

		void		SetSkyTextureName(std::string name);
		std::string	GetSkyTextureName() const;
		
		void		SetIsBackPicMove( const bool& bset );
		bool		GetISBackPicMove() const;
		
		void		SetBackPicLeftMoveOffset( const float& offset );
		float		GetBackPicLeftMoveOffset() const;
	
		void		SetBackPicRightMoveOffset( const float& offset );
		float		GetBackPicRightMoveOffset() const;

		void		SetBackPicTopMoveOffset( const float& offset );
		float		GetBackPicTopMoveOffset() const;

		void		SetBackPicBottonMoveOffset( const float& offset );
		float		GetBackPicBottonMoveOffset() const;

		void		SetBackTextureUVCenter( const CVector2f& center );
		void		SetBackTextureUCenter( const float u );
		
		void		SetBackTextureVCenter( const float v );
		CVector2f	GetBackTextureUVCenter() const;
		void		AdjustBackTextureUVCenter();
		
		void		SetBackTextureScale( const int scale );
		float		GetBackTextureScale() const;

		string		GetSkyFarBackTextureName();
		void		SetSkyFarBackTextureName(string name);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//scene ambinet
		void		SetSceneAmbientColor(DWORD color);
		DWORD		GetSceneAmbientColor() const;

		//player ambient
		void		SetPlayerAmbientColor(DWORD color);
		DWORD		GetPlayerAmbientColor() const; 
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//scene  sun diffuse
		void		SetSceneDirColor(DWORD color);
		DWORD		GetSceneDirColor() const;

		//player sun diffuse
		DWORD		GetPlayerDirColor() const;
		void		SetPlayerDirColor(DWORD color);
	
		CVector3f	GetDirDir() const;
		void		SetDirDir(const CVector3f v);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//shadow
		void		SetShadowColor(DWORD color);
		DWORD		GetShadowColor() const;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///specular
		void		SetSceneSpecurlarColor( DWORD dw );
		DWORD		GetSceneSpecurlarColor() const;

		void		SetPlayerSpecurlarColor( DWORD dw );
		DWORD		GetPlayerSpecurlarColor() const;

		void		SetSpecularReflectionPower( const int index, const float value );
		float		GetSpecularReflectionPower(const int index) const;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///fog
		void		SetDoesEnableFog(bool b);
		bool		GetDoesEnableFog() const;

		void		SetVertexFog(FogModeType mode);
		FogModeType GetFogMode() const;

		void		SetFogColor(DWORD dw);
		DWORD		GetFogColor() const;

		void		SetFogStart(float fStart);
		float		GetFogStart() const;

		void		SetFogEnd(float fEnd);
		float		GetFogEnd() const;

		void		SetDensity(float fDensity);
		float		GetDensity() const;

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///rgn
		void		RgnReSetColorInfo(DWORD ambient, DWORD diffuse, DWORD fog);
		void		RgnReSetFogParam( const int nFogMode, const float start, const float end, const float density );
		void		RgnReSetSpecularColorInfo(DWORD sceneSpecular, DWORD playerSpecular, float fPower);
		void		RgnReSetPlayerColor(DWORD ambient, DWORD diffuse);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///back music
		void		SetBackGroundMusic(string & str, bool bRegion);
		void		StopBackMusic();
		string &	GetBackGroundMusic();
		
		void		SetIsPlaySceneCues( const bool b );
		bool		GetIsPlaySceneCues() const;

		void		SetIsGenerateMinMap( const bool b );

		bool		GetIsGenerateMinMap() const;
		//////////////////////////////////////////////////////////////////////////

		// -----------------------------------------------------------------------------
		void		SetLightMapZOffset(const float zOffset);
		float		GetLightMapZOffset() const;

		void		SetIsLightMapShadow(bool b);
		bool		GetIsLightMapShadow()const;

		void		SetIsShowScreenRangeGrid(bool b);
		bool		GetIsShowScreenRangeGrid() const;

		void		ShowLightMap(bool b);
		void		ShowRealtimeShadow(bool b);
		bool		GetIsRealtimeShadow();

		sqr::CColor GetShadowedColor( int xGrid, int yGrid );
		void		LoadShadowedColor();
		bool		ShadowedColorIsEmpty()  const;

		//////////////////////////////////////////////////////////////////////////
		///全屏泛光
		void		CreateBloomTarget();
		bool		HadBloomTarget();
		
		void		SetGaussBrightPass(const float fBrightPass);
		float		GetGaussBrightPass() const;
		
		void		SetGaussMultiplier(const float fGaussMultiplier);
		float		GetGaussMultiplier() const;
		
		void		SetGaussMean(const float fGaussMean);
		float		GetGaussMean() const;

		void		SetGaussStdDev(const float fGaussStdDev);
		float		GetGaussStdDev() const;

		void		SetGaussExposure(const float fExposure);
		float		GetGaussExposure() const;
		
		void		SetGaussSample(const float fSam);
		float		GetGaussSample() const;
		
		void		InitGaussParams();
		void		SetIsTurnOnGaussTarget(const bool bTurn);
		bool		GetIsTurnOnGaussTarget() const;
		void		SetIsTurnOnRefractt(const bool bTurn);

		void		SaveGaussParamsInfo(FILE * fp);
		void		LoadGaussParamsInfo(FILE * fp);
		//////////////////////////////////////////////////////////////////////////
	};
}
