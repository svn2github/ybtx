#pragma once


namespace sqr
{
	class CQueryJob;
	class CQueryJobSubQueue;

	class CQueryJobQueue
	{
	public:
		CQueryJobQueue(size_t	stSlotNum);
		~CQueryJobQueue(void);

		void Push(CQueryJob* pJob);
		CQueryJob* Pop();

	private:

		CQueryJobSubQueue*		m_aryJobSubQueue;

		size_t const			m_stSlotNum;

		int						m_nCurInSlotIndex;
		int volatile			m_nCurOutSlotIndex;
	};

}

