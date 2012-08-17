#pragma once

#include "global.h"
#include "BaseTypes.h"
#include "HeadDataType.h"
#include "CAsyncUpdater.h"
#include "IBTDownloadHandler.h"



NAMESPACE_BEGIN(sqr)

class IAsyncUpdateHandler;
class CHTTPDownloader;

class CBTUpdater_Imp
{
public:
	CBTUpdater_Imp();
	~CBTUpdater_Imp();

	int Download();
	HANDLE bt_download(const string& url, const string& saveas); // 下载单个BT文件

	// BT下载
	static unsigned int __stdcall CBdownload_patch(void* pClass);
	int BeginDownloadPatchThread(IAsyncUpdateHandler* updater, const TCHAR* Host, const TCHAR* File, const TCHAR* szCurPath);

	// 操纵窗口空间回调类
	IAsyncUpdateHandler*  m_pAsyncUpdateHandler;
	IBTDownloadHandler*   m_Bittorrent;
	CHTTPDownloader*      m_http;

private:
	string  m_strHost;         // 下载服务器路径
	string  m_strFile;         // 下载文件名
	string  m_strCurPath;      // 当前路径

	string  m_strTorrentUrl;   // 当前下载种子链接
	string  m_strDstPath;      // 目标路径
	string  m_strSaveAs;       // 另存路径
	string  m_strDownloadPath; // 下载路径

	//seed_style      m_seed_style;
	PatchList       m_downloaded_packet;
	string          m_strBtOption;

	CRITICAL_SECTION   m_CriticalSection;
	FILE*              m_logfile;
};

static CBTUpdater_Imp *pThis;

NAMESPACE_END

