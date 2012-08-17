#pragma once


enum EEideModeType
{
	eMT_NORMAL,
	eMT_AREA_FB,
	eMT_DYNAMIC,
	eMT_PATH_EDIT
};

// CModeChoiceDlg dialog

class CModeChoiceDlg : public CDialog
{
	DECLARE_DYNAMIC(CModeChoiceDlg)

public:
	CModeChoiceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModeChoiceDlg();

// Dialog Data
	enum { IDD = IDD_MODE_CHOICE };
	EEideModeType GetMode(){return m_eMode; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
private:
	EEideModeType m_eMode;
public:
	afx_msg void OnBnClickedNormal();
	afx_msg void OnBnClickedAreaFb();
	afx_msg void OnBnClickedDynamic();
	afx_msg void OnBnClickedPathEdit();
};
