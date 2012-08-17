// MainFrm.h : CMainFrame 类的接口
//


#pragma once
class CMainFrame : public CFrameWnd
{	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:
	CView* GetShowView(){ return m_DrawView; }

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();

protected:  // 控件条嵌入成员
	CStatusBar   m_wndStatusBar;
	CToolBar     m_wndToolBar;
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;

	int         m_xSize;
	int         m_ySize;

	CView*      m_DrawView;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnAlphaGen();
};


