#include "TVariantStream.h"
#include "TSqrAllocator.inl"
#include "CMemData.inl"
#include "ErrLogHelper.h"

template<typename AllocObject>
TVariantStream<AllocObject>::TVariantStream()
:CMemData(AllocObject::GetPool())
,m_bFinalize(false)
,m_uCurPos(0)
{
}

template<typename AllocObject>
TVariantStream<AllocObject>::~TVariantStream()
{
}

template<typename AllocObject>
CBaseCypherVariantInputStream& TVariantStream<AllocObject>::GetInputStream()
{
	return *(static_cast<CBaseCypherVariantInputStream*>(this));
}

template<typename AllocObject>
CBaseCypherVariantOutputStream& TVariantStream<AllocObject>::GetOutputStream()
{
	return *(static_cast<CBaseCypherVariantOutputStream*>(this));
}

template<typename AllocObject>
void TVariantStream<AllocObject>::Finalize()
{
	m_bFinalize = true;
}

template<typename AllocObject>
void TVariantStream<AllocObject>::Write(const char* sData, size_t uSize)
{
	if (m_bFinalize)
	{
		LogErr("VariantStream::Read 已经Finalize, 不允许Write");
		return;
	}
	Append(sData, uSize);
}

template<typename AllocObject>
void TVariantStream<AllocObject>::ReadData(void*& Value, size_t uLen)
{
	Ast( m_uCurPos + uLen <= Size() );
	Value = (void*)(Data() + m_uCurPos);
	m_uCurPos += uLen;
}

template<typename AllocObject>
size_t TVariantStream<AllocObject>::RemainCount()
{
	if (Size() < m_uCurPos)
		return 0;
	return Size() - m_uCurPos;
}

template<typename AllocObject>
void TVariantStream<AllocObject>::GetLine(string& Value)
{
	Value = (Data() + m_uCurPos);
	m_uCurPos += Value.size()+1;
}

template<typename AllocObject>
void TVariantStream<AllocObject>::Clear()
{
	m_bFinalize = false;
	m_uCurPos = 0;
	CMemData::ReSize(0);
}

