#pragma once
#include "CMirrorMallocObject.h"

namespace sqr
{
	class CTrMirrorBuffer;

	class CBaseMirrorData
		: public CMirrorMallocObject
	{
	public:
		uint32 GetSlot() const { return m_uSlot; }

	protected:
		CBaseMirrorData(CTrMirrorBuffer* pMgr) : m_uSlot(0), m_pMgr(pMgr) {}
		CBaseMirrorData(uint32 uSlot, CTrMirrorBuffer*	pMgr) : m_uSlot(uSlot), m_pMgr(pMgr) {}

		void SetSlot(uint32 uSlot) { m_uSlot = uSlot; }
	
	protected:
		uint32				m_uSlot;
		CTrMirrorBuffer*	m_pMgr;
	};

	template<typename T>
	class TBaseRBData : public CBaseMirrorData
	{
	protected:
		TBaseRBData(CTrMirrorBuffer* pMgr) : CBaseMirrorData(pMgr){}

		T* GetData() const;

	};

	template<typename T>
	class TBaseWBData : public CBaseMirrorData
	{
	protected:
		TBaseWBData(uint32 uSlot, CTrMirrorBuffer* pMgr) : CBaseMirrorData(uSlot,pMgr){}

		T* GetData() const;
	};

}
