#pragma once

#include "TCoreObjAllocator.h"

namespace sqr
{
	class CSyncCoreObjectServer;

	class CObjDestoryState
		:public CCoreObjMallocObject
	{
		friend class CSyncCoreObjectServer;
	private:
		CObjDestoryState(void);
		~CObjDestoryState(void);

		bool DelRef();
		uint32		m_uRefNum;	
	};
};

