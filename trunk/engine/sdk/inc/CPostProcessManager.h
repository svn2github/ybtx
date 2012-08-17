#pragma once
//------------------------------------------------------------------------------
/**
	@class CPostProcessManager

	后处理管理器.
	管理所有效果的开关和RenderTarget的生命周期.
	各个效果之间可以共享临时的RenderTarget, 以节省资源占用
	效果之间的相对顺序不可改变
	禁用的效果需要把pass清空, 释放RTT的引用
*/
#include "TSingleton.h"
#include "TRefPointer.h"
#include "RTTFormat.h"
#include "CShaderConstParams.h"


namespace sqr
{
	enum PostProcessShaderParam
	{
		// ---- VS PARAMS
		PPSP_INV_VIEW_SIZE_VS			= ACT_CUSTOM_PARAMS_LIST,
		
		// ---- PS PARAMS
		PPSP_INV_VIEW_SIZE_PS			= 0,
		PPSP_BRIGHT_PASS_THRESHOLD_PS	= 1,
		PPSP_EXPOSURE_PS				= 2,
		PPSP_SCALER_PS					= 3,
		PPSP_HBLOOM_WEIGHTS_PS			= 4,
		PPSP_HBLOOM_OFFSETS_PS			= 7,
		PPSP_VBLOOM_WEIGHTS_PS			= 10,
		PPSP_VBLOOM_OFFSETS_PS			= 13,
		PPSP_DOWN_SAMPLE_OFFSETS_PS		= 16, // use 16 slots, so next param will be "32"
	};

	SmartPointer(CRenderTexture);
	SmartPointer(CPostEffect);
	SmartPointer(CPostEffectPass);
	SmartPointer(CPostEffectLoader);
	class CRenderGroup;

	class CPostProcessManager 
		: public Singleton<CPostProcessManager>
		, public CGraphicMallocObject
	{
		friend class Singleton<CPostProcessManager>;	
	public:
		CPostProcessManager();
		~CPostProcessManager();
		static const GString ConfigFile;

	public:
		/// 初始化后处理效果
		virtual void Setup();
		/// 释放资源
		virtual void Discard();

		/// 返回指定效果是否启用
		virtual bool IsEffectEnabled(const string& name);
		/// 启用/禁用指定效果
		virtual void SetEffectEnabled(const string& name, bool enable);

		/// 渲染到默认的RenderTarget
		//void Begin(CRenderGroup* pRenderGroup);
		/// 进行后处理
		virtual void DoPostProcess();

		/// 返回默认的RTT
		virtual CRenderTexture* GetDefaultRenderTarget();

	private:
		/// 是否有后处理效果启用
		bool HasPostEffectEnabled() const;
		/// 更新pass链
		void UpdatePassChain();
	//private:
	protected:
		GVector<CPostEffectPtr>			postEffects;
		CPostEffectPassPtr				nullPass;		///< 包装default RTT用的
		GSet<GString>					enabledEffects;
		CRenderTexture*					defaultRTT;
		CPostEffectLoaderPtr			effectLoader;
	};

	//------------------------------------------------------------------------------
	inline bool
	CPostProcessManager::HasPostEffectEnabled() const
	{
		return (!this->enabledEffects.empty());
	}

	//------------------------------------------------------------------------------
	inline CRenderTexture*
	CPostProcessManager::GetDefaultRenderTarget()
	{
		return this->defaultRTT;
	}

	//------------------------------------------------------------------------------
	inline bool
	CPostProcessManager::IsEffectEnabled( const string& name )
	{
		return (this->enabledEffects.end() != this->enabledEffects.find(name.c_str()));
	}

}// namespace sqr
