#pragma once

#include <string>
#include <vector>
#include <map>
#include <hash_map>

#include "WebBrowser.h"
#include "RegionManage.h"
#include "ImageManage.h"
#include "ImageButton.h"
#include "ImageProgress.h"
#include "ImageLabel.h"
#include "StaticText.h"
#include "afxcmn.h"
#include "FileTreeWalk.h"
#include "StaticText.h"
#include "SelectServerDlg.h"
#include "afxcmn.h"

using namespace std;
using namespace stdext;

class CImageButton;
class CImageProgress;
struct IHTMLDocument2;
class CStarterUpdateHandler;

namespace sqr
{
	class CAsyncUpdater;
	class VersionManager;
	class IAsyncUpdateHandler;
}

class CDownloadHandler;
class CNewHTTPDownloader;

enum DownType
{
	eDT_CheckVersion,
	eDT_VersionList,
	eDT_PatchFile,
	eDT_ServerList,
};

enum DownMode
{
	eDM_NONE,
	eDM_BT,
	eDM_HTTP,
};

struct PatchArgs
{
	sqr::IAsyncUpdateHandler* handler;
	std::wstring              PatchFile;
	std::wstring			  strMsg1;
	std::wstring			  strMsg2;
};

typedef int (* PFN_UPDATAYYVOICE)( const char*, void*, void*, void* ) ;
struct VerifyArgs
{
	sqr::IAsyncUpdateHandler* handler;
	std::wstring              strDirectory;
	std::wstring			  strCheckCodeFileName;
};
class CYbtxDlg : public CDialog
{
public:
	CYbtxDlg(CWnd* pParent = NULL);	
	~CYbtxDlg();
	void	UpdateSelectedServer();
	void	ChangeServer(); 
	wstring GetImagePath( const char* szImageFileName );
	void	ShowSelectServerBtn();

private:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	void ProcessList();
	void ApplyPatch();
	void StartupUpdater();
	void StartupStarter();
	void InitData();
	bool ProcessEntry(const char* szNewTarget);
	void AnalysisServerList();
	void ReadCrcCodeToMap();
	void Crc32Check();
	void DoCrc32Check( const string& strFile );
	void DownloadServerListFile();

	void VerifyFiles();
	static unsigned int __stdcall StartVerifyFiles(void* pClass);// MD5校验线程 

	static unsigned int __stdcall StartPatch(void* pClass);
	void InitLangPath();
	void InitYY();
	void YYChangeNormalState();
	void ChangeNormalState();
	void CreateInPatchFlag();

public:
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT ShowMessage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT ChangeDownState(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT ChangeSetupState(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT RestartProcess(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT DownloadFinish(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT AlertExit(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT PatchContinue(WPARAM wparam, LPARAM lparam);
	afx_msg	LRESULT DownloadYYFinish(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT VerifyFilesFinish(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonServer();
	afx_msg void OnBnClickedButtonBBS();
	afx_msg void OnBnClickedButtonCharge();
	afx_msg void OnBnClickedButtonRegist();
	afx_msg void OnBnClickedButtonSelectServer();
	afx_msg void OnBnClickedButtonOpenServerItem();
	afx_msg void OnBnClickedButtonManualUpdate();
	afx_msg void OnBnClickedButtonMin();
	afx_msg void OnBnClickedButtonLookChar();
	afx_msg void OnBnClickedButtonOpenDuoWan();
	DECLARE_MESSAGE_MAP()

private:
	enum { IDD = IDD_YBTX_DIALOG };
	HICON m_hIcon;
	CImageLabel   m_LocalVersion;
	CImageLabel   m_OfficialVersion;
	CImageLabel   m_WebBrowser;
	CImageLabel	  m_SelectedServerName;

	CImageButton  m_SetBtn;
	CImageButton  m_QuitBtn;
	CImageButton  m_MinBtn;
	CImageButton  m_StartBtn;
	CImageButton  m_ServerBtn;
	CImageButton  m_bbsBtn;
	CImageButton  m_ChargeBtn;
	CImageButton  m_RegistBtn;
	CImageButton  m_OpenServerItemBtn; 
	CImageButton  m_ManualUpdateBtn;
	CImageButton  m_SelectServerBtn;
	CImageButton  m_LookCharBtn;
	CImageButton  m_OpenDuoWanBtn;

	CImageProgress  m_DownProgress;
	CImageProgress  m_SetupProgress;

	CWebBrowser     m_WebNews;

	CSelectServerDlg m_SelectedServerDlg;

	CRegionManage    m_RegionManage; // 外形管理类
	CImageManage     m_BGImage;      // 图像管理类

	CStarterUpdateHandler*  m_pUpdater;      // 更新显示回调，会有多线程共用
	CDownloadHandler*       m_pDownHandle;   // 下载回调句柄，会有多线程共用
	CNewHTTPDownloader*     m_pDownloader;   // http下载类
	sqr::VersionManager*    m_pVersion;      // 版本管理类
	PatchArgs*              m_PatchArgs;     // 为多线程函数参数组装的struct

	wstring                 m_strServerInfo; // 由web获得的连接服务器信息
	wstring                 m_PatchListFile; // patch列表下载后的本地绝对路径
	wstring                 m_PatchUrl;      // patch下载服务器url
	wstring					m_PatchListUrl;  // patch列表下载服务器url
	wstring                 m_PatchFile;     // patch文件名
	wstring                 m_DownloadDir;   // patch下载本地存放路径
	wstring                 m_StarterFile;   // 启动器Ybtx.exe绝对路径
	wstring                 m_UpdaterFile;   // 更新器Updater.exe绝对路径
	wstring                 m_StarterPath;   // 启动器与更新器所在目录路径
	string                  m_TargetVersion; // 对应服务器需要的版本
	DownMode                m_eDownMode;     // 下载补丁的方式，http, bt等
	wstring                 m_ServerListFile; // 服务器列表文件下载后本地路径

	wstring                 m_ServerListUrl; // 服务器版本列表链接
	wstring                 m_NewsUrl;       // 新闻页面链接
	wstring					m_wstrUrlPath;	 // 当IntUrl.ini存在时，取URL_INI_FILE，否则取INT_URL_INI_FILE

	DownType                m_DownType;      // 当前下载文件类型
	bool                    m_InUpdate;      // 下载中...标记
	bool                    m_InPatch;       // 上补丁中...标记
	bool                    m_bNeedReload;	//首次启动调用标记
	bool                    m_bChangeTarget; // 是否中途改变了目标版本
	hash_map<string, string> m_hmapCrcCode;	 // crc32文件校验码
	IHTMLDocument2*			m_pIHTMLDOC;
	CStaticText*			m_pStaticText;
	string					m_strLangResPath;
	string					m_strStaticTxtPath;
	string					m_strOriLangResPath;
	string					m_strOriStaticTxtPath;
	bool					m_InYYUpdate;
	bool					m_InYbtxUpdate;
	
	WCHAR m_szYYFilePath[MAX_PATH]; 
	PFN_UPDATAYYVOICE pfnYYUpdate;
	VerifyArgs*				m_pVerifyArgs;
	bool					m_bNeedMd5Verify;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

