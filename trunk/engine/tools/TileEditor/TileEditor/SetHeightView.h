#pragma once

//////////////////////////////////////////////////////////////////////////

// CSetHeightView 窗体视图

class CSetHeightView : public CFormView
{	
public:
	CSetHeightView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSetHeightView();
	
public:
	enum { IDD = IDD_HEIGHTSET };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy() { }
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

private:

//	CRadios m_radios;

	DECLARE_MESSAGE_MAP()
protected:
	virtual void PreSubclassWindow();
	
public:
	afx_msg void OnBnClickedGauss();
	afx_msg void OnBnClickedLinear();
};


