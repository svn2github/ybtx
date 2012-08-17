#pragma once
#include "TBaseCOEvent.h"
#include "CoreObjectDefs.h"


namespace sqr
{

	template<typename Traits>
	class TCOEventMoveEnded
		:public TBaseCOEvent<Traits>
	{
	public:
		typedef TBaseCOEvent<Traits>				Parent_t;
		typedef typename Traits::CoreObject_t		CoreObj_t;
		TCOEventMoveEnded(CoreObj_t* pObj,EMoveEndedType eMEType);
	private:
		~TCOEventMoveEnded();

		EMoveEndedType m_eMEType;
	};

}

