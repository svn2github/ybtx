#pragma once
#include "TBaseCOEvent.h"


namespace sqr
{
	template<typename Traits>
	class TCOEventPosChanged
		:public TBaseCOEvent<Traits>
	{
	public:
		typedef TBaseCOEvent<Traits>				Parent_t;
		typedef typename Traits::CoreObject_t		CoreObj_t;
		TCOEventPosChanged(CoreObj_t* pObj);
	private:
		~TCOEventPosChanged();
	};
}

