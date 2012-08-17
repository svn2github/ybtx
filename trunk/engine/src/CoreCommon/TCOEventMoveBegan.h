#pragma once
#include "TBaseCOEvent.h"

namespace sqr
{
	template<typename Traits>
	class TCOEventMoveBegan
		:public TBaseCOEvent<Traits>
	{
	public:
		typedef TBaseCOEvent<Traits>				Parent_t;
		typedef typename Traits::CoreObject_t		CoreObj_t;
		TCOEventMoveBegan(CoreObj_t* pObj);
	private:
		~TCOEventMoveBegan();
	};

}

