#pragma once
#include "CFileProcessMgr/CTaskProcess.h"

namespace sqr_tools
{
	class CAgpFileProcesser : public CTaskProcess
	{
	public:
		CAgpFileProcesser();
		~CAgpFileProcesser();

		virtual void	Do( const wchar_t* szfilename );
		const string	GetName() const;
		ExtNameSet		GetExtNames();
	};

}