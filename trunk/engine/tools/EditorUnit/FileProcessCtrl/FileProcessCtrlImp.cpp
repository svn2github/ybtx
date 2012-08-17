#include "stdafx.h"
#include "FileProcessCtrl/FileProcessCtrlImp.h"
#include "ProgressCtrl/CProgressCtrlImp.h"
#include "CFileProcessMgr/CFileProcessMgr.h"
#include "CFileProcessMgr/CTaskProcess.h"
#include "CFileProcessMgr/CRsFileProcesser.h"
#include "CFileProcessMgr/CSkeFileProcesser.h"
#include "CFileProcessMgr/CAgpFileProcesser.h"
#include "CFileProcessMgr/CModFileProcesser.h"
#include "../CFileProcessMgr/CFxFileProcesser.h"
#include "CPathMgr.h"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "CodeCvs.h"

struct FilterParam
{
	int								init_progress;
	CProgressCtrlImp*				progressCtrlImp;
	CFileProcessCtrlImp*			pFileProcessCtrlImp;
	CFileProcessCtrlImp::FilesMap*	files_map_ptr;
};

FTW_RESULT FilterAllFiles(const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *filter_param_in)
{
	if( eFlag == _FTW_DIR )
	{
		if ( !_wcsicmp(szFileName + wcslen( szFileName ) - 4, L".svn") )
			return _FTW_IGNORE;
		else
			return _FTW_CONTINUNE;
	}

	typedef CFileProcessCtrlImp::FilesMap FilesMap;

	FilterParam*		filter_param	= (FilterParam*)filter_param_in;
	CProgressCtrlImp*	progressCtrlImp		= filter_param->progressCtrlImp;
	FilesMap&			files_map		= *(filter_param->files_map_ptr);

	filter_param->pFileProcessCtrlImp->SetCurTaskText(string("搜集文件 - ") + utf16_to_gbk(szFileName));

	FilesMap::iterator it = files_map.begin();
	for ( ; it!=files_map.end(); ++it )
	{
		wstring file_ext(szFileName);
		if ( file_ext.rfind(L".") == wstring::npos )
			break;

		file_ext = file_ext.substr(file_ext.rfind(L"."), file_ext.length());

		CTaskProcess* task = it->first;
		CTaskProcess::ExtNameSet ext_name_set = task->GetExtNames();
		if ( ext_name_set.find(file_ext) != ext_name_set.end() )
		{
			it->second.push_back(backslash_to_slash(wstring(szFileName)));
		}
	}

	// 进度条更新
	const int init_progress = filter_param->init_progress;
	const int pre_progress  = filter_param->progressCtrlImp->GetProgressValue();
	const int progress_left = filter_param->progressCtrlImp->GetProgressMaxValue() - pre_progress;
	int       new_progress  = pre_progress; 
	++new_progress;
	if( progress_left == 0 )
		new_progress = 0;

	filter_param->progressCtrlImp->SetProgressValue(new_progress);
	filter_param->progressCtrlImp->Update();

	return _FTW_CONTINUNE;
} 

DEF_UI_IMP(CFileProcessCtrlImp);
CFileProcessCtrlImp::CFileProcessCtrlImp( void* param ) 
{
	m_pCurProgressImp	= NULL;
	m_pTotalProgressImp = NULL;
	m_process_mgr		= NULL;
	
	this->InitProcesserList();
}

CFileProcessCtrlImp::~CFileProcessCtrlImp( void )
{	
	this->UnInitProcesserList();

	SafeDelete(m_process_mgr);
}

void CFileProcessCtrlImp::InitProcesserList()
{
	m_process_mgr		= new CFileProcessMgr(); 

	string strProcesserName("");

	CRsFileProcesser *pRSFileProcesser = new CRsFileProcesser;
	strProcesserName = pRSFileProcesser->GetName();
	m_processer_list.insert(make_pair(strProcesserName, pRSFileProcesser));

	CSkeFileProcesser *pSkeFileProcesser = new CSkeFileProcesser;
	strProcesserName = pSkeFileProcesser->GetName();
	m_processer_list.insert(make_pair(strProcesserName, pSkeFileProcesser));

	CAgpFileProcesser *pAgpFileProcesser = new CAgpFileProcesser;
	strProcesserName = pAgpFileProcesser->GetName();
	m_processer_list.insert(make_pair(strProcesserName, pAgpFileProcesser));

	CModFileProcesser *pModFileProcesser = new CModFileProcesser;
	strProcesserName = pModFileProcesser->GetName();
	m_processer_list.insert(make_pair(strProcesserName, pModFileProcesser));

	CFxFileProcesser *pFxFileProcesser = new CFxFileProcesser;
	strProcesserName = pFxFileProcesser->GetName();
	m_processer_list.insert(make_pair(strProcesserName, pFxFileProcesser));
}

