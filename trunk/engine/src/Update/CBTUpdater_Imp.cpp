#include "stdafx.h"

#include <direct.h>
#include <tchar.h>
#include <process.h>

#include "CBTUpdater_Imp.h"
#include "CAsyncUpdater.h"
#include "IBTDownloadHandler.h"
#include "IAsyncUpdateHandler.h"
#include "CPatcher.h"
#include "CHTTPDownloader.h"
#include "global.h"



NAMESPACE_BEGIN(sqr)

static bool FileExists(LPCTSTR lpszFileName);


CBTUpdater_Imp::CBTUpdater_Imp()
	: m_http(new CHTTPDownloader)
{
	m_Bittorrent = CreateDownloader(); // 获得BT下载器句柄
	pThis = this;
}

CBTUpdater_Imp::~CBTUpdater_Imp()
{
	if(&m_CriticalSection)
		DeleteCriticalSection(&m_CriticalSection);

	delete m_Bittorrent;
	delete m_http;
}

int CBTUpdater_Imp::BeginDownloadPatchThread(IAsyncUpdateHandler* updater, const TCHAR* Host, const TCHAR* File, const TCHAR* szCurPath)
{
	// 记录相关信息
/*	m_strHost = string(Host);
	m_strFile = string(File);
	m_strCurPath = string(szCurPath);
	m_pAsyncUpdateHandler = updater;

	//// 开始运行下载线程
	unsigned int tid; // 标识线程的ID
	uintptr_t BTThread;
	BTThread = _beginthreadex(NULL, 0, &CBTUpdater_Imp::CBdownload_patch, this, 0, &tid); // 第三个参数是创建线程运行的函数

	if(!BTThread)
	{
		m_pAsyncUpdateHandler->ShowMessage("创建BT下载线程失败!");
		return 0;
	}
*/

	return 1;
}

// BT下载主函数
// *** 该函数的版本获得方式还没有修改 ***
int CBTUpdater_Imp::Download()
{
/*
	// 获取升级补丁列表文件YbtxPatchList.data
	m_http->Download(m_strHost, m_strFile);
	string content(m_http->GetContent());

	// 创建下载目录
	TCHAR *szDownDir = "\\download";
	TCHAR szSavePath[MAX_PATH_SIZE];
	GetCurrentDirectory(MAX_PATH_SIZE, szSavePath);
	strcat(szSavePath, szDownDir);
	_mkdir(szSavePath);

	TCHAR path[1024];
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL && GetModuleFileName(hModule, path, MAX_PATH) != 0)
	{
		m_Bittorrent->AddFirewallException(path, _T("YbtxBT_Downloader"));
	}

	m_Bittorrent->InitAgent("YbtxBT_1.0");

	// 此处需要根据自身版本号进行对比，以确定具体需要下载那几个版本，把需要下载的版本号结构压入pl
	PatchList pl;
	CreatePatchList(pl, content);
	// 没有需要更新的补丁
	if(pl.size() < 1)
	{
		m_Bittorrent->Shutdown();
		//m_pAsyncUpdateHandler->CallBackFinishPatch(UPDATED_FLAG);
		return 0;
	}

	// 逐个下载各版本的Patch文件，每个Patch对应一个种子文件
	CPatcher patcher;
	PatchList::iterator it = pl.begin();
	for(; it!=pl.end(); ++it)
	{
		HANDLE hDownloader = bt_download(it->url, szSavePath);
		// 进入下载主循环等待该Patch下载完成
		int iPercent = 0;
		if (hDownloader == NULL)
		{
			m_pAsyncUpdateHandler->ShowMessage("BT下载补丁出错啦！！！！");
			return 1;
		}
		else
		{
			for(;;)
			{
				iPercent = m_Bittorrent->GetProgress(hDownloader);

				if (iPercent >= 100)
					break;
				else if (iPercent >= 0)
					m_pAsyncUpdateHandler->SetDownState(iPercent);
				else
					m_pAsyncUpdateHandler->ShowMessage("BT下载进度小于0！");

				Sleep(40);
			}

			m_pAsyncUpdateHandler->SetDownState(iPercent);
		}

		// 下载完毕一个补丁就上一个补丁
		TCHAR buffer[MAX_PATH];
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%s\\%s\\patch_%d.%d.%d.ybtx", m_strCurPath.c_str(), szDownDir, it->ver.byHiVersion, it->ver.byLoVersion, it->ver.wBuildVersion);
		patcher.CheckHeadInfo(m_pAsyncUpdateHandler, buffer);
		//m_pAsyncUpdateHandler->UpdateVersion();
		break;
	}

	m_Bittorrent->Shutdown();
	//m_pAsyncUpdateHandler->CallBackFinishPatch("updating");
*/
	return 0;
}

// 下载单个BT文件
HANDLE CBTUpdater_Imp::bt_download(const string& url, const string& saveas)
{
	// 解析url，提取出host和file
	string DownUrl(url);
	int iStart = DownUrl.find_first_of(':');
	iStart = iStart + 3; // 跳过 ://
	int iEnd = DownUrl.find_first_of('/', iStart);

	// 下载种子内容
	m_http->Download(DownUrl.substr(iStart, iEnd-iStart), DownUrl.substr(iEnd, DownUrl.size() - iEnd));
	string content(m_http->GetContent());

	HANDLE hDownloader = m_Bittorrent->DownFromBuffer((LPBYTE)content.data(), content.size(), saveas.c_str());

	return hDownloader;
}

unsigned int __stdcall CBTUpdater_Imp::CBdownload_patch(void* pClass)
{
	CBTUpdater_Imp* pCBTUpdater_Imp = static_cast<CBTUpdater_Imp*>(pClass);

	return pCBTUpdater_Imp->Download();
}


static bool FileExists(LPCTSTR lpszFileName)
{
	DWORD dwAttributes = GetFileAttributes(lpszFileName);

	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		return true;
	else
		return false;
}


NAMESPACE_END

