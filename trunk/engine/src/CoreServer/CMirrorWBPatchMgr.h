#pragma once
#include "CMirrorMallocObject.h"

namespace sqr
{
	class CMirrorBuffer;
	class CMirrorWBPatch;

	class CMemPool;

	template<typename T>
	class TMemPoolAllocator;

	class CMirrorPatchCmd;

	class CMirrorWBPatchMgr : public CMirrorMallocObject
	{
	public:
		CMirrorWBPatchMgr();
		~CMirrorWBPatchMgr();

		//创建一个指定版本的patch
		CMirrorWBPatch* CreatePatch(uint64 uVersion);
		
		//删除一个指定版本的patch
		void DeletePatch(uint64 uVersion);

		//删除某一个范围里面的patch
		void DeletePatch(uint64 uMinVersion, uint64 uMaxVersion);

		//得到当前版本的patch，如果没有，则创建一个
		CMirrorWBPatch* GetCurPatch(uint64 uVersion);
		
		//上某一个版本的patch
		void Patch(uint64 uVersion, CMirrorBuffer* pBuffer);
		
		//上某一个范围里面的patch
		void Patch(uint64 uMinVersion, uint64 uMaxVersion, CMirrorBuffer* pBuffer);

	private:
		CMirrorWBPatch* GetPatch(uint64 uVersion);
		
		void IntDeletePatch(CMirrorWBPatch* pPatch);

	private:
		typedef deque<CMirrorWBPatch*, 
			CMirrorMallocAllocator<CMirrorWBPatch*> >	DequePatch_t;

		DequePatch_t					m_quePatch; //把patch保存在queue里面，
		//queue里面第一个patch的版本号是最小的，最后一个是最大的

		typedef TMemPoolAllocator<CMirrorPatchCmd*>	MemPool_t;

		stack<MemPool_t*, deque<MemPool_t*, 
			CMirrorMallocAllocator<MemPool_t*> > >				m_stkMemPool;  //mem pool的对象池
	};
}
