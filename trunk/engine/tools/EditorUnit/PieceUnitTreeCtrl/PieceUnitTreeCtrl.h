#pragma once
#include "CEditorCore.h"

namespace sqr
{
	// CPieceUnitTreeCtrl
	class CPieceUnitTreeCtrl : public CTreeCtrl
	{
		DECLARE_DYNAMIC(CPieceUnitTreeCtrl)

		CImageList m_Image;

	public:
		CEdit m_zBais;
		int m_nCount;
		list<HTREEITEM> m_Node;//保存相同样式RS名字的不同piece
		CString m_NodeText;
		CString m_CheckNodeInfo;
	public:
		int		AddPieceUnit( CEditPiece* Piece );
		int		AddPiece( HTREEITEM hPieceItem, CEditPiece* Piece );
		int		AddPieceRS( HTREEITEM hRSItem, CEditPiece* Piece );
		int		AddPieceVerAni( HTREEITEM hVAItem, CEditPiece* Piece );
		int		DeletePieceUnit( CEditPiece* Piece );
		int		CreateRS( CString FileName = "" );
		void	CreateLevel( float Dist, float FacePercent, float DeleteMaxLen, int MinTris );
		void	AddVerAni( CString VerAniName, CEditPieceGroup* PieceGroup );
		bool	CheckPieceGroup();
		void    FindNodeName(HTREEITEM hItem);

		void	TreeNodeVisit(HTREEITEM hItem);//遍历树中每个节点

		//CSoftLevel* GetSelectSoftLevel();
		CMesh*		GetSelectMesh();
		//CSoftClothLevel* GetSelectSoftClothLevel();

	protected:
		DECLARE_MESSAGE_MAP()
		virtual void PreSubclassWindow();
	public:
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

		afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnTimer(UINT nIDEvent);
	protected:
		UINT          m_TimerTicks;      //处理滚动的定时器所经过的时间
		UINT          m_nScrollTimerID;  //处理滚动的定时器
		CPoint        m_HoverPoint;      //鼠标位置
		UINT          m_nHoverTimerID;   //鼠标敏感定时器
		DWORD         m_dwDragStart;     //按下鼠标左键那一刻的时间
		BOOL          m_bDragging;       //标识是否正在拖动过程中
		CImageList*   m_pDragImage;      //拖动时显示的图象列表
		HTREEITEM     m_hItemDragS;      //被拖动的标签
		HTREEITEM     m_hItemDragD;      //接受拖动的标签
		int			  m_nImage;
	private:
		HTREEITEM CopyBranch(HTREEITEM htiBranch,HTREEITEM htiNewParent,HTREEITEM htiAfter);
		HTREEITEM CopyItem(HTREEITEM hItem,HTREEITEM htiNewParent,HTREEITEM htiAfter);
	};
}