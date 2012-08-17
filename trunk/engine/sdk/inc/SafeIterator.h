#pragma once

namespace sqr
{
	/// @brief 比较迭代器时的漂移方向
	enum EIteratorCompareShift
	{
		EIteratorCompareShift_None,	///< 不以任何方式移动比较，发现任何一方无效则不等
		EIteratorCompareShift_Raw,	///< 以最原始的方式比较
		EIteratorCompareShift_Add,	///< 向右移动比较，会调用operator++
		EIteratorCompareShift_Sub,	///< 向左移动比较，会调用operator--
	};
	/// @brief 确保指针有效时的调整方向
	enum EIteratorCheckShift
	{
		EIteratorCheckShift_None,	///< 不做任何调整
		EIteratorCheckShift_Add,	///< 向右调整，会调用operator++
		EIteratorCheckShift_Sub,	///< 向左调整，会调用operator--
	};

	/// @brief 顺序安全迭代器的实现
	template<typename TSafeContainer>
	class CSafeIterator
	{
	protected:
		typedef TSafeContainer									host_type;
		typedef CSafeIterator<host_type>						this_type;
		typedef typename host_type::base_iterator				base_iterator;
		typedef this_type iterator;
	public:
		typedef bidirectional_iterator_tag						iterator_category;
		typedef typename base_iterator::value_type				value_type;
		typedef typename base_iterator::difference_type			difference_type;
		typedef typename base_iterator::pointer					pointer;
		typedef typename base_iterator::reference				reference;
	private:
		base_iterator	m_baseIt;
		host_type&		m_parent;
	public:
		CSafeIterator(const base_iterator& it, host_type& parent);
		CSafeIterator(const iterator& other);
		~CSafeIterator();
		base_iterator get_base_iterator() const;
		bool is_valid()const;
		iterator& check_valid(EIteratorCheckShift shift);
		iterator& operator=(const iterator& other);
		iterator& operator++(); // preincrement
		iterator operator++(int); // postdecrement
		iterator& operator--(); // predecrement
		iterator operator--(int); // postdecrement
		bool operator!=(const iterator& other)const;
		bool operator==(const iterator& other)const;
		bool equal(const iterator& other, EIteratorCompareShift shift)const;
		pointer operator->()const;
		reference operator*()const;
	};

	/// @brief 引用计数的简单维护
	class CSafeIteratorRefCount
	{
	private:
		int m_referenceCount;
	protected:
		CSafeIteratorRefCount();
		~CSafeIteratorRefCount();
		void add_ref();
		void sub_ref();
		int get_ref()const;
		void reset_ref();
	};

}
