#pragma once

namespace sqr 
{
	template<typename T_Key, typename T_Value, typename T_CompFunc,
		typename KeyAlloc = std::allocator<T_Key>,
		typename ValueAlloc = std::allocator<T_Value> >

	class TBinaryHeap 
	{
	public:
		TBinaryHeap(uint32 nInitSize = 4);
		~TBinaryHeap();

		bool empty()const;
		const T_Value& top()const;
		uint32	size()const;

		void push(T_Key key, const T_Value& value);
		void pop();
		void clear();
	protected:
		inline void move(uint32 destIndex, uint32 srcIndex);
		void re_allot();

		uint32			m_nNum;
		uint32			m_nAllotSize;

		typedef vector<T_Key, KeyAlloc>		VecKey_t;
		typedef vector<T_Value, ValueAlloc>	VecValue_t;

		VecKey_t		m_vecKeyHeap;
		VecValue_t		m_vecValueHeap;

		T_CompFunc		m_Compare;
	};
}


