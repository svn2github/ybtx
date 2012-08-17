
#pragma once
#include "SQRControl.h"
#include "IRenderTime.h"
namespace sqr
{

	class SQRClock;
	class GUI_API SQRClockPart : public SQRControl
	{
		friend class SQRClock;
		DYNAMIC_DECLARE_WND( SQRClockPart )
	public:
		SQRClockPart(void);
		virtual ~SQRClockPart(void);
		virtual void OnProcess();
		void	SetBeFrontColor(bool b);
		void	InitClockPart();
	private:
		//标记
		INT8	mflag;

		//当前移动的长度
		FLOAT	flength;

		//旋转的方向
		bool    bclock;
		//是否为前色
		bool	bfrontcl;		
	};

	class GUI_API SQRClock : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRClock )

	public:
		SQRClock(void);
		virtual ~SQRClock(void);

		//设置时间
		void setTime(int32 dtime);
		void setTimeEx(int32 dtime, IRenderTime* pTime);
		//设置已经运行的时间
		void setRunedTime(int32 runedtime);
		//设置Clock控件透明色
		void SetBackImageColor(uint32 uColor);
		//设置Clock控件覆盖色，和上面的是互斥的
		void SetBeFrontImageColor(bool b);

		//设置显示的位置
		void SetPos(int32 nPos);
		void SetPosEx(int32 nPos, IRenderTime* pTime);
		//设置最大位移
		void SetRange(int32 nRange);

		//设置旋转方向
		void SetDirection(bool Direction);

		virtual SQRWnd*				GetResWnd( int32 nCtrlMask );
		virtual WND_IMAGE_LIST*		GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		//逻辑的计算
		virtual void OnProcess();

		virtual int32	    	Create( const WndCreateParam& param );
		virtual void			OnDestroy();
		static bool				IsHasEventState(EventStateMask Mask);
		virtual bool			ObjHasEventState(EventStateMask Mask);

		virtual void			ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void			CreatWndTex();			// 节省资源模式下，窗口打开使用

		virtual void			_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void			_SetWndRect( const CFRect& rt );
	private:
		//做一个动态的旋转Button
		//开始时间
		DWORD			 mBegintime;
		//已经运行的时间
		DWORD			 mRunedtime;
		DWORD			 mRRunedtime;
		//结束时间
		DWORD			 mEndtiem;

		//标记Clock 还是 progress
		int32            Funcflag;

		//每次移动的步长
		FLOAT			 fStep;

		//标示走到第几个矩型了
		int32            flagRect;

		//每个矩形的长度
		FLOAT			 flength[4];

		//记录时间
		DWORD			 lastRunedTime;

		//一个的长度
		FLOAT			 falllength;

		//当前的长度
		FLOAT			 fNowlength;

		//保存下来位置和图形
		SQRClockPart*	 pProgress[4];
		bool			 bShowState[5];

		IRenderTime*	 m_pTime;
		//每个小控件的CFRect
		CFRect			 m_Rect;
		WND_IMAGE_LIST	 m_BackImage;

		//是否是由亮到暗
		bool			 m_bFrontColor;
	};
}