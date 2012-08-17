#include "stdafx.h"

#ifdef _WIN32
#include "CFileWatcherThread.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include "ResDll.h"
namespace sqr
{

CFileWatcherThread* CFileWatcherThread::ms_pInst = NULL;

CFileWatcherThread*& CFileWatcherThread::Inst()
{
	static CFileWatcherThread* ms_pInst = NULL;
	return ms_pInst;
}

void CFileWatcherThread::Init(CFileWatcherAdapter* pAdapter, const char* szCfgPath, const wchar_t* szLangPath, const char* szCodePath)
{
	Inst() = new CFileWatcherThread(pAdapter, szCfgPath,szLangPath,szCodePath);
	Inst()->InitAllFileState();
}

void CFileWatcherThread::Unit()
{
	delete Inst();
}

CFileWatcherThread::CFileWatcherThread()
:m_evCancel(NULL)
,hWnd(NULL)
,m_pChangeFileDataVec(NULL)
,m_pLUAFileDataVec(NULL)
,m_pTXTFileDataVec(NULL)
,m_pAdapter(NULL)
{
}

CFileWatcherThread::CFileWatcherThread(CFileWatcherAdapter* pAdapter, const char* szCfgPath, const wchar_t* szLangPath, const char* szCodePath)
:m_evCancel(NULL)
,hWnd(NULL)
,m_pChangeFileDataVec(NULL)
,m_pLUAFileDataVec(NULL)
,m_pTXTFileDataVec(NULL)
,m_pAdapter(pAdapter)
{
	m_strCfgTgtPath = L"";
	utf8_to_utf16(m_strCfgTgtPath, szCfgPath);
	
	m_strCfgLangPath = szLangPath;
	
	m_strCodePath = L"";
	utf8_to_utf16(m_strCodePath, szCodePath);
	
	m_evCancel = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	CreateThread(&m_hThread,0,&FileWatcherThreadProc,this);
}


CFileWatcherThread::~CFileWatcherThread()
{
	if(hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = NULL;
	}
	::SetEvent(m_evCancel);
	JoinThread(&m_hThread);
	CloseHandle(m_evCancel);
	if (m_pChangeFileDataVec)
	{
		m_pChangeFileDataVec->Release();
		m_pChangeFileDataVec = NULL;
	}
	if (m_pLUAFileDataVec)
	{
		delete m_pLUAFileDataVec;
		m_pLUAFileDataVec = NULL;
	}
	if (m_pTXTFileDataVec)
	{
		delete m_pTXTFileDataVec;
		m_pTXTFileDataVec = NULL;
	}
	m_mapFileTime.clear();
	delete m_pAdapter;
}

void CFileWatcherThread::FileWatcherThreadProc(void* pArg)
{
	HANDLE dwHandles[2];
	dwHandles[0] = static_cast<CFileWatcherThread*>(pArg)->m_evCancel;
	if(dwHandles[0] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 0");
		return;
	}
	wstring& wszCfgTgtPath = static_cast<CFileWatcherThread*>(pArg)->GetCfgTgtPath() + wstring(L"/skill");
	string& szCfgTgtPath = utf16_to_gbk(wszCfgTgtPath);
	dwHandles[1] = FindFirstChangeNotification( szCfgTgtPath.c_str(), TRUE,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(dwHandles[1] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 1");
		return;
	}
	/*wstring& wssCodePath = static_cast<CFileWatcherThread*>(pArg)->GetCodePath() + wstring(L"/gas/lua");
	string& szsCodePath = utf16_to_gbk(wssCodePath);
	dwHandles[2] = FindFirstChangeNotification( szsCodePath.c_str(), TRUE,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(dwHandles[2] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 2");
		return;
	}
	wstring& wscCodePath = static_cast<CFileWatcherThread*>(pArg)->GetCodePath() + wstring(L"/gac/lua");
	string& szcCodePath = utf16_to_gbk(wscCodePath);
	dwHandles[3] = FindFirstChangeNotification( szcCodePath.c_str(), TRUE,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(dwHandles[3] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 3");
		return;
	}
	wstring& ws2CodePath = static_cast<CFileWatcherThread*>(pArg)->GetCodePath() + wstring(L"/gac_gas/lua");
	string& sz2CodePath = utf16_to_gbk(ws2CodePath);
	dwHandles[4] = FindFirstChangeNotification( sz2CodePath.c_str(), TRUE,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(dwHandles[4] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 4");
		return;
	}
	string& szCfgLangPath = utf16_to_gbk(static_cast<CFileWatcherThread*>(pArg)->GetCfgLangPath());
	dwHandles[5] = FindFirstChangeNotification( szCfgLangPath.c_str(), TRUE,FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(dwHandles[5] == INVALID_HANDLE_VALUE)
	{
		LogErr("ReloadCfg ThreadProc Err :: handle 5");
		return;
	}*/

	while(true)
	{
		DWORD DRet = WaitForMultipleObjects(2,dwHandles,false,INFINITE);
		switch(DRet)
		{
			case WAIT_OBJECT_0:
				{
					return;
				}
			case WAIT_OBJECT_0 + 1:
				{
					static_cast<CFileWatcherThread*>(pArg)->OnFileChanged(ED_CFG_TGT);
					if(FindNextChangeNotification(dwHandles[1]) == false)
					{
						ostringstream strm;
						strm<<GetLastError();
						LogErr("ReloadCfg FindNextChangeNotification error: 1", (strm.str()).c_str());
						return;
					}
					break;
				}
			/*case WAIT_OBJECT_0 + 2:
				{
					static_cast<CFileWatcherThread*>(pArg)->OnFileChanged(ED_CODE_Gas);
					if(FindNextChangeNotification(dwHandles[2]) == false)
					{
						ostringstream strm;
						strm<<GetLastError();
						LogErr("ReloadCfg FindNextChangeNotification error: 2", (strm.str()).c_str());
						return;
					}
					break;
				}
			case WAIT_OBJECT_0 + 3:
				{
					static_cast<CFileWatcherThread*>(pArg)->OnFileChanged(ED_CODE_Gac);
					if(FindNextChangeNotification(dwHandles[3]) == false)
					{
						ostringstream strm;
						strm<<GetLastError();
						LogErr("ReloadCfg FindNextChangeNotification error: 3", (strm.str()).c_str());
						return;
					}
					break;
				}
			case WAIT_OBJECT_0 + 4:
				{
					static_cast<CFileWatcherThread*>(pArg)->OnFileChanged(ED_CODE_Gac2Gas);
					if(FindNextChangeNotification(dwHandles[4]) == false)
					{
						ostringstream strm;
						strm<<GetLastError();
						LogErr("ReloadCfg FindNextChangeNotification error: 4", (strm.str()).c_str());
						return;
					}
					break;
				}
			case WAIT_OBJECT_0 + 5:
				{
					static_cast<CFileWatcherThread*>(pArg)->OnFileChanged(ED_CFG_LANG);
					if(FindNextChangeNotification(dwHandles[4]) == false)
					{
						ostringstream strm;
						strm<<GetLastError();
						LogErr("ReloadCfg FindNextChangeNotification error: 5", (strm.str()).c_str());
						return;
					}
					break;
				}*/
			
		default:
			LogErr("ReloadCfg WaitForMultipleObjects Err !");
			return;
		}
	}
	return;
}

void CFileWatcherThread::OnFileChanged(EDir eDir)
{
	if (Inst())
	{
		Inst()->CalculateState(eDir);
	}
}

void CFileWatcherThread::AddChangeFileData(const char* szFileName)
{
	if (!m_pLUAFileDataVec)
	{
		m_pLUAFileDataVec = new CFileWatcherData();
		m_pLUAFileDataVec->SetIsLuaFile(true);
	}
	if (!m_pTXTFileDataVec)
	{
		m_pTXTFileDataVec = new CFileWatcherData();
		m_pTXTFileDataVec->SetIsLuaFile(false);
	}

	wstring wpFileNameStr = L"";
	if ( false == gbk_to_utf16(wpFileNameStr, szFileName) )
	{
		LogErr("字符串转换出错 - CFileWatcherThread::AddChangeFileData() wpFileNameStr", szFileName);
	}
	
	const wchar_t* pFileName = wpFileNameStr.c_str();
	int nLen = (int)wcslen( pFileName );
	if (!_wcsicmp( pFileName + nLen - 4, L".lua"))
	{
		//m_pLUAFileDataVec->PushBack(szFileName);
	}
	else if (!_wcsicmp( pFileName + nLen - 4, L".txt" ))
	{
		m_pTXTFileDataVec->PushBack(szFileName);
	}
}

void CFileWatcherThread::ClearChangeFileData()
{
	if (m_pTXTFileDataVec)
	{
		for (uint32 i=0; i<m_pTXTFileDataVec->GetCount(); i++)
		{
			DelChangeFileRecord(m_pTXTFileDataVec->GetFileName(i));
		}
		m_pTXTFileDataVec->Release();
		m_pTXTFileDataVec = NULL;
	}
	if (m_pLUAFileDataVec)
	{
		for (uint32 i=0; i<m_pLUAFileDataVec->GetCount(); i++)
		{
			DelChangeFileRecord(m_pLUAFileDataVec->GetFileName(i));
		}
		m_pLUAFileDataVec->Release();
		m_pLUAFileDataVec = NULL;
	}
}

bool CFileWatcherThread::GetFileAmendTime(const char* strFile,FILETIME* time)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strFile,&fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	*time = fd.ftLastWriteTime;
	FindClose(hFind);
	return true;
}

void CFileWatcherThread::SaveCurrentTime(const char* strFileName)
{
	map<string,FILETIME>::iterator iter = m_mapFileTime.find(strFileName);
	if (iter != m_mapFileTime.end())
	{
		FILETIME ft;
		if(GetFileAmendTime(strFileName,&ft))
			iter->second = ft;
	}
}

void CFileWatcherThread::InitAllFileState()
{
	FileTreeWalk(m_strCfgTgtPath.c_str(),InitCfgFileState,NULL);
	FileTreeWalk(m_strCfgLangPath.c_str(),InitCfgFileState,NULL);
	FileTreeWalk(m_strCodePath.c_str(),InitCodeFileState,NULL);
}

void CFileWatcherThread::CalculateState(EDir eDir)
{
	if (eDir == ED_CFG_TGT)
	{
		wstring& wszCfgTgtPath = m_strCfgTgtPath + wstring(L"/skill");
		FileTreeWalk(wszCfgTgtPath.c_str(),CheckFileState,NULL);
	}
	else if (eDir == ED_CFG_LANG)
	{
		FileTreeWalk(m_strCfgLangPath.c_str(),CheckFileState,NULL);
	}
	else if (eDir == ED_CODE_Gas)
	{
		wstring& wssCodePath = GetCodePath() + wstring(L"/gas/lua");
		FileTreeWalk(wssCodePath.c_str(),CheckFileState,NULL);
	}
	else if (eDir == ED_CODE_Gac)
	{
		wstring& wscCodePath = GetCodePath() + wstring(L"/gac/lua");
		FileTreeWalk(wscCodePath.c_str(),CheckFileState,NULL);
	}
	else if (eDir == ED_CODE_Gac2Gas)
	{
		wstring& ws2CodePath = GetCodePath() + wstring(L"/gac_gas/lua");
		FileTreeWalk(ws2CodePath.c_str(),CheckFileState,NULL);
	}
}

FTW_RESULT CFileWatcherThread::InitCfgFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = (int)wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".lua" ) &&  _wcsicmp( szFileName + nLen - 4, L".txt" ))
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CFileWatcherThread::Inst()->SaveFileState(strFileName);
	return _FTW_CONTINUNE;
}

