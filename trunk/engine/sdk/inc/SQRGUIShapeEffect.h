#pragma once
#include "SQRGUIEffectBase.h"
#include "SQRWnd.h"

// 形变效果，操做的应改是显示效果
// 在形变过成中，再操做控制形状是十分危险的
// 这个怎么处理啊
namespace sqr
{
	class SQRGUIShapeEffect : public SQRGUIEffectBase
	{
	public:
		SQRGUIShapeEffect(SQRWnd* wnd);
		~SQRGUIShapeEffect();

		virtual void		ExecuteEffect();
		virtual EGUIEffect	GetEffectType();

		void				SetShapeEffect(int32 runtime, CFRect src, CFRect des);
		virtual void		EndEffect();
	private:
		void				_SetShape(float percent);
		void				_EndShape();
	private:
		SQRWnd*			m_wnd;
		
		DWORD			m_systime;
		INT32			m_runtime;

		CFRect			m_souRt;
		CFRect			m_desRt;
		CFPos			m_souPos;
		CFPos			m_desPos;
		CFRect			m_wndRt;

		float			m_wndZoom;
	};
}