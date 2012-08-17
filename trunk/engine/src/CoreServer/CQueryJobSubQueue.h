#pragma once
#include "ThreadTypes.h"


namespace sqr
{

	class CQueryJob;

	class CQueryJobSubQueue
	{
	public:
		CQueryJobSubQueue(void);
		~CQueryJobSubQueue(void);

		void Push( CQueryJob* pJob );
		CQueryJob* Pop();

	private:
		CQueryJob*				m_pIn;
		CQueryJob*				m_pOut;

		//虽然这里有锁，但是几乎100%不会冲撞，所以还是称之为无所队列
		HSPINLOCK				m_hLock;
	};

}

