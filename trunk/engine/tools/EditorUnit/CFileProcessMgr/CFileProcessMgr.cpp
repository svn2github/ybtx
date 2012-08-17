#include "stdafx.h"
#include "CFileProcessMgr\CFileProcessMgr.h"
#include "CFileProcessMgr\CTaskProcess.h"
#include "CodeCvs.h"
#include "BaseHelper.h"
#include "FileProcessCtrl/FileProcessCtrlImp.h"
#include "ProgressCtrl/CProgressCtrlImp.h"

FTW_RESULT DoTask(const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *pNull)
{
	if( eFlag == _FTW_DIR && !_wcsicmp( szFileName + wcslen( szFileName ) - 4, L".svn" ) )
		return _FTW_IGNORE;

	TaskPool* pTaskPool = (TaskPool*)pNull;
	TaskPool::iterator iter = pTaskPool->begin();
	while (iter!=pTaskPool->end())
	{
		(*iter)->Do(szFileName);
		iter++;
	}
	return _FTW_CONTINUNE;
}

CFileProcessMgr::CFileProcessMgr()
{
}

CFileProcessMgr::~CFileProcessMgr()
{
	Clear();
}

void CFileProcessMgr::RegistTask(CTaskProcess* pTask)
{
	 m_TaskList.push_back(pTask);
}

void CFileProcessMgr::Clear()
{
	m_TaskList.clear();
}

//void CFileProcessMgr::Run(IProgressCallback* pProgress)
//{
//	if(!m_FileList.empty())
//	{
//		for(size_t i = 0;i<m_FileList.size();i++)
//		{
//			pProgress->SetDescription(utf16_to_gbk(m_FileList[i]).c_str());
//			TaskPool::iterator iter = m_TaskList.begin();
//			while(iter != m_TaskList.end())
//			{
//				(*iter)->Do(m_FileList[i].c_str());
//				iter++;
//			}
//			pProgress->SetProgress((float)i/(m_FileList.size() ));
//		}
//	}
//	if(m_Dir!=L"")
//		FileTreeWalk( m_Dir.c_str(), DoTask, &m_TaskList);
//}

void CFileProcessMgr::Run(CFileProcessCtrlImp* pFileProcessCtrlImp, CProgressCtrlImp* pProgressCtrlImp)
{
	if(!m_FileList.empty())
	{
		size_t size = m_FileList.size();
		int nMax = 100;
		pProgressCtrlImp->SetProgressMaxValue(nMax);
		pProgressCtrlImp->SetProgressValue(0);
		pProgressCtrlImp->Update();
		
		string strFileName("");
		for(size_t i = 0; i < size; ++i)
		{
			strFileName = utf16_to_gbk(m_FileList[i]).c_str();
			strFileName = "正在处理:" + strFileName;
			pFileProcessCtrlImp->SetCurTaskText(strFileName);
			TaskPool::iterator iter = m_TaskList.begin();
			while(iter != m_TaskList.end())
			{
				(*iter)->Do(m_FileList[i].c_str());
				iter++;
			}

			const int pre_progress  = pProgressCtrlImp->GetProgressValue();
			const int progress_left = nMax - pre_progress;
			int       new_progress  = pre_progress; 
			++new_progress;
			if( progress_left == 0 )
				new_progress = 0;

			pProgressCtrlImp->SetProgressValue(new_progress);
			pProgressCtrlImp->Update();
		}
	}
	if(m_Dir!=L"")
		FileTreeWalk( m_Dir.c_str(), DoTask, &m_TaskList);
}

void CFileProcessMgr::SetDir(const wchar_t* szDir)
{
	m_Dir = szDir;
}
void CFileProcessMgr::SetFileList(vector<wstring>& filelist)
{
	m_FileList = filelist;
}
