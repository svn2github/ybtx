#pragma once

namespace sqr
{
	template<typename ValueType>
	class TIDPtrMap;

	template<typename ValueType>
	class TIDPtrMapIter
	{
		typedef TIDPtrMap<ValueType> Container;
	public:
		TIDPtrMapIter(uint32 uIndex = uint32(-1), Container* pContainer = NULL);

		ValueType& operator*()const;
		ValueType* operator->()const;
		TIDPtrMapIter& operator++();
		bool operator==(const TIDPtrMapIter& i)const;
		bool operator!=(const TIDPtrMapIter& i)const;
	private:
		uint32		m_uIndex;
		Container*	m_pContainer;

	};
}

