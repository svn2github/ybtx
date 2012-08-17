#pragma once

#define REGPATH "Software\\NewSceneEditor"

class CSEPathMgr
{
private:
	CSEPathMgr(void);
public:
	~CSEPathMgr(void);

	CString GetMapPath();
	CString GetScenePath();
	CString GetAreaPath();
	
	void SaveMapPath(CString strPath);
	void SaveScenePath(CString strPath);
	void SaveAreaPath(CString strPath);
	void SetArtistPath(const CString& strPath);
	CString  GetArtistPath();
	void SetDesignerPath(const CString& strPath);
	CString GetDesignerPath();
	void SetWorkingPath(const CString& strPath);
	CString GetWorkingPath();
private:
	void SetRegistKeyStr(LPCTSTR Name, LPCTSTR str );
	LPCTSTR GetRegistKeyStr(LPCTSTR Name );

	CString m_StrArtistPath;
	CString m_StrDesignerPath;
	CString m_StrWorkingPath;
public:
	static CSEPathMgr& Inst();
};
