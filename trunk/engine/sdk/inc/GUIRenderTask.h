#pragma once
#include "SQRGuiMallocObject.h"
#include "SQRFlashLayer.h"


namespace sqr
{
	class GUIRenderTask : public SQRGuiMallocObject
	{
	public:
		GUIRenderTask(){}
		virtual ~GUIRenderTask(){}

		virtual void RenderTask() = 0;
	};

	// 闪烁材质的渲染对象
	class FLASHRenderTask : public GUIRenderTask
	{
	public:
		FLASHRenderTask();
		virtual ~FLASHRenderTask();

		virtual void RenderTask();
		virtual void AddFlashTask(UIString flashTex, const CFRect& rt, SQRWnd* pChildWnd, uint32 ct, float v, 
								  float lx, float ty ,float rx, float by);

		virtual void AddFlashTask(FlashWndInfo* flash, SQRWnd* pwnd, uint32 st);
		virtual void DelFlashTaskAll(SQRWnd* pWnd);	// 删除窗体上的所有闪烁效果
		virtual void DelFlashTask(SQRWnd* pWnd, UIString flash); // 删除窗体上的特定闪烁效果
		virtual bool HasCountFlash();
	
	private:
		UIVector<SQRFlashLayer*>	m_FlashWnd[2];		// 0常规队列，1状态队列
	};

	// 文字渲染对象
	class TextRenderTask : public GUIRenderTask
	{
	public:
		TextRenderTask();
		virtual ~TextRenderTask();

		virtual void	RenderTask();
		void			SetFlushDlg(SQRWnd* pDlg);
	private:
		SQRWnd*			m_pDlg;
	};

	inline void TextRenderTask::SetFlushDlg(SQRWnd* pDlg)
	{
		m_pDlg = pDlg;
	}

	// 特效渲染对象
	class FxRenderTask : public GUIRenderTask
	{
	public:
		struct _FxTask
		{
			SQRWnd* pWnd;
			uint	curTime;
			float	zoom;
			_FxTask(SQRWnd* wnd, uint ct, float zm);
		};
		FxRenderTask();
		virtual ~FxRenderTask();

		virtual void	RenderTask();
		void			PushRenderStack(SQRWnd* pWnd, uint curTime, float zoom);
	private:
		UIVector<_FxTask>	m_FxTaskVec;
	};
}
