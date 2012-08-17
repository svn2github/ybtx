#pragma once
#include "TBaseCOEvent.h"


namespace sqr
{

	template<typename Traits>
	class TCOEventMovePathChanged
		:public TBaseCOEvent<Traits>
	{
	public:
		typedef TBaseCOEvent<Traits>				Parent_t;
		typedef typename Traits::CoreObject_t		CoreObj_t;
		TCOEventMovePathChanged(CoreObj_t* pObj);
	private:
		~TCOEventMovePathChanged();
	};

}

