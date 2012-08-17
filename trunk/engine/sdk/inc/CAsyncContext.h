#pragma once


namespace sqr
{

	class CAsyncContext
	{
	public:
		CAsyncContext();
		~CAsyncContext();

		void SetContext(void* pContext);
		void* GetContext()const;
	private:
		void*	m_pContext;
	};


}

