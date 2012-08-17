// TextureEditorView.h : CTextureEditorView 类的接口
//

#pragma once


class CTextureEditorView : public CView, UpdateRes
{
protected: // 仅从序列化创建
	CTextureEditorView();
	DECLARE_DYNCREATE(CTextureEditorView)

// 属性
public:
	CTextureEditorDoc* CTextureEditorView::GetDocument() const
   { return reinterpret_cast<CTextureEditorDoc*>(m_pDocument); }

   CD3d       m_d3d;
   CMenu      m_Menu;
   CWnd       m_show;
   CPoint     m_Start;
   CPoint     m_IniP;
   LPVERBUF   m_pVB;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC){};  // 重写以绘制该视图
	void Draw();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT RestoreRes();
	virtual LRESULT ReleaseRes();
protected:

// 实现
public:
	virtual ~CTextureEditorView();

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
};

