#pragma once
#include "TSingleton.h"
#include "CDynamicObject.h"
#include "CSceneMallocObject.h"

class TiXmlElement;

namespace sqr
{
	class CXmlConfig;
	class CSceneConfig
		: public Singleton<CSceneConfig>
		, public virtual CDynamicObject
		, public CSceneMallocObject
	{
	public:
		CSceneConfig();
		CSceneConfig(const string& ConfigFile);
		CSceneConfig(const string& ConfigFile,TiXmlElement* Node);
		virtual ~CSceneConfig();

		uint32	GetDefaultShaderTile()	const;
		uint32	GetDefaultAntialias()	const;
		uint32	GetDefaultShadowType()	const;
		uint32	GetDefaultWaterType()	const;
		uint32	GetDefaultTexQuality()	const;
		uint32	GetDefaultTexHMem()		const;
		uint32	GetDefaultTexMMen()		const;
		uint32	GetDefaultMaxShader()	const;
		bool	GetDefaultFreeCamera()	const;
		bool	GetDefaultCameraShake() const;
		float	GetDefaultAudioVolume(const char* category);
		bool	GetDefaultVoiceSwitch()	const;

		
		bool	GetDefaultFullScreen()	const;
		bool	GetDefaultVSync()		const;
		bool	GetDefaultBloom()		const;
		bool	GetDefaultRefract()		const;

		float	GetDefaultMinSpeed()	const;
		float	GetDefaultEffectLevel() const;
		float	GetDefaultUIScale()		const;
		
		virtual void SaveConfig(void);

		//构造函数调用的函数不能是虚函数
		void LoadDefaultConfig(void);

	protected:
		SString		m_strConfigFile;
		CXmlConfig*	m_pXmlConfig;

	protected:		
		float	m_fGUIZoom;					//Gui缩放
		SWString	m_strScreenshotPath;		//截图路径
		SVector<SString> m_lakeTexNames;

		//Shader相关
		SWString m_strShaderConfigpath;		//Shader路径		
		uint32	m_MaxShader;				//Shader高版本 0代表固定管线

		//贴图相关
		uint32	m_uTexQuailty;				//贴图质量
		uint32	m_TexHMem;					//高质量贴图最小显存限制
		uint32	m_TexMMem;					//中质量贴图最小显存限制
		
		//显示相关
		bool	m_bLockRatio;				//是否锁定成固定比例
		bool	m_bShowFps;					//是否显示FPS	
		bool	m_bBloom;					//是否开启全屏泛光
		bool	m_bRefract;					//是否开启空气折射
		bool	m_bVSync;					//是否开启垂直同步
		bool	m_bFullScreen;				//是否全屏显示
		uint32	m_iAntiAlias;				//抗锯齿等级	0为不开启
		uint32	m_iShadowType;				//阴影等级		0为不显示
		uint32	m_iWaterType;				//水质量等级
		uint32	m_iColorDepth;				//色深
		uint32	m_iTerrainType;				//地表等级
		uint32	m_iWndWidth;				//屏幕宽度
		uint32	m_iWndHeight;				//屏幕高度
		uint32	m_uRenderCyc;				//渲染频率	

		float	m_fEffectLevel;				//特效等级
		bool	m_bCameraShake;				//镜头振动等级
		float	m_fMinFogDist;				//最近雾效
		float	m_fMaxFogDist;				//最远雾效


		//摄像机相关
		bool	m_bIsFreeCamera;
		SString  m_strCameraConfigpath;		//相机路径

		//音效相关
		SMap<SString, float> m_volume;
		bool	m_bVoiceSwitch;

	public:
		float	GetGuiZoom(void);
		float	GetEffectLevel(void);
		bool	GetCameraShake(void);
		float	GetMinFogDist(void) const;
		float	GetMaxFogDist(void) const;

		uint32	GetWaterType()			const;
		uint32	GetShadowType()			const;
		uint32	GetColorDepth()			const;
		uint32	GetAntiAliasQuality()	const;
		uint32	GetTexQualityLevel(void);
		uint32	GetTerrainQuality(void);
		uint32	GetTextHightMem(void);
		uint32	GetTextMidMem(void);
		uint32	GetMaxShaderVer(void);
		uint32	GetWindowWidth()const;	
		uint32	GetWindowHeight()const;
		uint32	GetRenderCyc()const;

		bool	IsFreeCameraEnabled(void);
		bool	IsFullScreen()		const;
		bool	IsShowFps()			const;
		bool	IsLockRatio()		const;
		bool	IsBloomEnabled()	const;
		bool	IsRefractEnabled()	const;
		bool	IsVSynchronize()	const;

		void	SetGuiZoom(float zoom);
		void	SetEffectLevel(float level);
		void	SetCameraShake(bool b);
		void	SetWaterType(uint32 type);		
		void	SetShadowType(uint32 type);
		void	SetAntiAliasQuality(uint32 type);
		void	SetColorDepth(uint32 depth);
		void	SetTerrainQuality(uint32 trQuality );
		void	SetTexQualityLevel( uint32 QualityLevel );
		void	SetLockRatio(bool isLock);
		void	SetShowFps(bool isShow);
		void	SetBloomEnabled(bool b);		
		void	SetRefractEnabled(bool b);		
		void	SetVSynchronize(bool b);
		void	SetFullScreen(bool b);;		
		void	SetFreeCameraEnabled( bool isFree );
		void	SetWindowWidth(uint32 w);
		void	SetWindowHeight(uint32 h);
		void	SetRenderCyc( uint32 cyc );

		//其他
		void	SetShaderConfigPath(const SWString& sPath);
		const	wchar_t* GetShaderConfigPath()		const;
		const	wchar_t* GetScreenshotPath()		const;
		const	SVector<SString>& GetLakeTexNames()	const;
		float	GetAudioVolume(const char* category);
		void	SetAudioVolume(const char* category, float volume);
		bool	GetVoiceSwitch() const;
		void	SetVoiceSwitch(bool bOpen);

		//
		void    SetCameraConfigPath(const SString& cPath);
		const   SString&  GetCameraConfigPath()		const;
		bool    LoadCameraConfig();

	};

