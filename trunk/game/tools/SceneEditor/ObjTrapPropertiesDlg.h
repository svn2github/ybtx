#pragma once

// CObjTrapPropertiesDlg dialog

class CObjTrapPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CObjTrapPropertiesDlg)

public:
	CObjTrapPropertiesDlg(const CString& type,const CString& name, const FPos& fPos, const CPoint& dlgPos, CWnd* pParent = NULL);   // standard constructor
	virtual ~CObjTrapPropertiesDlg();
// Dialog Data
	enum { IDD = IDD_OBJ_TRAP_RDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_sType;
	CString m_sName;
	FPos m_fPos;
	CPoint m_dlgPos;
};
