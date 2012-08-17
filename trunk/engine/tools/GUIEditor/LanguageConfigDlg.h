#pragma once
#include "afxwin.h"

class CLanguageConfigEdit;

class CLanguageConfigDlg : public ::CDialog
{
	DECLARE_DYNAMIC(CLanguageConfigDlg)

public:
	CLanguageConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLanguageConfigDlg();

	virtual BOOL	OnInitDialog();
	virtual INT_PTR DoModal();

// Dialog Data
	enum { IDD = IDD_LANGUAGE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void		 loadConfigFile();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddLanguage();
	afx_msg void OnBnClickedAddFont();
	afx_msg void OnBnClickedGenerateConfigFile();
	afx_msg void OnBnClickedCancelConfigLanguage();
	afx_msg void OnCbnSelChangeLanguageList();

private:

	void		 addLanguage(const string &szLanguage);
	void		 addFont(int32 nLanIndex, const string &szFontFile, 
						 const string &szFontName, const string &szFontSize);
	void		 combAddLanguage(const string &szLanguage);
	void		 combAddFont(int32 id, const string &szFontName, const string &szFontFile);
	void		 updateControl();

public:
	::CComboBox	m_combLanguageList;
	::CComboBox	m_combFontList;
	::CEdit		m_editLanguage;
	::CEdit		m_editFontFile;
	::CEdit		m_editFontName;
	::CEdit		m_editFontSize;
};
