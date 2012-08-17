#pragma once


// CGetInputStr dialog

class CGetInputStr : public CDialog
{
	DECLARE_DYNAMIC(CGetInputStr)

	CString m_Title;

public:
	CGetInputStr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGetInputStr();

// Dialog Data
	enum { IDD = IDD_GETINPUTSTR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual CString& DoModal( CString Title = "", CString Init = "" );
	CString m_String;
protected:
	virtual void OnOK();
};
