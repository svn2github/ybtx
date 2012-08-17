#pragma once
#include "TSafeContainerBase.h"
#include <map>

namespace sqr
{
template < class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<pair<const Key,T> > >
class TSafeMap : public TSafeContainerBase< TSafeMap<Key, T, Compare, Allocator>, std::map<Key, T, Compare, Allocator> >
{
public:
	typedef std::map< Key, T, Compare, Allocator >				base_container;
	typedef TSafeMap< Key, T, Compare, Allocator >				this_type;
	typedef TSafeContainerBase<
	TSafeMap<Key, T, Compare, Allocator>,
	std::map<Key, T, Compare, Allocator> >				base_type;
	typedef typename base_type::iterator						iterator;
	typedef typename base_container::value_type					value_type;
	typedef typename base_container::key_type					key_type;
	typedef typename base_type::size_type						size_type;
	typedef typename base_container::reference					reference;
public:
#ifdef _WIN32
	friend base_type;
#else
	friend class TSafeContainerBase<
				TSafeMap<Key, T, Compare, Allocator>,
				std::map<Key, T, Compare, Allocator> >;
#endif
public:
	TSafeMap();
	template<typename InputIterator>
	TSafeMap(InputIterator first, InputIterator last);
	TSafeMap(iterator first, iterator last);
	TSafeMap(const base_container& other);
	TSafeMap(this_type& other);
public:
	void erase(const iterator& position);
	size_type erase(const key_type& x);
	iterator find(const key_type& x);
	size_type count(const key_type& x);
	pair<iterator, bool> insert(const key_type& key, const T& value);
	pair<iterator, bool> insert(const value_type& x);
	T& operator[](const key_type& x);
protected:
	void commit_erase();
};
}
