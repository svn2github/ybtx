#pragma once


// CFILE

class CFILE : public CFileDialog
{
	DECLARE_DYNAMIC(CFILE)

public:
	CFILE(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CFILE();

protected:
	DECLARE_MESSAGE_MAP()
};


void ForceDeleteFile(const string& strFile);
void WriteToXml(string& strXml, vector<bool>& vecType,string& FileName);
void GetColType(string& strFileName,vector<bool>& vecType);