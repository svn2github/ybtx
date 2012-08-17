#pragma once

#include "CDynamicObject.h"
#include "ILogicalHandler.h"
#include "IGraphic.h"
#include "CSceneMallocObject.h"

#define UseRuntimeDebug
//#define UseVedioRecord //录像开关

#ifdef UseRuntimeDebug
#include "FilterDefine.h"
#endif
namespace sqr
{
class CGraphic;
class IEffectManager;

class CRenderObject;
class CObjectDialog;
class CRenderDialog;

class CRenderScene;
class CSceneManagerClient;
class CAppClient;
class CRenderSpace;
class SQRDialog;
class RenderSettings;
class IProcessCallBack;

class CRenderSystem
			:public virtual CDynamicObject
			,public Singleton<CRenderSystem>
			,public CSceneMallocObject
{
public:
	CRenderSystem();
	~CRenderSystem(void);

	static IGraphic*	  Inst_Graphic();

public: //创建职能
	CObjectDialog*		CreateObjectDialog();
	void DestroyObjectDialog(CObjectDialog* pWnd);
	
	bool CreateSkesPool(const char* szPath);
	void InitRenderSystem(HWND hWnd);

public: //渲染相关职能
			void	Refresh(void);
			void	SetClearColor(uint32 r,uint32 g,uint32 b);
			void	Clear(void);

			void	UpdateAudioSystem();

	const	char*	Snapshot(void);	// 截屏并保存, 返回文件路径
			void		SetProcessCallBack( IProcessCallBack* pProcessCall );
	IProcessCallBack*	GetProcessCallBack( void );

public: //Config职能
	//Get
	EShadowType	GetShadowType(void) const;
	EWaterType	GetWaterType(void)	const;
	void		GetRenderSettings(RenderSettings& settings)			const;
	float		GetUIZoom(void);
	float		GetEffectLevel(void);
	bool		GetCameraShake(void);
	float		GetAudioVolume(const char* category);
	bool		GetVoiceSwitch(void)		const;
	uint32		GetScreenWidth(void);
	uint32		GetScreenHeight(void);

	bool		IsBloomEnabled(void)		const;
	bool		IsUseShaderTerrain(void)	const;
	bool		IsRefractEnabled(void)		const;
	bool		IsSelectByCursor(void);
	bool		IsShowFps(void);
	bool		IsInWindowState();


	//uint32		IsShowDebug(void);
	uint32		IsShowASyn(void);
	uint32		GetTexQualityLevel(void);
	uint32		GetBackBufferNum(void);
	uint32		GetResWidthAt(index_t i)	const;
	uint32		GetRenderCyc()				const;
	size_t		GetNumResWidths()			const;

	//Set
	virtual bool	ChangeRenderSettings(RenderSettings* settings);
	virtual bool	SetWaterType(EWaterType type);
	virtual void	SetShadowType(EShadowType type);

	virtual void	SetUIZoom(float zoom);
	virtual void	SetEffectLevel(float level);
	virtual void	SetCameraShake(bool b);
	virtual void	SetAudioVolume(const char* category, float volume);
	virtual void	SetVoiceSwitch(bool bOpen);

	virtual void	SetIsRender(bool isRender);
	virtual bool	SetBloomEnabled(bool b);
	virtual void	SetUseShaderTerrain(bool b);
	virtual bool	SetRefractEnabled(bool b);
	virtual void	SetSelectByCursor(bool isSel);
	virtual void	SetAudioMute(bool mute);// 静音(音量设为0)
	virtual void	SetShowFog( bool isFog );

	virtual void	SetTexQualityLevel( uint32 QualityLevel );
	virtual void	SetBackBufferNum(uint32 Num);
	virtual void	SetRenderCyc(uint32 cyc);

public: //功能开关
	virtual void	ShowFpsSwitch(void);
	virtual void	ShowDebugSwitch(void);
	virtual void	ShowRenderObjSwitch(void);
	virtual void	ShowGUISwith(void);
	virtual void	ShowPlayerSwitch(void);
	virtual void	ShowASynLoadSwitch(void);
	virtual void	LockAspectSwitch(void);
	virtual void	SwitchHidenMode(void);
	virtual void	SwitchIsRender(void){ SetIsRender(!m_isRender); };
	virtual void	_SyncTime(void){};
public:
	bool				m_bShowObj;
	int16				m_fIncGUI;
	int16				m_fIncPlayer;
	int16				m_RenderObjAlpha;
	int16				m_MainPlayerAlpha;
	int16				m_GUIAlpha;
	bool				m_isUseAutoLimit;
	CRenderObject*		m_pMainPlayer;
	int					m_isShowASynLoad;
	int					m_isShowDebug;

protected:
	int					m_isShowFps;
	bool				m_isShowGrid;
	bool				m_isRender;
	bool				m_isAutoLimitFps;
	bool				m_bFreeCamera;
	bool				m_bCursorSel;
	RenderQueueFlag		m_RenderFlag;
	HWND				m_hWnd;
	vector<uint>		m_resWidths;	///< 分辨率宽度
	IProcessCallBack*	m_pProcessHandler;
};

inline	bool	CRenderSystem::IsShowFps(void)			{	return m_isShowFps != 0;	}
//inline	uint32	CRenderSystem::IsShowDebug(void)		{	return m_isShowDebug;		}
inline	uint32	CRenderSystem::IsShowASyn(void)			{	return m_isShowASynLoad;	}
inline	uint32	CRenderSystem::GetScreenWidth(void)		{	return (uint32)(RenderSettings::GetScreenWidth());	}
inline	uint32	CRenderSystem::GetScreenHeight(void)	{	return (uint32)(RenderSettings::GetScreenHeight());	}
}
