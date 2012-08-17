#pragma once
#include <vector>
#include "CPathMgr.h"
namespace sqr_tools
{
	class CTaskProcess;
	class CFileProcessCtrlImp;
	class CProgressCtrlImp;
	typedef vector<CTaskProcess*> TaskPool;

	class CFileProcessMgr
	{
	public:
		CFileProcessMgr();
		~CFileProcessMgr();
		TaskPool m_TaskList;

		void RegistTask(CTaskProcess* pTask);
		void Clear();
		void Run(CFileProcessCtrlImp* pFileProcessCtrlImp, CProgressCtrlImp* pProgressCtrlImp);

		void SetDir(const wchar_t* szDir);
		void SetFileList(vector<wstring>& filelist);
		vector<wstring>  m_FileList;
		wstring			m_Dir;
	};
}