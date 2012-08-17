#pragma once
#include "SafeIterator.h"
#include <cassert>
#include "ExpHelper.h"

namespace sqr
{
	template<typename TSafeContainer>
	CSafeIterator<TSafeContainer>::CSafeIterator(const base_iterator& it, host_type& parent)
		: m_baseIt(it)
		, m_parent(parent)
	{
		m_parent.add_ref();
	}
	template<typename TSafeContainer>
	CSafeIterator<TSafeContainer>::CSafeIterator(const iterator& other)
		: m_baseIt(other.m_baseIt)
		, m_parent(other.m_parent)
	{
		m_parent.add_ref();
	}
	template<typename TSafeContainer>
	CSafeIterator<TSafeContainer>::~CSafeIterator()
	{
		m_parent.sub_ref();
		m_parent.commit();
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::base_iterator CSafeIterator<TSafeContainer>::get_base_iterator() const
	{
		return m_baseIt;
	}
	template<typename TSafeContainer>
	bool CSafeIterator<TSafeContainer>::is_valid()const
	{
		if(m_baseIt == m_parent.m_baseData.end())
			return false;
		else
			return ! m_parent.m_eraseNote.exist(m_baseIt);
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator& CSafeIterator<TSafeContainer>::check_valid(EIteratorCheckShift shift)
	{
		if(is_valid())
			return *this;
		switch(shift)
		{
		case EIteratorCheckShift_None:
			return *this;
		case EIteratorCheckShift_Add:
			if(m_baseIt == m_parent.m_baseData.end())
				return *this;
			else
				return ++*this;
		case EIteratorCheckShift_Sub:
			return --*this;
		default:
			Ast(false);
			return *this;
		}
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator& CSafeIterator<TSafeContainer>::operator=(const iterator& other)
	{
		if(&other != this)
		{
			m_baseIt = other.m_baseIt;
			m_parent = other.m_parent;
		}
		return *this;
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator& CSafeIterator<TSafeContainer>::operator++() // preincrement
	{ 
		if(m_baseIt == m_parent.m_baseData.end())
		{
			Ast(false && "尝试对end迭代器进行++操作");
			return *this;
		}
		for(;;)
		{
			if(++m_baseIt == m_parent.m_baseData.end())
				return *this;
			else if(m_parent.m_eraseNote.exist(m_baseIt))
				continue;
			else
				return *this;
		}
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator CSafeIterator<TSafeContainer>::operator++(int) // postdecrement
	{
		iterator now = *this;
		++*this;
		return now;
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator& CSafeIterator<TSafeContainer>::operator--() // predecrement
	{	
		for(;;)
		{
			if(m_baseIt == m_parent.m_baseData.begin())
			{
				Ast(false && "尝试对begin迭代器进行--操作");
				return (*this);
			}
			if(m_parent.m_eraseNote.exist(--m_baseIt))
				continue;
			else
				return *this;
		}
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::iterator CSafeIterator<TSafeContainer>::operator--(int) // postdecrement
	{	
		iterator now = *this;
		--*this;
		return now;
	}
	template<typename TSafeContainer>
	bool CSafeIterator<TSafeContainer>::operator!=(const iterator& other)const
	{
		return !equal(other, EIteratorCompareShift_Add);
	}
	template<typename TSafeContainer>
	bool CSafeIterator<TSafeContainer>::operator==(const iterator& other)const
	{
		return equal(other, EIteratorCompareShift_Add);
	}
	template<typename TSafeContainer>
	bool CSafeIterator<TSafeContainer>::equal(const iterator& other, EIteratorCompareShift shift)const
	{
		switch(shift)
		{
		case EIteratorCompareShift_None:
			return this->is_valid() && other.is_valid() && this->m_baseIt == other.m_baseIt;
		case EIteratorCompareShift_Add:
			if(this->is_valid())
			{
				if(other.is_valid())
					return this->m_baseIt == other.m_baseIt;
				else
					return this->m_baseIt == iterator(other).check_valid(EIteratorCheckShift_Add).m_baseIt;
			}
			else
			{
				if(other.is_valid())
					return iterator(*this).check_valid(EIteratorCheckShift_Add).m_baseIt == other.m_baseIt;
				else
					return iterator(*this).check_valid(EIteratorCheckShift_Add).m_baseIt 
					== iterator(other).check_valid(EIteratorCheckShift_Add).m_baseIt;
			}
		case EIteratorCompareShift_Sub:
			if(this->is_valid())
			{
				if(other.is_valid())
					return this->m_baseIt == other.m_baseIt;
				else
					return this->m_baseIt == iterator(other).check_valid(EIteratorCheckShift_Sub).m_baseIt;
			}
			else
			{
				if(other.is_valid())
					return iterator(*this).check_valid(EIteratorCheckShift_Sub).m_baseIt == other.m_baseIt;
				else
					return iterator(*this).check_valid(EIteratorCheckShift_Sub).m_baseIt 
					== iterator(other).check_valid(EIteratorCheckShift_Sub).m_baseIt;
			}
		case EIteratorCompareShift_Raw:
			return this->m_baseIt == other.m_baseIt;
		default:
			Ast(false);
			return false;
		}
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::pointer CSafeIterator<TSafeContainer>::operator->()const
	{
		Ast(is_valid() && "尝试对非法的迭代器取值");
		return m_baseIt.operator->();
	}
	template<typename TSafeContainer>
	typename CSafeIterator<TSafeContainer>::reference CSafeIterator<TSafeContainer>::operator*()const
	{
		Ast(is_valid() && "尝试对非法的迭代器取值");
		return m_baseIt.operator*();
	}

	CSafeIteratorRefCount::CSafeIteratorRefCount()
		: m_referenceCount(0)
	{
	}
	CSafeIteratorRefCount::~CSafeIteratorRefCount()
	{
		Ast(m_referenceCount == 0 && "引用计数维护错误");
	}
	void CSafeIteratorRefCount::add_ref()
	{
		++m_referenceCount;
		//for(int i = 0; i < m_referenceCount; i++) std::cout << "    ";
		//std::cout << "+ref=>" << m_referenceCount << std::endl;
	}
	void CSafeIteratorRefCount::sub_ref()
	{
		Ast(m_referenceCount >= 1 && "引用计数维护错误"); 
		--m_referenceCount;
		//for(int i = 0; i < m_referenceCount; i++) std::cout << "    ";
		//std::cout << "-ref=>" << m_referenceCount << std::endl;
	}
	int CSafeIteratorRefCount::get_ref()const
	{
		return m_referenceCount;
	}
	void CSafeIteratorRefCount::reset_ref()
	{
		m_referenceCount = 0;
	}
}
