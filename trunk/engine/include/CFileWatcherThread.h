#ifdef _WIN32

#pragma once
#include "CDynamicObject.h"
#include "ThreadTypes.h"
#include "windows.h"
#include "CAppThreadEvent.h"
#include "CPathMgr.h"
#include "CFileWatcherData.h"
#include "CFileWatcherAdapter.h"

namespace sqr
{
	enum EDir
	{
		ED_NONE,
		ED_CFG_TGT,
		ED_CFG_LANG,
		ED_CODE_Gas,
		ED_CODE_Gac,
		ED_CODE_Gac2Gas,
	};

	class CFileWatcherData;

	class CFileWatcherThread
		 :public virtual CDynamicObject
	{
	public:
		typedef map<string,FILETIME>				MapFileTime;
		
		CFileWatcherThread();
		CFileWatcherThread(CFileWatcherAdapter* pAdapter, const char* szCfgPath, const wchar_t* szLangPath, const char* szCodePath);
		~CFileWatcherThread(void);
	
		static CFileWatcherThread*& Inst();
		static void Init(CFileWatcherAdapter* pAdapter, const char* szCfgPath, const wchar_t* szLangPath, const char* szCodePath);
		static void Unit();
		static void FileWatcherThreadProc(void* pArg);
		void	InitAllFileState();
		void	OnFileChanged(EDir eDir);
		void	AddChangeFileData(const char* szFileName);
		void	ClearChangeFileData();
		bool	GetFileAmendTime(const char* strFileName,FILETIME* time);
		void	SaveCurrentTime(const char* strFileName);
		void	CalculateState(EDir eDir);
		void	SaveFileState(string& strFileName);
		FTW_RESULT CheckFileModify(string& strFileName);
		
		void	AddChangeFileRecord(const char* strFileName);
		void	DelChangeFileRecord(const char* strFileName);
		void	NotityFileChanged();
		void	ShowChangeFile(const char* szFileName, CFileWatcherData* pChangeFileDataVec);
		const wchar_t* GetCfgTgtPath();
		const wchar_t* GetCfgLangPath();
		const wchar_t* GetCodePath();
		
		static FTW_RESULT InitCfgFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
		static FTW_RESULT InitCodeFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
		static FTW_RESULT CheckFileState( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	public:
		HWND						hWnd;
	private:
		static CFileWatcherThread*	ms_pInst;
		HTHREAD						m_hThread;
		HANDLE						m_evCancel;
		MapFileTime m_mapFileTime;
		wstring m_strCfgTgtPath;
		wstring m_strCfgLangPath;
		wstring m_strCodePath;
		CFileWatcherData* m_pChangeFileDataVec;//被修改文件的记录
		CFileWatcherData* m_pLUAFileDataVec;
		CFileWatcherData* m_pTXTFileDataVec;
		CFileWatcherAdapter* m_pAdapter;
	};

	class CFileWatcherEvent : public CAppThreadEvent
	{
	public:
		CFileWatcherEvent(string& strFileName, CFileWatcherData* pFileWatcherData);
		~CFileWatcherEvent();
		string m_szFileName;
		CFileWatcherData* m_pFileWatcherData;
	};

}
#endif
