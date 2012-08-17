#pragma once
#include "CFileProcessMgr/CTaskProcess.h"

namespace sqr_tools
{
	class CModFileProcesser : public CTaskProcess
	{
	public:
		CModFileProcesser();
		~CModFileProcesser();

		virtual void	Do( const wchar_t* szfilename );
		const string	GetName() const;
		ExtNameSet		GetExtNames();
	};

}