	inline bool		CSceneConfig::IsRefractEnabled()const				{	return m_bRefract;		}
	inline bool		CSceneConfig::IsFullScreen()	const				{	return m_bFullScreen;	}
	inline bool		CSceneConfig::IsLockRatio()		const				{	return m_bLockRatio;	}
	inline bool		CSceneConfig::IsShowFps()		const				{	return m_bShowFps;		}
	inline bool		CSceneConfig::IsBloomEnabled()	const				{	return m_bBloom;		}
	inline bool		CSceneConfig::IsVSynchronize()	const				{	return m_bVSync;		}
	inline bool		CSceneConfig::IsFreeCameraEnabled(void)				{	return m_bIsFreeCamera;	}
	
	inline uint32	CSceneConfig::GetRenderCyc()			const		{	return m_uRenderCyc;	}
	inline uint32	CSceneConfig::GetWindowWidth()			const		{	return m_iWndWidth;		}
	inline uint32	CSceneConfig::GetWindowHeight()			const		{	return m_iWndHeight;	}
	inline uint32	CSceneConfig::GetWaterType()			const		{	return m_iWaterType;	}
	inline uint32	CSceneConfig::GetShadowType()			const		{	return m_iShadowType;	}
	inline uint32	CSceneConfig::GetAntiAliasQuality()		const		{	return m_iAntiAlias;	}
	inline uint32	CSceneConfig::GetColorDepth()			const		{	return m_iColorDepth;	}
	inline uint32	CSceneConfig::GetMaxShaderVer(void)					{	return m_MaxShader;		}
	inline uint32	CSceneConfig::GetTextHightMem(void)					{	return m_TexHMem;		}
	inline uint32	CSceneConfig::GetTextMidMem(void)					{	return m_TexMMem;		}
	inline uint32	CSceneConfig::GetTexQualityLevel(void)				{	return m_uTexQuailty;	}
	inline uint32 	CSceneConfig::GetTerrainQuality(void)				{	return m_iTerrainType;	}
	

