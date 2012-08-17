#pragma once
#include "CFileProcessMgr/CTaskProcess.h"

namespace sqr_tools
{
	class CFxFileProcesser : public CTaskProcess
	{
	public:
		virtual void	Do( const wchar_t* szfilename );
		const string	GetName() const;
		ExtNameSet		GetExtNames();
	};
}