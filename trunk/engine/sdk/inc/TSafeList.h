#pragma once
#include "TSafeContainerBase.h"
#include <list>

namespace sqr
{
	template < typename T, typename Allocator = std::allocator<T> >
	class TSafeList : public TSafeContainerBase< TSafeList<T, Allocator>, std::list<T, Allocator> >
	{
	public:
		typedef std::list<T, Allocator>										base_container;
		typedef TSafeList<T, Allocator>										this_type;
		typedef TSafeContainerBase<this_type, base_container>		base_type;
		typedef typename base_type::iterator								iterator;
		typedef typename base_container::value_type							value_type;
		typedef typename base_container::reference							reference;
	public:
#ifdef _WIN32
		friend base_type;
#else
		friend class TSafeContainerBase< TSafeList<T, Allocator>, std::list<T, Allocator> >;
#endif
	public:
		TSafeList();
		template <typename InputIterator>
		TSafeList(InputIterator first, InputIterator last);
		TSafeList(const iterator& first, const iterator& last);
		TSafeList(const base_container& other);
		TSafeList(this_type& other);
	public:
		reference front();
		reference back();
		iterator erase(const value_type& data);
		iterator erase(iterator position);
		void push_front(const value_type& x);
		void pop_front();
		void push_back(const value_type& x);
		void pop_back ();
		iterator insert(const iterator& position, const value_type& x);
		void insert(const iterator& position, size_t n, const value_type& x);
		template <class InputIterator>
		void insert (const iterator& position, InputIterator first, InputIterator last);
	protected:
		void commit_erase();
	};
}
