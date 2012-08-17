#pragma once
#include "CCoreMsg.h"
#include "PatternRef.h"

namespace sqr
{

	template<typename Traits>
	class TBaseCOEvent
		:public CCoreMsg
	{
	public:
		typedef TBaseCOEvent<Traits>				BaseEvent_t;
		typedef typename Traits::CoreObject_t		CoreObj_t;

		TBaseCOEvent(CoreObj_t* pObj);
	
		~TBaseCOEvent(void);
		CoreObj_t* GetCoreObj()const;
	private:
		TPtRefer<BaseEvent_t,CoreObj_t>		m_RefCoreObj;
	};

}

