#pragma once
#include "CRenderSystem.h"
#include "CRenderScene.h"
#include "TSingleton.h"

namespace sqr
{
	class CRenderSystemClient;
	class CClientRenderScene 
		: public CRenderScene
		, public Singleton<CClientRenderScene>
	{
		friend class CRenderSystemClient;
	public:
		CClientRenderScene( CDataScene* pDataScene );
		~CClientRenderScene(void);
	};

	class CRenderSystemClient
		: public CRenderSystem
	{
	public:
		CRenderSystemClient( HWND hWnd );
		~CRenderSystemClient();
		static CRenderSystemClient* Inst();
		void	SetIsRender(bool isRender);
		void	SetBackBufferNum(uint32 Num);
		void	Render(void);
		void	Update(void);

		bool	IsFreeCameraEnabled(void);
		void	SetFreeCameraEnabled(bool isFree);
		void	SetFree3DCameraEnabled(bool isFree);

		CClientRenderScene*	CreateRenderScene(  CDataScene* pMetaScene );
		void			DestroyRenderScene(CClientRenderScene* pScene);


		void	InitCameraInfo();
		void	SetCameraInfo(ECamMode c);
		void	SetCameraInfo(float w, float h);
		void	SetCameraNoChange(bool noc);
		bool	GetCameraNoChange();
		
	public: //功能开关
		void	ShowFpsSwitch(void);
		void	ShowDebugSwitch(void);
		//void	ShowRenderObjSwitch(void);
		void	ShowGUISwith(void);
		void	ShowPlayerSwitch(void);
		void	ShowASynLoadSwitch(void);
		void	LockAspectSwitch(void);
		void	SwitchHidenMode(void);

		float	GetCamMaxXRadian(void)	const;
		float	GetCamMinXRadian(void)	const;
		float	GetCamYRadian(void)		const;
		float	GetCamMaxFovRadian(void)const;
		float	GetCamMinFovRadian(void)const;
		float	GetCamMaxNearPlan(void)	const;
		float	GetCamMinNearPlan(void)	const;
		float	GetCamMinYOffset(void)	const;
		float	GetCamMaxYOffset(void)	const;
		float	GetCamMinDist(void)		const;
		float	GetCamMaxDist(void)		const;
		float	GetCamMaxHOffset(void)	const;
		float	GetCamSpeed(void)		const;
		float	GetCamDefDist(void)		const;

	protected:
		void					_SyncTime(void);
		ECamMode				m_CamMode;
		bool					m_NoChange;
	};

	inline void CRenderSystemClient::SetCameraNoChange(bool noc)
	{
		m_NoChange = noc;
	}
	inline bool	CRenderSystemClient::GetCameraNoChange()
	{
		return m_NoChange;
	}

}