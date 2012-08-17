#pragma once
//------------------------------------------------------------------------------
/**
@class CGuiHandler

GUI与特效库之间的中介.

(C) 2009 ThreeOGCMan
*/
#include "GUIBase.h"
#include "TRefPointer.h"
#include "CSceneMallocObject.h"

//------------------------------------------------------------------------------
namespace sqr
{
	class CWnd;
	class IEffect;
	class IEffectManager;
	class IEffectGroup;
	class CRenderGroup;
	SmartPointer(CViewport);
	struct CGuiEffect : public SQRGuiMallocObject
	{
		CGuiEffect( uint endTime, IEffect* pEffect, EEffectRun eEffrun = eEffRun_Normal) 
				  : m_uEndTime(endTime),m_pEffect(pEffect), m_eEffRunMode(eEffrun){};
		uint		m_uEndTime;
		EEffectRun	m_eEffRunMode;
		IEffect*	m_pEffect;
	};

	class CGuiHandler : public IGUIHandler
	{
	public:
		CGuiHandler(IEffectManager* manager, const string& fxPath);
		~CGuiHandler();
		void OnEnableIme( bool b );
		/// 窗口大小改变
		void  OnWndSized(const CFRect& rect);

		/// 播放特效
		void  OnPlayFx( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
		void* OnPlayFxIme( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
		/// 指定特效路径
		void  OnPlayFxEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
		void* OnPlayFxImeEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
		/// 删除特效
		void  OnClearFx( SQRWnd* pWnd, const char* fxName );
		void  OnClearFx( SQRWnd* pWnd );
		void  OnClearImeFx( SQRWnd* pWnd, const char* fxName );
		/// 更新
		void OnUpdateFx(uint curTime, float zoom);
		/// 特定更新
		bool OnRenderFx( EffectKey& key, uint curTime, float zoom );

	private:
		typedef UIMap<EffectKey, CGuiEffect> EffectMap;
	private:
		IEffectManager*		effectManager;
		EffectMap			effectMap;
		EffectMap			ImeEffectMap;

		CRenderGroup*		renderGroup;
		CViewportPtr		viewport;
		SString				m_fxDefPath;
	};

}// namespace sqr
