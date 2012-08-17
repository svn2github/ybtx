#pragma once
#include "SafeIterator.h"
#include <map>

namespace sqr
{
	class CTestSuiteSafeContainer;

	/// @brief 扩充hash_map，提供exist和insert方法，方便删除记录的定义和使用
	template<typename TContainer>
	class TEraseNote : public map<const void*, typename TContainer::iterator>
	{
	public:
		typedef TEraseNote<TContainer>									this_type;
		typedef map<const void*, typename TContainer::iterator>			base_type;
		typedef typename base_type::key_type							key_type;
		typedef typename base_type::value_type 							value_type;
		typedef typename base_type::iterator							iterator;
		typedef typename base_type::const_iterator						const_iterator;
		typedef typename TContainer::iterator							content_type;
	public:
		TEraseNote();
	private:
		// 防止拷贝构造和赋值操作
		TEraseNote(const this_type& other);
		TEraseNote& operator=(const this_type& other);
	public:
		bool exist(const content_type& it);
		bool insert(const content_type& it);
		bool erase(const content_type& it);
	public:
		static key_type get_key(const content_type& it);
	};

	/// @brief 安全容器基类
	template <typename TSafeContainer, typename TBaseContainer>
	class TSafeContainerBase : public CSafeIteratorRefCount
	{
	protected:
		typedef TSafeContainer											sub_type;
		typedef TBaseContainer											base_container;
		typedef TSafeContainerBase<sub_type, base_container>			this_type;
		typedef TEraseNote<base_container>								erase_note;
	public:
		typedef typename base_container::iterator                       base_iterator;
		typedef typename base_container::value_type						value_type;
		typedef typename base_container::reference						reference;
		typedef CSafeIterator<sub_type>									iterator;
		typedef typename base_container::size_type						size_type;
	public:
		friend class CSafeIterator<TSafeContainer>;
		friend class CTestSuiteSafeContainer;	// for unit test
	protected:
		base_container	m_baseData;
		erase_note		m_eraseNote;
	public:
		TSafeContainerBase();
	private:
		// 防止拷贝构造和赋值操作
		TSafeContainerBase(const this_type& other);
		TSafeContainerBase& operator=(const this_type& other);
	protected:
		const base_container& get_base_container()const;
		const erase_note& get_erase_note()const;
	public:
		void commit(bool force = false);
	public:
		size_t size() const;
		bool empty()const;
		void clear();
		iterator begin();
		iterator end();
	protected:
		void base_erase(const iterator& position);
	};
}
