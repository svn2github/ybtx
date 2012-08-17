
#pragma once
#include "SQRControl.h"

namespace sqr
{
	//可以让字体自己移动的窗口
	class GUI_API SQRWndOffSet : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRWndOffSet )
	public:
		SQRWndOffSet();
		~SQRWndOffSet();

		//设置窗口内容
		virtual void            SetWndText( const char* str );

		//得到窗口内容
		virtual const char*	GetWndText();

		//重载背景窗口
		virtual void            DrawWndBackground();

		//窗口是否显示
		virtual void			ShowWnd( bool bShow );

		virtual void            DrawText( const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect );

		//设置移动的速度
		void		            SetStep(INT32 time, FLOAT mStep);

		static bool				IsHasEventState(EventStateMask Mask);
		virtual bool			ObjHasEventState(EventStateMask Mask);
	private:
		//控件的内部自己处理
		virtual void            OnProcess();

	private:
		//保留原来的字符串
		UIString  resstring;
		DWORD   mSteptimer;
		DWORD   mBeginTimer;
		FLOAT   mStepResult;
		FLOAT   mStepOver;
		FLOAT   mStep;
	};
}