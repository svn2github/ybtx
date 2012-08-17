#pragma once

#include "CCtrlBase.h"

//use STL

#define	MGT_VIDEOSET_UPDATE	0x01
#define	MGT_VIDEOSET_RESET	0x02
#define	MGT_VIDEOSET_ENSURE	0x03

namespace sqr_tools
{
	
	struct VideoSettingState
	{
		unsigned int	AliaxLevel;
		unsigned int	ShadowLevel;
		unsigned int	TerrainLevel;
		unsigned int	WaterLevel;
		unsigned int	TexLevel;
		unsigned int	EffectLevel;
		bool			IsRefract;
		bool			IsBloom;
		unsigned int	IsSyn;
		int				WindowWidth;
		int				WindowHeight;
		int				FrameBorder;
	};

	class CVideoSettingImp :public CCtrlBase
	{
	public:
		CVideoSettingImp(void);
		~CVideoSettingImp(void);
	private:
		VideoSettingState realState;
		VideoSettingState tmpState;
	public:
		void InitState();
		void AppState();
		void EnsureState();
		void ResetState();
		void UpdateState();

		void UpdateWindowRate(int rate_w,int rate_h);   //更新窗口比例 单做 这个函数

		virtual unsigned int	GetAliaxLevel();
		virtual unsigned int	GetShadowLevel();
		virtual unsigned int	GetTerrainLevel();
		virtual unsigned int	GetWaterLevel();
		virtual unsigned int	GetTexLevel();
		virtual unsigned int	GetEffectLevel();
		virtual bool			GetIsRefract();
		virtual bool			GetIsBloom();
		virtual unsigned int	GetIsSyn();
		virtual int				GetWindowWidth();
		virtual int				GetWindowHeight();
		virtual int				GetFrameBorder();

		virtual void			SetAliaxLevel(unsigned int level);
		virtual void			SetShadowLevel(unsigned int level);
		virtual void			SetTerrainLevel(unsigned int level);
		virtual void			SetWaterLevel(unsigned int level);
		virtual void			SetTexLevel(unsigned int level);
		virtual void			SetEffectLevel(unsigned int level);
		virtual void			SetIsRefract(bool enable);
		virtual void			SetIsBloom(bool enable);
		virtual void			SetIsSyn(unsigned int level);
		virtual void			SetWindowWidth(int width);
		virtual void			SetWindowHeight(int height);
		virtual void			SetFrameBorder(int border);

		virtual bool _ProcMsg(const CMsg& msg );
	};

}

