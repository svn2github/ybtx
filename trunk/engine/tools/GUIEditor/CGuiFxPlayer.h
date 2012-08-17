#pragma once
//------------------------------------------------------------------------------
/**
    @class CGuiFxPlayer

	GUI与特效库之间的中介.

    (C) 2009 ThreeOGCMan
*/
#include "GUIBase.h"
#include "TRefPointer.h"

//------------------------------------------------------------------------------
namespace sqr
{
class SQRWnd;
class IEffect;
class IEffectManager;
class IEffectGroup;
class CRenderGroup;
SmartPointer(CViewport);

class CGuiFxPlayer : public IGUIHandler
{
public:
	CGuiFxPlayer(IEffectManager* manager, const string& fxPath);
	~CGuiFxPlayer();
	void OnEnableIme( bool bEnable ) {};
	/// 播放特效
	void OnPlayFx( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
	/// 立即播放特效
	void* OnPlayFxIme( SQRWnd* pWnd, const char* fxName, EEffectRun eEffrun = eEffRun_Normal );
	/// 播放特效
	void  OnPlayFxEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal ){};
	/// 立即播放特效
	void* OnPlayFxImeEx( SQRWnd* pWnd, const char* fxFileName, const char* fxName, EEffectRun eEffrun = eEffRun_Normal ){ return NULL; };
	/// 清理特效
	void OnClearFx( SQRWnd* pWnd, const char* fxName );
	/// 清理特效
	void OnClearFx( SQRWnd* pWnd );
	/// 清理立即特效
	void OnClearImeFx( SQRWnd* pWnd, const char* fxName );

	/// 更新
	void OnUpdateFx(uint curTime, float zoom);
	void OnWndSized(const CFRect& rect) {}
	bool OnRenderFx( EffectKey& key, uint curTime, float zoom );
	void SetBePreView(bool pre);
	
private:
	struct EffectUnit
	{
		EffectUnit()
			: window(NULL) {}
		EffectUnit(IEffect* fx, SQRWnd* wnd)
			: effect(fx), window(wnd) {}
		EffectUnit(IEffect* fx, SQRWnd* wnd, const char* nm)
			: effect(fx), window(wnd) 
		{
			fx_name = (UIString)(nm);
		}
	public:
		IEffect* effect;
		SQRWnd* window;
		UIString fx_name;
	};
private:
	IEffectManager* effectManager;
	IEffectGroup* effectGroup;

	list<EffectUnit> effects;
	list<EffectUnit> effectsIme;

	CRenderGroup* renderGroup;
	CViewportPtr viewport;

	bool	m_PreView;
};

}// namespace sqr
