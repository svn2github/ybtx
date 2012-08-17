#pragma once
#include "CFileProcessMgr/CTaskProcess.h"

namespace sqr_tools
{
	class CRsFileProcesser : public CTaskProcess
	{
	public:
		CRsFileProcesser();
		~CRsFileProcesser();

		virtual void	Do( const wchar_t* szfilename );
		const string	GetName() const;
		ExtNameSet		GetExtNames();
	};

}