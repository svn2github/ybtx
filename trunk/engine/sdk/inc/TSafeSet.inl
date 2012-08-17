#pragma once
#include "TSafeContainerBase.inl"
#include "TSafeSet.h"

namespace sqr
{
	template < class Key, class Compare, class Allocator >
		TSafeSet<Key, Compare, Allocator>::TSafeSet()
		{
		}
	template < class Key, class Compare, class Allocator >
		template<typename InputIterator>
		TSafeSet<Key, Compare, Allocator>::TSafeSet(InputIterator first, InputIterator last)
		{
			this->m_baseData.insert(first, last);
		}
	template < class Key, class Compare, class Allocator >
		TSafeSet<Key, Compare, Allocator>::TSafeSet(iterator first, iterator last)
		{
			for(iterator it = first; it != last; ++it)
				this->m_baseData.insert(*(it.get_base_iterator()));
		}
	template < class Key, class Compare, class Allocator >
		TSafeSet<Key, Compare, Allocator>::TSafeSet(const base_container& other)
		{
			this->m_baseData.insert(other.begin(), other.end());
		}
	template < class Key, class Compare, class Allocator >
		TSafeSet<Key, Compare, Allocator>::TSafeSet(this_type& other)
		{
			for(iterator it = other.begin(); it != other.end(); ++it)
				this->m_baseData.insert(*(it.get_base_iterator()));
		}
	template < class Key, class Compare, class Allocator >
		void TSafeSet<Key, Compare, Allocator>::erase(iterator& position)
		{
			if(position != this->end())
				base_erase(position);
		}
	template < class Key, class Compare, class Allocator >
		typename TSafeSet<Key, Compare, Allocator>::size_type TSafeSet<Key, Compare, Allocator>::erase(const value_type& x)
		{
			iterator it = find(x);
			if(it == this->end())
				return 0;
			base_erase(it);
			return 1;
		}
	template < class Key, class Compare, class Allocator >
		typename TSafeSet<Key, Compare, Allocator>::iterator TSafeSet<Key, Compare, Allocator>::find(const value_type& x)
		{
			typename base_type::base_iterator it = this->m_baseData.find(x);
			if(it != this->m_baseData.end() && this->m_eraseNote.exist(it) == false)
				return iterator(it, *this);
			return this->end();
		}
	template < class Key, class Compare, class Allocator >
		bool TSafeSet<Key, Compare, Allocator>::exist(const value_type& x)
		{
			return find(x) != this->end();
		}
	template < class Key, class Compare, class Allocator >
		typename TSafeSet<Key, Compare, Allocator>::size_type TSafeSet<Key, Compare, Allocator>::count(const value_type& x)
		{
			return exist(x) ? 1 : 0;
		}
	template < class Key, class Compare, class Allocator >
		std::pair<typename TSafeSet<Key, Compare, Allocator>::iterator, bool> TSafeSet<Key, Compare, Allocator>::insert(const value_type& x)
		{
			typename base_type::base_iterator it = this->m_baseData.find(x);
			if(it == this->m_baseData.end())
			{
				pair<typename base_type::base_iterator, bool> ret = this->m_baseData.insert(x);
				return make_pair(iterator(ret.first, *this), ret.second);
			}
			else if(this->m_eraseNote.exist(it))
			{
				this->m_eraseNote.erase(it);
				return make_pair(iterator(it, *this), true);
			}
			else
			{
				return make_pair(iterator(it, *this), false);
			}
		}
	template < class Key, class Compare, class Allocator >
		void TSafeSet<Key, Compare, Allocator>::commit_erase()
		{
			for(typename base_type::erase_note::iterator it = this->m_eraseNote.begin(); it != this->m_eraseNote.end(); ++it)
				this->m_baseData.erase(it->second);
		}
}
