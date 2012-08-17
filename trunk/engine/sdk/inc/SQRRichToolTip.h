#pragma once
#include "TipHandler.h"
#include "BaseHelper.h"
#include "GUIBase.h"

namespace sqr
{
	class TipHandler;
	class SQRRichToolTip
		: public virtual CDynamicObject
	{
		struct PosRect
		{
			CFRect OrgRect;
			CFRect RejRect;
			bool   RePos;
			PosRect(){}
		};
	public:
		SQRRichToolTip();
		~SQRRichToolTip();

		WND_IMAGE_LIST	m_Image;
		uint32			m_uStyle;
		CFPos			m_DrawPos;
		CFPos			m_TempPos;
		bool			m_bTexInit;

		virtual bool		  AddChildToolTip(void);
		virtual SQRRichToolTip* GetChildToolTip(void);
		virtual void		  SetToolTipStyle(uint32 uStyle);
		void				  SetParentWnd(SQRWnd* wnd);
	public:
		SQRWnd*				  PushTipsWnd(const char* szWindowNameInResFile);
		static void			  PrepareToolTip( SQRWnd* pWnd );
		static void			  DrawToolTip( SQRWnd* pWnd );
		void				  OnTipShow();								// 显示tooltip
		void				  OnTipHide();								// 隐藏tooltip
		bool				  CanBeShow();
		virtual	void		  ClearChildToolTip(void);
		void				  PushData();
		SQRWnd*				  GetParentWnd();
		void				  SetTipHandler( TipHandler* handler );
		void				  ClearTipHandler();

	protected:
		void				  Draw( SQRWnd* Wnd );
		float				  MaxHeight();
		float				  MaxWidth();
		void				  LayoutTips(CFPos& pos);
		void				  RejustPos(float x, float y);
		bool				  CheckNeedRepos(const CFRect& r1, const CFRect& r2);

	private:
		SQRRichToolTip*		  m_pChildToolTip;
		UIList<SQRWnd*>		  m_ChildList;			// TOOLTIP添加窗口列表 // 自动布局窗口
		UIList<PosRect*>	  m_RectList;
		SQRWnd*				  m_pParent;			// TOOLTOP挂载到的父窗体

		TipHandler*			  m_pTipHandler;
	};

}