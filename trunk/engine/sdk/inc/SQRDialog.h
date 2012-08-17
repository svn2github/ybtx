#pragma once
#include "SQRWnd.h"
#include "GUIRenderTask.h"

namespace sqr
{
	class SQRDialog 
		: public SQRWnd
		, public GUITextureHelper
	{
		DYNAMIC_DECLARE_WND( SQRDialog )
	public:
		SQRDialog();
		virtual ~SQRDialog();

		virtual int32	CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd );
		virtual int32	CreateFromResEx( const char* szWindowNameInResFile, SQRWnd* ParentWnd, bool merged, bool bAsyn = true );
		virtual void	ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void	SetWndRect( const CFRect& rt );

		virtual void	_ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void	_SetWndRect( const CFRect& rt );
		virtual int32	DestroyWnd();
	public:
		virtual bool	IsFlushDialog();

		virtual void	DrawAfterChildren();
		virtual void	ShowWnd( bool bShow );
		virtual void	OnProcess();
		virtual void	DrawWndBackground();
		
		FLASHRenderTask*	GetFlashRenderTask(bool toNew = false);
		TextRenderTask*		GetTextRenderTask(bool toNew = false);
		FxRenderTask*		GetFxRenderTask(bool toNew = false);

		//画接口
		virtual void		Draw();
		virtual void		InsertTexture(ITexture* pTexture);
		virtual bool		IsAsynLoad();

	private:
		void		  DealWndTex();
		int32		  _CreateFromRes( const char* szWindowNameInResFile, SQRWnd* ParentWnd );
		GUIRenderTask*_GetRenderTask( uint32 et );
	private:
		GUIRenderTask*	m_DlgRenderTask[EDLG_FLUSH_COUNT];	// dialog级别的渲染任务
		typedef UISet<ITexture*> TexPool;
		TexPool			m_TexMap;
		bool			m_bWaitHide;
		bool			m_bOptimize;
		bool			m_bFlashed;
		bool			m_bAutoFlushWnd;
		bool			m_bAsynLoad;
		bool			m_bCanDraw;
		bool			m_bValid;
	};

	class CCppMouseHook 
		:public SQRDialog
		,public IHook
	{
	public:
		CCppMouseHook();
		~CCppMouseHook();
	};
}