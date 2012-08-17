#pragma once

#include "CCtrlBase.h"

namespace sqr
{
	struct PkgDebugInfo;
}

namespace sqr_tools
{
	class CReadPkgCtrlImp : public CCtrlBase
	{
		REG_UI_IMP(CReadPkgCtrlImp);
	public:
		CReadPkgCtrlImp(void* param);
		~CReadPkgCtrlImp(void);

		bool GetFileInfo(const string& filepath, PkgDebugInfo* pdebugInfo);
		void ReadPkgFiles(const vector<string> pkgs);
	};
}