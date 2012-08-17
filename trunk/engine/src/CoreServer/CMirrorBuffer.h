#pragma once
#include "CMirrorMallocObject.h"

#define INVALID_TRIPLEBUFFER_SLOT ((uint32)-1)

namespace sqr{
	
	class CMirrorBufferData;

	class CTrMirrorBuffer;

	class CTripleBufferSlotMgr;

	template<typename Type, class IDContainer>
	class TIDStack;

	class CMirrorBuffer : public CMirrorMallocObject
	{
	public:
		friend class CTestSuiteTripleBuffer;

		CMirrorBuffer(uint32 uDataSize, uint32 uDataNum, CTrMirrorBuffer* pMgr);
		virtual ~CMirrorBuffer();

		CTrMirrorBuffer* GetBufferMgr();

		uint64	GetVersion();
		void	SetVersion(uint64 uVersion);
		// 版本号自增1
		void	IncVersion();  

		void*	AllocData(uint32 uSlot);
		void	ReleaseData(uint32 uSlot);

		uint32	WithdrawIdleSlot();
		void	RestoreIdleSlot(uint32 uSlot);
		void	RemoveIdleSlot(uint32 uSlot);
		bool	IsIdleSlot(uint32 uSlot);
		
		//交换两个buffer里面内容
		virtual void	Swap(CMirrorBuffer* pOtBuf);
	
		void*	GetBuffer(uint32 uSlot);

	protected:
		void	ExpandBuffer();
		uint32	GetIdleSlotNum()const;
		
		void AppendBuffer(size_t stSize);

	protected:
		uint64	m_uVersion;

		typedef vector<char, CMirrorMallocAllocator<char> >	VecBuffer_t;
		VecBuffer_t*	m_pVecBuffer;

		uint32	m_uDataSize;
	
		CTrMirrorBuffer*	m_pMgr;

		typedef vector<uint32, CMirrorMallocAllocator<uint32> > VecID_t;
		typedef TIDStack<uint32, VecID_t >	TIDStack_t;	
		TIDStack_t*	m_pIDStack;
	};

}
