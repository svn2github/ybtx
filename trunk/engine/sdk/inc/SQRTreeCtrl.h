#pragma once
#include "SQRScrollBar.h"

namespace sqr
{
	struct TreeNode;
	typedef TreeNode* HTREENODE;

	class GUI_API SQRTreeCtrl : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRTreeCtrl )

		bool NodeWalk( uint32 uMsg, CFPos Point );
		void ResetScrollBar();

	protected:
		friend struct TreeNode;

		HANDLE m_hTree;

		WND_IMAGE_LIST			m_CheckMouseOverImage;
		WND_IMAGE_LIST			m_CheckClickDownImage;
		WND_IMAGE_LIST          m_CheckEnableImage;
		WND_IMAGE_LIST          m_CheckDisableImage;
		WND_IMAGE_LIST			m_UncheckMouseOverImage;
		WND_IMAGE_LIST			m_UncheckClickDownImage;
		WND_IMAGE_LIST          m_UncheckEnableImage;
		WND_IMAGE_LIST          m_UncheckDisableImage;

		uint32					m_CheckMouseOverColor;
		uint32					m_CheckClickDownColor;
		uint32			        m_CheckEnableColor;
		uint32			        m_CheckDisableColor;
		uint32					m_UncheckMouseOverColor;
		uint32					m_UncheckClickDownColor;
		uint32			        m_UncheckEnableColor;
		uint32			        m_UncheckDisableColor;

		uint32					m_CheckMouseOverBcColor;
		uint32					m_CheckClickDownBcColor;
		uint32			        m_CheckEnableBcColor;
		uint32			        m_CheckDisableBcColor;
		uint32					m_UncheckMouseOverBcColor;
		uint32					m_UncheckClickDownBcColor;
		uint32			        m_UncheckEnableBcColor;
		uint32			        m_UncheckDisableBcColor;

		SQRScrollBar				m_ScrollBar;
		HTREENODE				m_pCurrentNode;

		//本窗口处理消息的函数
		virtual void OnLButtonDblClk( uint32 nFlags, int32 x, int32 y );
		virtual void OnLButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void OnRButtonDblClk( uint32 nFlags, int32 x, int32 y );
		virtual void OnRButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		virtual void OnNodeExpand( bool bExpand ) {};
		virtual void OnLClickNode( HTREENODE pNode ) {};
		virtual void OnLDBClickNode( HTREENODE pNode )
		{
			ExpandNode( pNode, !IsExpand( pNode ) );
		};;
		virtual void OnRClickNode( HTREENODE pNode ) {};
		virtual void OnRDBClickNode( HTREENODE pNode ) {};

		virtual void DrawWndBackground();
		virtual void DrawNode( HTREENODE pNode, CFRect rtNode, uint32 uTextColor, uint32 uBackColor );
		virtual void DrawNodeHeadImage( HTREENODE pNode, CFRect& rtImage );
		virtual void DrawNodeTailImage( HTREENODE pNode, CFRect& rtImage );
		virtual void DrawNodeText( HTREENODE pNode, CFRect& rtText, uint32 uTextColor, uint32 uBackColor );
	public:
		SQRTreeCtrl(void);
		virtual ~SQRTreeCtrl(void);

		virtual int32 DestroyWnd();

		HTREENODE				GetCurrentNode()
		{
			return m_pCurrentNode;
		}

		virtual void			ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void			CreatWndTex();			// 节省资源模式下，窗口打开使用

		virtual int32	    	Create( const WndCreateParam& param );
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		virtual uint32			GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual uint32			GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void			SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );

		virtual SQRWnd*			GetResWnd( int32 nCtrlMask );
		int32					GetShowNodeCount();

		//操作函数
		HTREENODE		InsertNode( HTREENODE pParent, const char* szText,
			const WND_IMAGE_LIST* pHeadImage = NULL, const WND_IMAGE_LIST* pTailImage = NULL,
			uint32 uAscendData = 0, uint32 uParam = 0 );
		void			DeleteNode( HTREENODE pNode );

		//排序，对根节点排序
		void			Sort(HTREENODE pParent = NULL);
		void			DeleteAll();

		void			ExpandNode( HTREENODE pNode, bool bExpand );
		bool			IsExpand( HTREENODE pNode );
		void			GroupNode( HTREENODE pNode, bool bGroup );
		bool			IsGroup( HTREENODE pNode );
		void			SelectNode( HTREENODE pNode, bool bSelect );
		void			SelectNodeNone();
		bool			IsSelect( HTREENODE pNode );
		void			ScrollToNode( HTREENODE pNode );

		void			SetNodeText( HTREENODE pNode, const char* szText );
		const char*		GetNodeText( HTREENODE pNode );
		void			SetNodeImage( HTREENODE pNode, const WND_IMAGE_LIST* pHeadImage, const WND_IMAGE_LIST* pTailImage );
		void			GetNodeImage( HTREENODE pNode, WND_IMAGE_LIST& HeadImage, WND_IMAGE_LIST& TailImage );
		void			SetNodeAscendData( HTREENODE pNode, uint32 uAscendData );
		uint32			GetNodeAscendData( HTREENODE pNode );
		void			SetNodeParam( HTREENODE pNode, uint32 uParam );
		uint32			GetNodeParam( HTREENODE pNode );

		HTREENODE		GetNodeParent( HTREENODE pNode );
		uint32			GetChildNodeCount( HTREENODE pNode );
		HTREENODE		GetFirstChildNode( HTREENODE pNode );
		HTREENODE		GetNextChildNode( HTREENODE pNode );


		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);
		/// @brief 得到给定字符串对应的节点
		/// @param text 要查找的的字符串
		/// @param pNode 在该节点的子节点（不包括该节点）下递归查找，默认为NULL，表示在整棵树中搜索
		/// @return 找打的节点句柄，失败返回NULL
		HTREENODE		GetNodeByText(const char* text, HTREENODE pNode = NULL) const;

	private:
		UIString m_utf8Str;
	};

}