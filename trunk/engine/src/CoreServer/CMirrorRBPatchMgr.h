#pragma once
#include "CMirrorMallocObject.h"

namespace sqr
{
	class CMirrorBuffer;
	class CMirrorRBPatch;

	//rb patch mgr 里面就只有一个patch
	class CMirrorRBPatchMgr : public CMirrorMallocObject
	{
	public:
		CMirrorRBPatchMgr();
		~CMirrorRBPatchMgr();

		CMirrorRBPatch* GetCurPatch();

		void Patch(CMirrorBuffer* pBuffer);

	private:
		CMirrorRBPatch*		m_pPatch;
	};

}
