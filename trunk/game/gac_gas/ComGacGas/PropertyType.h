#pragma once
#include <typeinfo>
#include "TFighterAllocator.h"

class CBasePropertyHolder : public  CFighterMallocObject
{
protected:
	virtual ~CBasePropertyHolder(){};
};

class AnyProperty
{
public:
	struct content
	{
		virtual ~content(){}
	};
	template<typename _U>
	struct Impl : public content
	{
		Impl(const _U& u)
			:_u(u)
		{

		}
		_U _u;
	};

	template<typename _U>
	AnyProperty(const _U& c)
		:_pc(new Impl<_U>(c))
	{

	}
	~AnyProperty()
	{
		delete _pc;
	}

	template<typename _T>
	_T& get()
	{
		Impl<_T>* p = dynamic_cast<Impl<_T>* >(_pc);
		if (0 == p)
		{
			throw bad_cast();
		}
		return p->_u;
	}
private:
	content* _pc;
};

