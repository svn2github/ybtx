// MainFrm.h : CMainFrame 类的接口
//
#pragma once

#include "EditPan.h"
#include "SetView.h"

using namespace bg;
using namespace bc;

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

private:
	CWnd * m_worldView; // show the world
	CResSetView m_resSetView;

public:
	CResSetView * GetResSetView() { return &m_resSetView; }
	void InitMapEditor(DWORD dwWidth, DWORD dwDepth);
	CWnd * GetWorldView() { return m_worldView; }
	
	RECT GetBackBufferRect()
	{
		RECT rcMainFrame;
		RECT rcToolBar;
		m_wndToolBar.GetClientRect(&rcToolBar); 
		this->GetClientRect(&rcMainFrame);
		rcMainFrame.top += rcToolBar.bottom;
		return rcMainFrame;
	}
	
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
public:
	virtual ~CMainFrame();

	void SetHide()
	{
		static bool b = false;
		static bool first = true; 

		b = !b;

		static RECT rcToolBar;
		static RECT rcMainFrame;
		static RECT rcWorldViewOld;
		
		if ( first )
		{
			m_wndToolBar.GetClientRect(&rcToolBar); 
			this->GetClientRect(&rcMainFrame);
			m_worldView->GetWindowRect(&rcWorldViewOld);
			rcMainFrame.top += rcToolBar.bottom;
			rcWorldViewOld.top = 0;
			rcWorldViewOld.top += rcToolBar.bottom;
			first = false;
		}
		
		if ( b )
		{
			m_worldView->SetWindowPos(NULL,rcMainFrame.left,rcMainFrame.top,rcMainFrame.right,rcMainFrame.bottom,NULL);
		}
		else
		{
			m_worldView->SetWindowPos(NULL,rcWorldViewOld.left,rcWorldViewOld.top,rcWorldViewOld.right,rcWorldViewOld.bottom,NULL);
		}
	}

protected:  
	CStatusBar  m_wndStatusBar;
	CToolBar	m_wndToolBar;
	CDialogBar  m_wndEditBar;
	CEditPan    m_wndEditPan;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnViewHide();
	afx_msg void OnExportasclient();

	void UpdateCamera();

	afx_msg void OnExportasserver();
	afx_msg void OnClose();
	afx_msg void OnSv();
};