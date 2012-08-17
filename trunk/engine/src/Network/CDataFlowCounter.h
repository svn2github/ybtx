#pragma once


namespace sqr
{

	struct CDataFlowUnit
	{
		size_t	m_stDataSize;

		union
		{
			uint64	m_uTime;
			uint64	m_uBeginTime;
		};
	};

	//5Ãë¼ÆÁ÷Á¿Æ÷
	class CDataFlowCounter
	{
	public:
		CDataFlowCounter();

		void DataFlow( size_t stSize ,uint64 uNow );

		float GetFlowSpeed( uint64 uNow );

	private:
		enum{ stArySize=10 };
		CDataFlowUnit	m_aryFlowUnit[stArySize];
		size_t			m_stIndex;
	};

}
