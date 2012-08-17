#pragma once
#include "CFileProcessMgr/CTaskProcess.h"

namespace sqr_tools
{
	class CSkeFileProcesser : public CTaskProcess
	{
	public:
		CSkeFileProcesser();
		~CSkeFileProcesser();

		virtual void	Do( const wchar_t* szfilename );
		const string	GetName() const;
		ExtNameSet		GetExtNames();
	};

}