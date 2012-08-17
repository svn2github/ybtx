#pragma once
#include "CMirrorMallocObject.h"

namespace sqr
{
	class CTrMirrorBuffer;

	class CCoreTrMirrorBufferMgr : public CMirrorMallocObject
	{
	public:
		friend class CAsynCorePos;
		friend class CSyncCorePos;
		friend class CAsynCoreQuadObject;
		friend class CAsynCoreQuadScene;
		friend class CSyncCoreQuadObject;
		friend class CSyncCoreQuadScene;

		static void Init();
		static void Unit();
		
		static CCoreTrMirrorBufferMgr*& Inst();
		
		void OnRBUpdate();

		void OnWBUpdate();

		CTrMirrorBuffer* GetPosBufferMgr() { return m_pPosBufferMgr; }
		CTrMirrorBuffer* GetQuadBufferMgr() { return m_pQuadBufferMgr; }

	private:
		CCoreTrMirrorBufferMgr();
		~CCoreTrMirrorBufferMgr();

	private:
		CTrMirrorBuffer*	m_pPosBufferMgr;
		CTrMirrorBuffer*	m_pQuadBufferMgr;

	};

}
