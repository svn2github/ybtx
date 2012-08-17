#pragma once
#include "CFileFormatMgr.h"

namespace sqr_tools
{
	class CDemoFormat : public CFileFormatMgr
	{
		// friend class CAnyObject;
	public:
		CDemoFormat(void);
		~CDemoFormat(void);

		static void TestMe(void);
	};
}