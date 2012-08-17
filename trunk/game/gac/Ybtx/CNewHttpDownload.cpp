#include "stdafx.h"
#include <afxinet.h>
#include <iostream>
#include <sstream>
#include <process.h>
#include "Ybtx.h"
#include "CNewHttpDownload.h"

void CNewHTTPDownloader::DownloadFile(const wstring& strHostName, 
									  const wstring& strFileName, 
									  const wstring& strDistPath, 
									  int nPriority /* = THREAD_PRIORITY_IDLE */)
{
	m_strHostname = strHostName;
	m_strFileName = strFileName;
	m_strDistPath = strDistPath;
	m_handleCurThread = (HANDLE)_beginthreadex(NULL, 0, &CNewHTTPDownloader::__uis_DownloadFile, this, 0, &m_uCurThreadId);
}

unsigned int __stdcall CNewHTTPDownloader::__uis_DownloadFile(void* pParam)
{
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	CNewHTTPDownloader* pThis = (CNewHTTPDownloader*)pParam;
	char *pBuf = NULL;
	int nBufLen = 0;
	CInternetSession Sess;
	Sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 30*1000);
	Sess.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
	Sess.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 5);
	wstring strFileUrl = pThis->m_strHostname + L"/" + pThis->m_strFileName;

	//获取当前文件的长度
	int nCurrentLength = 0;
	wstring strOutFile = pThis->m_strDistPath + L"/" + pThis->m_strFileName;
	ifstream inFile(strOutFile.c_str(), std::ios::out|std::ios::binary);
	if (inFile.is_open())
	{
		inFile.seekg(0, std::ios::end);
		nCurrentLength = inFile.tellg();
	}
	inFile.close();

	//连接http服务器
	DWORD dwFlags = INTERNET_FLAG_TRANSFER_BINARY;
	
	CHttpFile* pF;
	
	try
	{
		pF = (CHttpFile*)Sess.OpenURL(strFileUrl.c_str(), 1, dwFlags);
	}
	catch(CInternetException* pInternetExcep)
	{
		wchar_t wszError[100];
		switch(pInternetExcep->m_dwError)
		{
		case 12029:
			wsprintf(wszError, L"无法连接到补丁下载服务器，请确认网络是否畅通。(%d)", pInternetExcep->m_dwError);
			break;
		case 12007:
			wsprintf(wszError, L"无法获取补丁系在服务器的ip地址。(%d)", pInternetExcep->m_dwError);
			break;
		default:
			wsprintf(wszError, L"下载补丁失败。错误号：%d", pInternetExcep->m_dwError);
			break;
		}
		
		pThis->m_pHander->OnError(strFileUrl, wszError);
		return 1;
	}
	if (!pF)
	{
		pThis->m_pHander->OnError(strFileUrl, L"下载补丁失败");
		return 1;
	}

	CString str;
	pF->QueryInfo(HTTP_QUERY_STATUS_CODE, str);

	//判断http服务器返回值
	if (str!= L"200")
	{
		pF->Close();
		delete pF;
		pThis->m_pHander->OnError(strFileUrl, wstring(strFileUrl + L"补丁下载地址找不到该文件").c_str());	
		return 1;
	}

	//设置偏移值，开始下载
	try
	{
		pF->Seek(nCurrentLength, 0);
	}
	catch (CInternetException* pInternetExcep)
	{
		wchar_t wszError[100];
		wsprintf(wszError, L"设置补丁续传位置时发生网络异常：%d", pInternetExcep->m_dwError);
		pThis->m_pHander->OnError(strFileUrl, wszError);
		return 1;
	}
	pF->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, str);
	pThis->m_pHander->OnStartDownload(strFileUrl);

	if (_wtoi(str))
	{
		wstring strOutFile = pThis->m_strDistPath + L"/" + pThis->m_strFileName;
		
		HANDLE houtFile = CreateFile(strOutFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == houtFile)
		{
			if (ERROR_ACCESS_DENIED == GetLastError())
			{
				pThis->m_pHander->OnError(strFileUrl, L"文件被占用");
			}
			else
			{
				pThis->m_pHander->OnError(strFileUrl, wstring(strFileUrl + L"文件不存在").c_str());
			}
			return 1;
		}
		if (0xFFFFFFFF  == SetFilePointer(houtFile, 0, NULL, FILE_END))
		{
			pThis->m_pHander->OnError(strFileUrl, L"文件无法定位到末尾");
			CloseHandle(houtFile);
			return 1;
		}

		int nLen = (nBufLen = _wtoi(str));
		nLen -= nCurrentLength;
		MSG msg;
		while(true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_USER_DOWNLOAD_SWITCH)
				{
					CloseHandle(houtFile);
					pThis->m_pHander->OnProcess(strFileUrl, 0, nBufLen);
					return 0;
				}
			}
			char* pBuf = new char[8192];
			int n;
			try
			{
				n = pF->Read(pBuf, (nLen < 8192) ? nLen: 8192);
			}
			catch (CInternetException* pInternetExcep)
			{
				CloseHandle(houtFile);
				wchar_t wszError[100];
				wsprintf(wszError, L"下载补丁时发生网络异常：%d", pInternetExcep->m_dwError);
				pThis->m_pHander->OnError(strFileUrl, wszError);
				return 1;
			}
			if (n<=0)
				break;
			DWORD NumberOfBytesWritten = 0;
			WriteFile(houtFile, pBuf, n, &NumberOfBytesWritten, NULL);
			nLen -= n;
			delete []pBuf;
			pThis->m_pHander->OnProcess(strFileUrl, nBufLen-nLen, nBufLen);
		}
		CloseHandle(houtFile);
		if (nLen != 0)
		{
			pThis->m_pHander->OnError(strFileUrl, L"与补丁下载服务器断开连接");
		}
		else{
			pThis->m_pHander->OnFinished(strOutFile);
		}
	}
	return 0;
}

HANDLE CNewHTTPDownloader::GetCurThreadHandle()
{
	return m_handleCurThread;
}

DWORD CNewHTTPDownloader::GetCurThreadId()
{
	return m_uCurThreadId;
}
