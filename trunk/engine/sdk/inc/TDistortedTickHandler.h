#pragma once

#include "CDistortedTick.h"

namespace sqr
{
	template<typename HolderType>
	class TDistortedTickHandler
		:public CDistortedTick
	{
	private:
		HolderType*	m_pHolder;
	public:
		void*		m_pArg;

		TDistortedTickHandler(HolderType* pHolder,void* pArg = NULL)
			:m_pHolder(pHolder),m_pArg(pArg)
		{
		}

		virtual void OnTick()
		{
			return m_pHolder->OnTick(this);
		}
	};

	template<typename HolderType, void(HolderType::*pFun)() >
	class TDistortedTickFunctionHandler
		:public CDistortedTick
	{
	private:
		HolderType*	m_pHolder;
	public:

		TDistortedTickFunctionHandler(HolderType* pHolder)
			:m_pHolder(pHolder)
		{
		}

		virtual void OnTick()
		{
			return (m_pHolder->*pFun)();
		}
	};
}
