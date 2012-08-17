#include "StdAfx.h"
#include "ScriptMgr.h"
#include "SEPathMgr.h"

CSEPathMgr::CSEPathMgr(void)
{
}

CSEPathMgr::~CSEPathMgr(void)
{
}


CSEPathMgr& CSEPathMgr::Inst()
{
	static CSEPathMgr instance;
	return instance;
}


void CSEPathMgr::SetRegistKeyStr(LPCTSTR Name, LPCTSTR str )
{
	HKEY hKey = NULL;
	RegCreateKey( HKEY_LOCAL_MACHINE, REGPATH, &hKey );
	RegSetValueEx( hKey, Name, NULL, REG_SZ, (BYTE*)str, (DWORD)strlen( str ) );
	RegCloseKey(hKey);
}

LPCTSTR CSEPathMgr::GetRegistKeyStr(LPCTSTR Name )
{
	HKEY hKey = NULL;
	DWORD Len = 1024;
	DWORD Type = REG_SZ;
	static BYTE  Buf[1024];
	RegCreateKey( HKEY_LOCAL_MACHINE, REGPATH, &hKey );
	if( ERROR_SUCCESS != RegQueryValueEx( hKey, Name, NULL, &Type, Buf, &Len ) )
		Buf[0] = 0;
	RegCloseKey(hKey);
	return (char*)Buf;
}

CString CSEPathMgr::GetMapPath()
{
	return CString(GetRegistKeyStr("MapPath"));
}

CString CSEPathMgr::GetScenePath()
{
	return CString(GetRegistKeyStr("ScenePath"));
}

CString CSEPathMgr::GetAreaPath()
{
	return CString(GetRegistKeyStr("AreaPath"));
}

void CSEPathMgr::SaveMapPath(CString strPath)
{
	SetRegistKeyStr("MapPath", strPath.GetBuffer());
}

void CSEPathMgr::SaveScenePath(CString strPath)
{
	SetRegistKeyStr("ScenePath", strPath.GetBuffer());
}

void CSEPathMgr::SaveAreaPath(CString strPath)
{
	SetRegistKeyStr("AreaPath", strPath.GetBuffer());
}


void CSEPathMgr::SetArtistPath(const CString& strPath)
{
	m_StrArtistPath = strPath;
}

CString CSEPathMgr::GetArtistPath()
{
	return m_StrArtistPath;
}

void CSEPathMgr::SetDesignerPath(const CString& strPath)
{
	m_StrDesignerPath = strPath;
}

CString CSEPathMgr::GetDesignerPath()
{
	return m_StrDesignerPath;
}

void CSEPathMgr::SetWorkingPath(const CString& strPath)
{
	m_StrWorkingPath = strPath;
}

CString CSEPathMgr::GetWorkingPath()
{
	return m_StrWorkingPath;
}