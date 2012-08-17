#pragma once

#include "SQRControl.h"
#include "SQRScrollBar.h"

namespace sqr
{
	class SQRWnd;

	/////////////////////////////////////////////////////////
	// class SQRListCtrl
	/////////////////////////////////////////////////////////
	class GUI_API SQRListCtrl : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRListCtrl )

			HANDLE	m_hList;
	public:
		SQRListCtrl(void);
		virtual ~SQRListCtrl(void);


	public:
		virtual int32	  			Create( const WndCreateParam& param );
		virtual int32				DestroyWnd();
		/// @brief 绘制ListCtrl item间的格子
		virtual void				DrawAfterChildren();
		virtual WND_IMAGE_LIST*		GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32				GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void				SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32				GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void				SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual void				DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY);

		virtual SQRWnd*				GetResWnd( int32 nCtrlMask );
		virtual void				OnLButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void				OnRButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void				OnLButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void				OnRButtonClick( uint32 nFlags, int32 x, int32 y );


		static bool					IsHasEventState(EventStateMask Mask);
		virtual bool				ObjHasEventState(EventStateMask Mask);

	protected:
		virtual void				OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		virtual void				OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 wParam, uint32 lParam );

	public:
		void						SetStyle( uint32 Style );
		void						SetWndRect( const CFRect& rt );
		void						ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void				_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void				_SetWndRect( const CFRect& rt );
	public:
		int32						InsertColumn( int32 nCol, float fWidth );
		int32						InsertItem( int32 nItem, float fHeight = -1 );
		int32						InsertItemFromRes( int32 nItem, const char* szRes );
		void						SetItemHeight( float fHeight, int32 nItem = -1 );
		SQRWnd*					    GetSubItem( int32 nItem, int32 nSubItem );
		int32						GetSelectItem( int32 nPreItem = -1 );
		int32						GetSelectSubItem( int32 nItem, int32 nPreSubItem = -1 );
		void						DeleteAllItem();
		void						DeleteItem( int32 nItem );
		void						DeleteSubItem(int row, int col);
		int32						GetItemCount();
		int32						GetSubItemCount();
		void						SelectItem( int32 nItem );
		void						ResetItem(void)
		{
			ResetItemRect();
		}
		void						UpdateItemImage();
		void						SetItemHighLight(int32 index);
		int32						GetHighLightIndex();
		/// @brief 滚动条是否已经滚动到最下端
		bool						Isbottom(); 
		bool						Istop();
		void						SetVerticalPos(int32 nPos);
		int32						GetVerticalPos();

		virtual void				ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void				CreatWndTex();			// 节省资源模式下，窗口打开使用

		float						GetWorkAreaHeight();
		float						GetWorkAreaWidth();
		CFRect						GetWorkAreaRect();
		void						GetWorkAreaRectEx(CFRect& rect);
		float						GetWorkAreaOrgHeight();
		float						GetWorkAreaOrgWidth();
		CFRect						GetWorkAreaOrgRect();
		void						GetWorkAreaOrgRectEx(CFRect& rect);
		CFPos						GetWorkAreaOffset();

		virtual void				GetCutAreaRect( CFRect& rt );

		SQRScrollBar*				GetVerticalScroll();
	protected:
		void						ResetItemRect( bool bSetScroll = true, bool refreshbak = false );

	protected:
		WND_IMAGE_LIST	  			m_ItemSelImage;
		WND_IMAGE_LIST				m_ItemUnselImage;
		WND_IMAGE_LIST				m_ItemMouseOverImage;

		uint32						m_ItemSelColor;
		uint32						m_ItemUnselColor;
		uint32						m_ItemMouseOverColor;
		uint32						m_ItemSelBkColor;
		uint32						m_ItemUnselBkColor;
		uint32						m_ItemMouseOverBkColor;

		SQRScrollBar				m_sbHorizon	;    //水平滚动条
		SQRScrollBar				m_sbVertical;   //竖直滚动条
		int8						m_keyboardCount;
		int8						m_mouseCount;
		int32						m_uLastTimeHighLightX;
		int32						m_uLastTimeHighLightY;
	};
}
