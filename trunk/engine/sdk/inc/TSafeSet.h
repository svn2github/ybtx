#pragma once
#include "TSafeContainerBase.h"
#include <set>

namespace sqr
{
template < class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key> >
class TSafeSet : public TSafeContainerBase< TSafeSet<Key, Compare, Allocator>, std::set<Key, Compare, Allocator> >
{
public:
	typedef std::set< Key, Compare, Allocator>					base_container;
	typedef TSafeSet< Key, Compare, Allocator>					this_type;
	typedef TSafeContainerBase<
	TSafeSet<Key, Compare, Allocator>,
	std::set<Key, Compare, Allocator> >						base_type;
	typedef typename base_type::iterator						iterator;
	typedef typename base_container::value_type					value_type;
	typedef typename base_type::size_type						size_type;
	typedef typename base_container::reference					reference;
public:
#ifdef _WIN32
	friend class base_type;
#else
	friend class TSafeContainerBase<
				TSafeSet<Key, Compare, Allocator>,
				std::set<Key, Compare, Allocator> >;
#endif
public:
	TSafeSet();
	template<typename InputIterator>
	TSafeSet(InputIterator first, InputIterator last);
	TSafeSet(iterator first, iterator last);
	TSafeSet(const base_container& other);
	TSafeSet(this_type& other);
public:
	void erase(iterator& position);
	size_type erase(const value_type& x);
	iterator find(const value_type& x);
	bool exist(const value_type& x);
	size_type count(const value_type& x);
	pair<iterator,bool> insert(const value_type& x);
protected:
	void commit_erase();
};
}
