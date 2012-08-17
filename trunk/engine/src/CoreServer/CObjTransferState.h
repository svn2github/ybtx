#pragma once
#include "CPos.h"
#include "TCoreObjAllocator.h"

namespace sqr
{
	class CSyncCoreSceneServer;

	class CObjTransferState
		:public CCoreObjMallocObject
	{
		friend class CSyncCoreObjectServer;
	private:
		CObjTransferState(CSyncCoreSceneServer*	pTargetScene, CFPos m_PixelPos);
		~CObjTransferState(void);

		bool DelRef();
		CSyncCoreSceneServer*	m_pTargetScene;
		CFPos					m_PixelPos;
		uint32					m_uRefNum;
	};

}