	inline float	CSceneConfig::GetMinFogDist(void) const				{	return m_fMinFogDist;	}
	inline float	CSceneConfig::GetMaxFogDist(void) const				{	return m_fMaxFogDist;	}
	inline float	CSceneConfig::GetEffectLevel(void)					{	return m_fEffectLevel;	}
	inline bool		CSceneConfig::GetCameraShake(void)					{	return m_bCameraShake;	}
	inline float	CSceneConfig::GetGuiZoom(void)						{	return m_fGUIZoom;		}
	inline const SVector<SString>&	CSceneConfig::GetLakeTexNames() const		{	return m_lakeTexNames;			}
	inline const wchar_t*			CSceneConfig::GetShaderConfigPath()const	{	return m_strShaderConfigpath.c_str();	}
	inline const wchar_t*			CSceneConfig::GetScreenshotPath() const		{	return m_strScreenshotPath.c_str();		}

	inline bool		CSceneConfig::GetVoiceSwitch() const				{	return m_bVoiceSwitch;	}
	
	inline void		CSceneConfig::SetRenderCyc( uint32 cyc )			{	m_uRenderCyc = cyc;		}
	inline void		CSceneConfig::SetWindowWidth( uint32 w )			{	m_iWndWidth = w;		}
	inline void		CSceneConfig::SetWindowHeight( uint32 h )			{	m_iWndHeight = h;		}
	inline void		CSceneConfig::SetWaterType( uint32 type )			{	m_iWaterType = type;	}
	inline void		CSceneConfig::SetShadowType( uint32 type )			{	m_iShadowType = type;	}
	inline void		CSceneConfig::SetAntiAliasQuality( uint32 type )	{	m_iAntiAlias = type;	}
	inline void		CSceneConfig::SetColorDepth( uint32 depth )			{	m_iColorDepth = depth;	}
	inline void		CSceneConfig::SetTexQualityLevel(uint32 QLevel)		{	m_uTexQuailty = QLevel;	}
	inline void 	CSceneConfig::SetTerrainQuality(uint32 TLevel)		{	m_iTerrainType = TLevel;}
	inline void		CSceneConfig::SetRefractEnabled(bool b)				{	m_bRefract = b;			}
	inline void		CSceneConfig::SetBloomEnabled( bool b )				{	m_bBloom = b;			}
	inline void		CSceneConfig::SetFreeCameraEnabled( bool isFree )	{	m_bIsFreeCamera=isFree;	}
	inline void		CSceneConfig::SetVSynchronize( bool b )				{	m_bVSync = b;			}
	inline void		CSceneConfig::SetFullScreen( bool b )				{	m_bFullScreen = b;		}
	inline void		CSceneConfig::SetEffectLevel(float level)			{	m_fEffectLevel = level;	}
	inline void		CSceneConfig::SetCameraShake(bool b)				{	m_bCameraShake = b;}
	inline void		CSceneConfig::SetGuiZoom(float zoom)				{	m_fGUIZoom = zoom;		}

	inline void		CSceneConfig::SetAudioVolume( const char* category, float volume )	{	m_volume[category] = volume;	}
	inline void		CSceneConfig::SetVoiceSwitch( bool bOpen )	{	m_bVoiceSwitch = bOpen;	}
	inline void		CSceneConfig::SetShaderConfigPath(const SWString& sPath)				{	m_strShaderConfigpath = sPath;	}
	
	inline void     CSceneConfig::SetCameraConfigPath(const SString& cPath) {   m_strCameraConfigpath=cPath;  }
	inline const SString&     CSceneConfig::GetCameraConfigPath() const     {   return m_strCameraConfigpath; }



}