#pragma once
#include "ISend.h"


namespace sqr
{
	template<typename TargetType,void (TargetType::*SendFunc)(const void*,uint32)const>
	class TSendAdapter
		:public ISend
	{
	public:
		TSendAdapter(const TargetType* pTarget)
			:m_pTarget(pTarget)
		{
		}

		TSendAdapter(void)
			:m_pTarget(NULL)
		{
		}

		~TSendAdapter(void)
		{
		}

		const TargetType* GetTarget()const
		{
			return m_pTarget;
		}

		void SetTarget(const TargetType* pTarget)
		{
			m_pTarget = pTarget;
		}
	private:
		const TargetType*	m_pTarget;

		uint32 Send(const void* pData,uint32 uSize)
		{
			if(!m_pTarget)
				return 0;
			(m_pTarget->*SendFunc)(pData,uSize);
			return uSize;
		}
	};

}


