#pragma once

#include "Brain.h"

// CPropView 窗体视图

class CPropView : public CFormView
{
public:
	CPropView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPropView();

public:
	enum { IDD = IDD_PROPS };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy() { }

private:
	int				m_nControlCnt;
	int				m_nCurPos;
	DWORD			m_dwControlHeight;
	DWORD			m_dwControlWidth;
	CBrain *		m_pBrain;
	
	void GetLabelRect( int nLabelIndex, CRect &rect );
	void GetControlRect( int nControlIndex, CRect &rect );
	
	void			Init();
	void			Term();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnChangeEditBool();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