FTW_RESULT CFileWatcherThread::InitCodeFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = (int)wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".lua" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CFileWatcherThread::Inst()->SaveFileState(strFileName);
	return _FTW_CONTINUNE;
}

FTW_RESULT CFileWatcherThread::CheckFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = (int)wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" )) //_wcsicmp( szFileName + nLen - 4, L".lua" ) &&  
		return _FTW_CONTINUNE;

	string strFileName = utf16_to_gbk(szFileName);
	return CFileWatcherThread::Inst()->CheckFileModify(strFileName);
}

FTW_RESULT CFileWatcherThread::CheckFileModify(string& strFileName)
{
	map<string,FILETIME>::iterator iter = m_mapFileTime.find(strFileName);
	if (iter != m_mapFileTime.end())
	{
		FILETIME ft;
		if(GetFileAmendTime(strFileName.c_str(),&ft) && (ft.dwHighDateTime != iter->second.dwHighDateTime || ft.dwLowDateTime != iter->second.dwLowDateTime))
		{
			m_pAdapter->PostThreadEvent(new CFileWatcherEvent(strFileName,m_pChangeFileDataVec));
			SaveCurrentTime(strFileName.c_str());
			return _FTW_STOP;
		}
	}
	return _FTW_CONTINUNE;
}

void CFileWatcherThread::SaveFileState(string& strFileName)
{
	if (m_mapFileTime.find(strFileName) == m_mapFileTime.end())
	{
		FILETIME ft;
		if(GetFileAmendTime(strFileName.c_str(),&ft))
		{
			m_mapFileTime.insert(make_pair(strFileName,ft));
		}
	}
}

