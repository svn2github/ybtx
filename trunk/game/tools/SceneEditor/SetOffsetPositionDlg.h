#pragma once
#include "Resource.h"
#include "DataType.h"

// CSetOffsetPositionDlg dialog

class CSetOffsetPositionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetOffsetPositionDlg)

public:
	CSetOffsetPositionDlg(float x, float y, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetOffsetPositionDlg();

	void GetOffsetPos(FPos & pos);
	void UpdatePos();
// Dialog Data
	enum { IDD = IDD_SET_OFFSET_POSITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	float m_fOffsetX;
	float m_fOffsetY;
public:
	virtual BOOL OnInitDialog();
};