void CFileProcessCtrlImp::UnInitProcesserList()
{
	for ( ProcesserMap::iterator it = m_processer_list.begin(); it != m_processer_list.end(); ++it )
		SafeDelete((it->second));

	m_processer_list.clear();
}

void CFileProcessCtrlImp::Update()
{
	CCtrlBase::Update();
}

void CFileProcessCtrlImp::GetProcesserList( vector<string>& list )
{
	string process("");
	ProcesserMap::iterator it = m_processer_list.begin();
	for ( ; it != m_processer_list.end(); ++it )
	{
		process = it->first;
		list.push_back(process);
	}
}

void CFileProcessCtrlImp::PreProcess(const vector<string>& list)
{
	this->SetCurTaskText("开始搜集所有文件信息...");

	m_files_map.clear();
	string str("");
	size_t size = list.size();
	for (size_t m = 0; m < size; ++m)
	{
		str = list[m];
		CTaskProcess* cur_process = m_processer_list[str];
		m_files_map.insert( make_pair(cur_process, vector<wstring>()) );
	}
}

void CFileProcessCtrlImp::GetAllFilesName(const vector<string>& paths)//选择路径
{
	if( paths.empty() )
		return;

	m_pCurProgressImp->SetProgressMaxValue(100);
	m_pCurProgressImp->Update();

	FilterParam filter_param;
	filter_param.files_map_ptr		= &m_files_map;
	filter_param.progressCtrlImp	= m_pCurProgressImp;
	filter_param.pFileProcessCtrlImp= this;
	filter_param.init_progress		= m_pCurProgressImp->GetProgressStep();

	size_t size = paths.size();
	for ( size_t m = 0; m < size; ++m )
	{
		string strPkg = paths[m];
		wstring ws = gbk_to_utf16(strPkg);
		FileTreeWalk(ws.c_str(), FilterAllFiles, &filter_param);
	}

	const int cur_progress  = m_pCurProgressImp->GetProgressValue();
	const int progress_left = m_pCurProgressImp->GetProgressMaxValue() - cur_progress;
	for ( int m = 0; m < progress_left; ++m )
	{
		m_pCurProgressImp->SetProgressValue(cur_progress+m+1);
		m_pCurProgressImp->Update();
	}
}

void CFileProcessCtrlImp::DoProcess( )
{
	string strFileName("");
	for ( FilesMap::iterator it = m_files_map.begin(); it != m_files_map.end(); ++it )
	{
		strFileName = it->first->GetName();
		this->SetCurTaskText(strFileName);
		m_process_mgr->Clear();
		m_process_mgr->RegistTask(it->first);
		m_process_mgr->SetFileList(it->second);
		m_process_mgr->Run(this, this->m_pTotalProgressImp);
	}
}


void CFileProcessCtrlImp::ResetUI()
{
	m_pCurProgressImp->SetProgressValue(0);
	m_pTotalProgressImp->SetProgressValue(0);
	this->SetCurTaskText("处理进度");

	m_pCurProgressImp->Update();
	m_pTotalProgressImp->Update();
}


bool CFileProcessCtrlImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_FILES_PROCESS:
				{
					MSG_INFO info = msg.msgInfo;
					vector<vector<string>> msgInfos;
					msgInfos = *(vector<vector<string>>*)(info._handle);

					vector<string> paths = msgInfos[0];
					vector<string> pkgs  = msgInfos[1];
					
					this->ResetUI();
					this->PreProcess(pkgs);
					this->GetAllFilesName(paths);
					this->DoProcess();
					break;
				}
			}
		}
	}
	return false;
}

void CFileProcessCtrlImp::SetCurProgressImp(CProgressCtrlImp* pProgressImp) 
{
	m_pCurProgressImp = pProgressImp;
}

void CFileProcessCtrlImp::SetTotalProgressImp(CProgressCtrlImp* pProgressImp) 
{
	m_pTotalProgressImp = pProgressImp;
}

void CFileProcessCtrlImp::SetCurTaskText( const string& text )
{
	m_strCurTaskText = text;
	if( !m_strCurTaskText.empty() )
		this->Update();
}

std::string CFileProcessCtrlImp::GetCurTaskText()
{
	return m_strCurTaskText;
}
