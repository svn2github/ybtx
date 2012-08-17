#pragma once
#include "TSafeContainerBase.inl"
#include "TSafeMap.h"

namespace sqr
{
	template < class Key, class T, class Compare, class Allocator >
		TSafeMap<Key, T, Compare, Allocator>::TSafeMap()
		{
		}
	template < class Key, class T, class Compare, class Allocator >
		template<typename InputIterator>
		TSafeMap<Key, T, Compare, Allocator>::TSafeMap(InputIterator first, InputIterator last)
		{
			this->m_baseData.insert(first, last);
		}
	template < class Key, class T, class Compare, class Allocator >
		TSafeMap<Key, T, Compare, Allocator>::TSafeMap(iterator first, iterator last)
		{
			for(iterator it = first; it != last; ++it)
				this->m_baseData.insert(*(it.get_base_iterator()));
		}
	template < class Key, class T, class Compare, class Allocator >
		TSafeMap<Key, T, Compare, Allocator>::TSafeMap(const base_container& other)
		{
			this->m_baseData.insert(other.begin(), other.end());
		}
	template < class Key, class T, class Compare, class Allocator >
		TSafeMap<Key, T, Compare, Allocator>::TSafeMap(this_type& other)
		{
			for(iterator it = other.begin(); it != other.end(); ++it)
				this->m_baseData.insert(*(it.get_base_iterator()));
		}
	template < class Key, class T, class Compare, class Allocator >
		void TSafeMap<Key, T, Compare, Allocator>::erase(const iterator& position)
		{
			if(position != this->end())
				this->base_erase(position);
		}
	template < class Key, class T, class Compare, class Allocator >
		typename TSafeMap<Key, T, Compare, Allocator>::size_type TSafeMap<Key, T, Compare, Allocator>::erase(const key_type& x)
		{
			iterator it = find(x);
			if(it == this->end())
				return 0;
			base_erase(it);
			return 1;
		}
	template < class Key, class T, class Compare, class Allocator >
		typename TSafeMap<Key, T, Compare, Allocator>::iterator TSafeMap<Key, T, Compare, Allocator>::find(const key_type& x)
		{
			typename base_type::base_iterator it = this->m_baseData.find(x);
			if(it != this->m_baseData.end() && this->m_eraseNote.exist(it) == false)
				return iterator(it, *this);
			return this->end();
		}
	template < class Key, class T, class Compare, class Allocator >
		typename TSafeMap<Key, T, Compare, Allocator>::size_type TSafeMap<Key, T, Compare, Allocator>::count(const key_type& x)
		{
			return find(x) == this->end() ? 0 : 1;
		}
	template < class Key, class T, class Compare, class Allocator >
		std::pair<typename TSafeMap<Key, T, Compare, Allocator>::iterator, bool> TSafeMap<Key, T, Compare, Allocator>::insert(const key_type& key, const T& value)
		{
			return insert(make_pair(key, value));
		}
	template < class Key, class T, class Compare, class Allocator >
		std::pair<typename TSafeMap<Key, T, Compare, Allocator>::iterator, bool> TSafeMap<Key, T, Compare, Allocator>::insert(const value_type& x)
		{
			typename base_type::base_iterator it = this->m_baseData.find(x.first);
			if(it == this->m_baseData.end())
			{
				pair<typename base_type::base_iterator, bool> ret = this->m_baseData.insert(x);
				return make_pair(iterator(ret.first, *this), ret.second);
			}
			else if(this->m_eraseNote.exist(it))
			{
				it->second = x.second;
				this->m_eraseNote.erase(it);
				return make_pair(iterator(it, *this), true);
			}
			else
			{
				return make_pair(iterator(it, *this), false);
			}
		}
	template < class Key, class T, class Compare, class Allocator >
		T& TSafeMap<Key, T, Compare, Allocator>::operator[](const key_type& x)
		{
			return (*((this->insert(make_pair(x,T()))).first)).second;
		}
	template < class Key, class T, class Compare, class Allocator >
		void TSafeMap<Key, T, Compare, Allocator>::commit_erase()
		{
			for(typename base_type::erase_note::iterator it = this->m_eraseNote.begin(); it != this->m_eraseNote.end(); ++it)
				this->m_baseData.erase(it->second);
		}
}
