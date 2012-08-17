#pragma once
#include "afxcmn.h"

//////////////////////////////////////////////////////////////////////////

class CGroundView : public CFormView
{	
public:
	CGroundView();           // 动态创建所使用的受保护的构造函数
	virtual ~CGroundView();
	
public:
	enum { IDD = IDD_GROUND };
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
	sc::COperator::EEDIT_STATE m_eEditState;
	CSliderCtrl m_sliderBrushStrength;
	CSliderCtrl m_heightStrength;
	
	CImageList m_images;
	CTreeCtrl m_textureTree;
	
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PreSubclassWindow();
	
public:
	afx_msg void OnBnClickedLayerone();
	afx_msg void OnBnClickedLayertwo();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnBnClickedPhysicheight();
	afx_msg void OnBnClickedLogicheight();
	afx_msg void OnBnClickedLinear();
	afx_msg void OnBnClickedGauss();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedX();
	afx_msg void OnBnClickedZ();
	afx_msg void OnBnClickedBoth();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFileNew();

private:
	void InitTextures();
public:
	afx_msg void OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult);
};


