#pragma once

#include "CCtrlBase.h"


namespace sqr_tools
{
	#define	MGT_FILES_PROCESS	0x01

	class CFileProcessMgr;
	class CTaskProcess;
	class CProgressCtrlImp;

	class CFileProcessCtrlImp : public CCtrlBase
	{
		REG_UI_IMP(CFileProcessCtrlImp);
	public:
		CFileProcessCtrlImp(void* param);
		~CFileProcessCtrlImp(void);

	public:
		typedef map<string, CTaskProcess*>				ProcesserMap;
		typedef map<CTaskProcess*, vector<wstring> >	FilesMap;

	private:
		CFileProcessMgr*		m_process_mgr;
		ProcesserMap			m_processer_list;
		FilesMap				m_files_map;				// 存储文件过滤器指针以及其对应要处理的所有文件

		CProgressCtrlImp*		m_pCurProgressImp;
		CProgressCtrlImp*		m_pTotalProgressImp;

		string					m_strCurTaskText;

		virtual void	InitProcesserList();
		virtual void	UnInitProcesserList();
		void			GetAllFilesName(const vector<string>& paths);
		void			PreProcess(const vector<string>& list);

	public:
		virtual void	SetCurProgressImp(CProgressCtrlImp* pProgressImp);
		virtual void	SetTotalProgressImp(CProgressCtrlImp* pProgressImp);
		virtual	void	SetCurTaskText(const string& text);
		virtual string	GetCurTaskText();

		virtual void	Update();
		virtual void	GetProcesserList(vector<string>& list);
		virtual void	DoProcess();
		virtual void	ResetUI();
		
		virtual bool	_ProcMsg(const CMsg& msg );
	};
}