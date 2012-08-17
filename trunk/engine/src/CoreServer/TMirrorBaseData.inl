#pragma once
#include "TMirrorBaseData.h"
#include "CTrMirrorBuffer.h"
#include "CMirrorBuffer.h"

template<typename T>
inline T* TBaseRBData<T>::GetData() const
{
	CMirrorBuffer* pBuffer = this->m_pMgr->GetReadBuffer();
	return static_cast<T*>(pBuffer->GetBuffer(m_uSlot));
}

template<typename T>
inline T* TBaseWBData<T>::GetData() const
{
	CMirrorBuffer* pBuffer = this->m_pMgr->GetWriteBuffer();
	return static_cast<T*>(pBuffer->GetBuffer(m_uSlot));
}
