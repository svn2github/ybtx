#pragma once
#include "ThreadTypes.h"

namespace sqr
{

	template<typename ValueType>
	class TTlsPtr
	{
	public:
		TTlsPtr(void);
		~TTlsPtr(void);

		operator ValueType*()const;
		ValueType* operator =(ValueType* p);
		ValueType* operator->()const;
	private:
		HTLSKEY	m_hKey;
		stack<ValueType*>	m_stkPtr;
		HSPINLOCK			m_hLock;
	};

}
