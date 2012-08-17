#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CCueListCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CCueListCtrlImp);

	public:
		CCueListCtrlImp(void* param);
		~CCueListCtrlImp(void);

		typedef map<string, vector<string>> cuePool;
		

	private:
		cuePool				m_cueList;
		string				m_strCurCueName;

	public:
 		virtual void	Update();
 		virtual bool	_ProcMsg(const CMsg& msg );

		virtual	void	CreateAudioSystem();
		virtual void	LoadMusicResource();

		virtual size_t	GetMusicTypeCount();
		virtual void	GetMusicTypeName(const size_t index, string& name);

		virtual size_t	GetEachMusicTypeCount(const size_t index);
		virtual void	GetCueName(const string& musictypename, const size_t index, string& name);

		virtual void	SetCurSelectCueName(const string& name);
		virtual void	GetCurSelectCueName(string& name);
		virtual const char* GetCurSelectCueName();

		virtual void	PlayCurSelectCue(const bool bPlay);
	};

}