void CFileWatcherThread::AddChangeFileRecord(const char* strFileName)
{
	wstring wpFileNameStr = L""; 
	if ( false == gbk_to_utf16(wpFileNameStr, strFileName) )
	{
		LogErr("字符串转换出错 - CFileWatcherThread::AddChangeFileRecord() wpFileNameStr", strFileName);
	}
	
	const wchar_t* pFileName = wpFileNameStr.c_str();
	int nLen = (int)wcslen( pFileName );
	if (!_wcsicmp( pFileName + nLen - 4, L".lua"))
	{
		return; //现在不处理Lua文件
	}
	
	if (!m_pChangeFileDataVec)
	{
		m_pChangeFileDataVec = new CFileWatcherData();
		m_pChangeFileDataVec->SetIsLuaFile(false);
	}
	for (uint32 i=0; i< m_pChangeFileDataVec->GetCount(); i++)
	{
		if (strcmp(m_pChangeFileDataVec->GetFileName(i),strFileName) == 0)
		{
			return;	
		}
	}
	m_pChangeFileDataVec->PushBack(strFileName);
}

void CFileWatcherThread::DelChangeFileRecord(const char* strFileName)
{
	if (m_pChangeFileDataVec)
	{
		m_pChangeFileDataVec->Pop(strFileName);
	}
}

