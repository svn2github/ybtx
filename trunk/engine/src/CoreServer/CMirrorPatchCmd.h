#pragma once

namespace sqr
{
	class CMirrorPatch;
	class CMirrorBuffer;

	class CMirrorPatchCmd
	{
	public:
		CMirrorPatchCmd(uint32 uSlot) : m_uSlot(uSlot){}
		virtual ~CMirrorPatchCmd(){};

		uint32 GetSlot() {  return m_uSlot; }

		virtual void HandleCmd(CMirrorBuffer* pBuffer) = 0;
		virtual void Release() = 0;

	protected:
		uint32	m_uSlot;
	};

	class CMirrorRBPatchCmd : public CMirrorPatchCmd
	{
	public:
		CMirrorRBPatchCmd(uint32 uSlot);
		
		static void* operator new(size_t stSize);
		static void operator delete(void* pMem, size_t stSize);

		virtual void HandleCmd(CMirrorBuffer* pBuffer) = 0;
		virtual void Release() = 0;
		virtual bool PatchEnabled(CMirrorBuffer* pBuffer);
		bool PatchDiscarded(CMirrorBuffer* pBuffer);
	};


	class CMirrorWBPatchCmd : public CMirrorPatchCmd
	{
	public:
		CMirrorWBPatchCmd(uint32 uSlot);
		
		static void* operator new(size_t stSize, CMirrorPatch* pPatch);
		static void operator delete(void* , CMirrorPatch* ){}
		static void operator delete(void*){}

		virtual void HandleCmd(CMirrorBuffer* pBuffer) = 0;
		virtual void Release() = 0;
	};

}
