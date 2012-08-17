#pragma once
#include "TSafeContainerBase.inl"
#include "TSafeList.h"

namespace sqr
{
	template < typename T, typename Allocator >
		TSafeList<T, Allocator>::TSafeList()
		{
		}
	template < typename T, typename Allocator >
		template <typename InputIterator>
		TSafeList<T, Allocator>::TSafeList(InputIterator first, InputIterator last)
		{
			this->m_baseData.assign(first, last);
		}
	template < typename T, typename Allocator >
		TSafeList<T, Allocator>::TSafeList(const iterator& first, const iterator& last)
		{
			for(iterator it = first; it != last; ++it)
				this->m_baseData.push_back(*it);
		}
	template < typename T, typename Allocator >
		TSafeList<T, Allocator>::TSafeList(const base_container& other)
		{
			this->m_baseData.assign(other.begin(), other.end());
		}
	template < typename T, typename Allocator >
		TSafeList<T, Allocator>::TSafeList(this_type& other)
		{
			for(iterator it = other.begin(); it != other.end(); ++it)
				this->m_baseData.push_back(*it);
		}
	template < typename T, typename Allocator >
		typename TSafeList<T, Allocator>::reference TSafeList<T, Allocator>::front()
		{   
			return *(this->begin());
		}   
	template < typename T, typename Allocator >
		typename TSafeList<T, Allocator>::reference TSafeList<T, Allocator>::back()
		{   
			return *iterator((++this->m_baseData.rbegin()).base(), *this).check_valid(EIteratorCheckShift_Sub);
		} 
	template < typename T, typename Allocator >
		typename TSafeList<T, Allocator>::iterator TSafeList<T, Allocator>::erase(const value_type& data)
		{
			for(iterator it = this->begin(); it != this->end(); ++it)
			{
				if(*it == data)
				{
					return erase(it);
				}
			}
			return this->end();
		}
	template < typename T, typename Allocator >
		typename TSafeList<T, Allocator>::iterator TSafeList<T, Allocator>::erase(iterator position)
		{
			if(position != this->end())
			{
				base_erase(position);
				return ++position;
			}
			return this->end();
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::push_front(const value_type& x)
		{
			this->m_baseData.push_front(x);
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::pop_front()
		{
			erase(this->begin());
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::push_back(const value_type& x)
		{
			this->m_baseData.push_back(x);
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::pop_back ()
		{
			erase(iterator((++this->m_baseData.rbegin()).base(), *this).check_valid(EIteratorCheckShift_Sub));
		}
	template < typename T, typename Allocator >
		typename TSafeList<T, Allocator>::iterator TSafeList<T, Allocator>::insert(const iterator& position, const value_type& x)
		{
			return iterator(this->m_baseData.insert(position.get_base_iterator(), x), *this);
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::insert(const iterator& position, size_t n, const value_type& x)
		{
			return iterator(this->m_baseData.insert(position.get_base_iterator(), n, x), *this);
		}
	template < typename T, typename Allocator >
		template <class InputIterator>
		void TSafeList<T, Allocator>::insert (const iterator& position, InputIterator first, InputIterator last)
		{
			this->m_baseData.insert<InputIterator>(position.get_base_iterator(), first, last);
		}
	template < typename T, typename Allocator >
		void TSafeList<T, Allocator>::commit_erase()
		{
			for(typename base_type::erase_note::iterator it = this->m_eraseNote.begin(); it != this->m_eraseNote.end(); ++it)
				this->m_baseData.erase(it->second);
		}
}