const wchar_t* CFileWatcherThread::GetCfgTgtPath()
{
	return m_strCfgTgtPath.c_str();
}

const wchar_t* CFileWatcherThread::GetCfgLangPath()
{
	return m_strCfgLangPath.c_str();
}

const wchar_t* CFileWatcherThread::GetCodePath()
{
	return m_strCodePath.c_str();
}

DLGPROC DialogProcFunc(HWND hWnd, UINT uInt, WPARAM wParam, LPARAM lParam)
{
	switch(uInt)
	{
	case WM_INITDIALOG:
		return 0;
	case WM_COMMAND:
		if(LOWORD(wParam)==IDOK)
		{
			HWND ListhWnd = GetListChild(hWnd);
			int selCount = (int)SendMessage(ListhWnd,LB_GETSELCOUNT, 0, 0);
			int* indexBuff = new int[selCount];
			SendMessage(ListhWnd,LB_GETSELITEMS,selCount, LPARAM(indexBuff));
			for (int index=0; index<selCount; index++)
			{
				int len = (int)SendMessage(ListhWnd,LB_GETTEXTLEN,index,0);
				char* szBuffer = new char[len];
				SendMessage(ListhWnd,LB_GETTEXT,indexBuff[index],LPARAM(szBuffer));
				CFileWatcherThread::Inst()->AddChangeFileData(szBuffer);
			}
			CFileWatcherThread::Inst()->NotityFileChanged();
		}
		else if(LOWORD(wParam)==IDCANCEL)
		{
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		hWnd = NULL;
		CFileWatcherThread::Inst()->hWnd = NULL;
		return 0;
	}
	return 0;
}

void CFileWatcherThread::NotityFileChanged()
{
	if (m_pTXTFileDataVec && !m_pTXTFileDataVec->Empty())
	{
		m_pAdapter->NotityFileChanged(m_pTXTFileDataVec);
		for (uint32 i=0; i<m_pTXTFileDataVec->GetCount(); i++)
		{
			HWND ListhWnd = GetListChild(hWnd);
			LRESULT index = SendMessage(ListhWnd, LB_FINDSTRING, 0, LPARAM(m_pTXTFileDataVec->GetFileName(i)));
			if (index != LB_ERR)
			{
				LRESULT ret = SendMessage(ListhWnd, LB_DELETESTRING, index, 0);
			}
		}
	}
	if (m_pLUAFileDataVec && !m_pLUAFileDataVec->Empty())
	{
		m_pAdapter->NotityFileChanged(m_pLUAFileDataVec);
		for (uint32 i=0; i<m_pLUAFileDataVec->GetCount(); i++)
		{
			HWND ListhWnd = GetListChild(hWnd);
			LRESULT index = SendMessage(ListhWnd, LB_FINDSTRING, 0, LPARAM(m_pLUAFileDataVec->GetFileName(i)));
			if (index != LB_ERR)
			{
				LRESULT ret = SendMessage(ListhWnd, LB_DELETESTRING, index, 0);
			}
		}
	}
	ClearChangeFileData();
}

void CFileWatcherThread::ShowChangeFile(const char* szFileName, CFileWatcherData* pChangeFileDataVec)
{
	if (!hWnd)
	{
		hWnd = ShowDlg((DLGPROC)DialogProcFunc);
		if (pChangeFileDataVec)
		{
			HWND ListhWnd = GetListChild(hWnd);
			for (uint32 i=0; i<pChangeFileDataVec->GetCount(); i++)
			{
				LRESULT index = SendMessage(ListhWnd, LB_FINDSTRING, 0, LPARAM(pChangeFileDataVec->GetFileName(i)));
				if (index == LB_ERR)
				{
					LRESULT ret = SendMessage(ListhWnd, LB_ADDSTRING, 0, LPARAM(pChangeFileDataVec->GetFileName(i)));
				}
			}
		}
	}
	HWND ListhWnd = GetListChild(hWnd);
	LRESULT index = SendMessage(ListhWnd, LB_FINDSTRING, 0, LPARAM(szFileName));
	if (index == LB_ERR)
	{
		LRESULT ret = SendMessage(ListhWnd, LB_ADDSTRING, 0, LPARAM(szFileName));
	}
	AddChangeFileRecord(szFileName);
}


CFileWatcherEvent::CFileWatcherEvent(string& strFileName, CFileWatcherData* pFileWatcherData)
:m_szFileName(strFileName)
,m_pFileWatcherData(pFileWatcherData)
{
}

CFileWatcherEvent::~CFileWatcherEvent()
{
	if (CFileWatcherThread::Inst())
	{
		CFileWatcherThread::Inst()->ShowChangeFile(m_szFileName.c_str(), m_pFileWatcherData);
	}
}

}

#endif
