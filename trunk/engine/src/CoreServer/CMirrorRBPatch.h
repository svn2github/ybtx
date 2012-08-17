#pragma once
#include "CMirrorPatch.h"
#include "CMirrorMallocObject.h"

namespace sqr
{
	class CMirrorBuffer;
	
	class CMirrorRBPatchMgr;
	class CMirrorPatchCmd;

	class CMirrorRBPatch : public CMirrorPatch
						 , public CMirrorMallocObject
	{
	public:
		void AddCmd(CMirrorPatchCmd* pCmd);
		
	private:
		friend class CMirrorRBPatchMgr;

		CMirrorRBPatch();
		~CMirrorRBPatch();

		void Patch(CMirrorBuffer* pBuffer);
	
	private:
		/*typedef deque<CMirrorPatchCmd*, 
			CMirrorMallocAllocator<CMirrorPatchCmd*> >	DequeCmd_t;*/

		class DequeCmd_t : public deque<CMirrorPatchCmd*, 
			CMirrorMallocAllocator<CMirrorPatchCmd*> >
			, public CMirrorMallocObject
		{

		};

		DequeCmd_t*						m_pQueCmd;
		DequeCmd_t*						m_pSwapQueCmd;
	};

}
