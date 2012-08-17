#pragma once
#include "CDynamicObject.h"


namespace sqr
{
	class CTickImp;
	class CTickMgr;
	class CTestSuiteTickMgr;

	class CTick
		:public virtual CDynamicObject
	{
	public:
		CTick(void);
		
		virtual ~CTick(void);
		virtual void OnTick();

		bool Registered()const;

		void UnRegister();

		uint32 GetInterval()const;

		void SetTickName(const char* szName);
		const char* GetTickName()const;

		virtual const char* GetTickClassName()const;

		uint32 GetLeftTime()const;

	private:
		CTickImp*	m_pImp;

		friend class CTickImp;
		friend class CTickMgr;
		friend class CTestSuiteTickMgr;
	};
}
