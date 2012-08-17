#pragma once
#include "SafeIterator.inl"
#include "TSafeContainerBase.h"

namespace sqr
{
	template<typename TContainer>
	TEraseNote<TContainer>::TEraseNote()
	{
	}
	template<typename TContainer>
	bool TEraseNote<TContainer>::exist(const content_type& it)
	{
		return this->find(get_key(it)) != this->end();	
	}
	template<typename TContainer>
	bool TEraseNote<TContainer>::insert(const content_type& it)
	{
		return base_type::insert(make_pair(get_key(it), it)).second;
	}
	template<typename TContainer>
	bool TEraseNote<TContainer>::erase(const content_type& it)
	{
		iterator iter = this->find(get_key(it));
		if(iter == this->end())
			return false;
		base_type::erase(iter);
		return true;
	}
	template<typename TContainer>
	typename TEraseNote<TContainer>::key_type TEraseNote<TContainer>::get_key(const content_type& it)
	{
		return &(*it);
	}

	template <typename TSafeContainer, typename TBaseContainer>
	TSafeContainerBase<TSafeContainer, TBaseContainer>::TSafeContainerBase()
	{
	}
	template <typename TSafeContainer, typename TBaseContainer>
	const typename TSafeContainerBase<TSafeContainer, TBaseContainer>::base_container& TSafeContainerBase<TSafeContainer, TBaseContainer>::get_base_container()const
	{
		return m_baseData;
	}
	template <typename TSafeContainer, typename TBaseContainer>
	const typename TSafeContainerBase<TSafeContainer, TBaseContainer>::erase_note& TSafeContainerBase<TSafeContainer, TBaseContainer>::get_erase_note()const
	{
		return m_eraseNote;
	}
	template <typename TSafeContainer, typename TBaseContainer>
	void TSafeContainerBase<TSafeContainer, TBaseContainer>::commit(bool force/* = false*/)
	{
		if(get_ref() == 0 || force)
		{
			if(m_eraseNote.empty() == false)
			{
				static_cast<sub_type*>(this)->commit_erase();
				m_eraseNote.clear();
				reset_ref();
				//cout << "commit run" << endl;
			}
		}
	}
	template <typename TSafeContainer, typename TBaseContainer>
	size_t TSafeContainerBase<TSafeContainer, TBaseContainer>::size() const
	{
		return m_baseData.size() - m_eraseNote.size();
	}
	template <typename TSafeContainer, typename TBaseContainer>
	bool TSafeContainerBase<TSafeContainer, TBaseContainer>::empty()const
	{
		return size() == 0;
	}
	template <typename TSafeContainer, typename TBaseContainer>
	void TSafeContainerBase<TSafeContainer, TBaseContainer>::clear()
	{
		if(empty() == false)
		{
			for(base_iterator it = m_baseData.begin(); it != m_baseData.end(); ++it)
				m_eraseNote.insert(it);
		}
	}
	template <typename TSafeContainer, typename TBaseContainer>
	typename TSafeContainerBase<TSafeContainer, TBaseContainer>::iterator TSafeContainerBase<TSafeContainer, TBaseContainer>::begin()
	{
		return iterator(m_baseData.begin(), *static_cast<sub_type*>(this)).check_valid(EIteratorCheckShift_Add);
	}
	template <typename TSafeContainer, typename TBaseContainer>
	typename TSafeContainerBase<TSafeContainer, TBaseContainer>::iterator TSafeContainerBase<TSafeContainer, TBaseContainer>::end()
	{
		return iterator(m_baseData.end(), *static_cast<sub_type*>(this));
	}
	template <typename TSafeContainer, typename TBaseContainer>
	void TSafeContainerBase<TSafeContainer, TBaseContainer>::base_erase(const iterator& position)
	{
#ifdef _DEBUG
		bool find = false;
		for(base_iterator it = m_baseData.begin(); it != m_baseData.end(); ++it)
		{
			if(it == position.get_base_iterator())
			{
				find = true;
				break;
			}
		}
		Ast(find && "³¢ÊÔÉ¾³ý²»´æÔÚµÄµü´úÆ÷");
#endif
		m_eraseNote.insert(position.get_base_iterator());
	}
}
